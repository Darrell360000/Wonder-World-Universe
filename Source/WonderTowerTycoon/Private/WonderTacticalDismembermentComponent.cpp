#include "WonderTacticalDismembermentComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"

UWonderTacticalDismembermentComponent::UWonderTacticalDismembermentComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	InitializeDefaultZones();
}

void UWonderTacticalDismembermentComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UWonderTacticalDismembermentComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// If spinning out, apply continuous torque until grounded
	if (bIsSpinningOut && GetOwner())
	{
		if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent()))
		{
			if (Prim->IsSimulatingPhysics())
			{
				Prim->AddTorqueInDegrees(FVector(0.f, 0.f, SpinOutTorque * DeltaTime), NAME_None, true);
			}
		}
	}
}

void UWonderTacticalDismembermentComponent::InitializeDefaultZones()
{
	if (DamageZones.Num() > 0) return;

	auto AddZone = [this](EDismembermentZone Zone, FName BoneName, float MaxHealth)
	{
		FTacticalDamageZone NewZone;
		NewZone.Zone = Zone;
		NewZone.TargetBoneName = BoneName;
		NewZone.MaxZoneHealth = MaxHealth;
		NewZone.ZoneHealth = MaxHealth;
		NewZone.bIsSevered = false;
		DamageZones.Add(Zone, NewZone);
	};

	AddZone(EDismembermentZone::LeftHoverTurbine, FName("turbine_l"), 120.0f);
	AddZone(EDismembermentZone::RightHoverTurbine, FName("turbine_r"), 120.0f);
	AddZone(EDismembermentZone::LeftWeaponMount, FName("weapon_mount_l"), 85.0f);
	AddZone(EDismembermentZone::RightWeaponMount, FName("weapon_mount_r"), 85.0f);
	AddZone(EDismembermentZone::LeftKneeActuator, FName("knee_l"), 95.0f);
	AddZone(EDismembermentZone::RightKneeActuator, FName("knee_r"), 95.0f);
	AddZone(EDismembermentZone::PowerCore, FName("spine_03"), 220.0f);
}

void UWonderTacticalDismembermentComponent::ApplyBoneDamage(FName HitBoneName, float DamageAmount, FVector HitLocation)
{
	for (auto& Pair : DamageZones)
	{
		if (Pair.Value.TargetBoneName == HitBoneName)
		{
			ApplyZoneDamage(Pair.Key, DamageAmount, HitLocation);
			return;
		}
	}

	// Default body damage to power core if general torso hit
	ApplyZoneDamage(EDismembermentZone::PowerCore, DamageAmount * 0.5f, HitLocation);
}

void UWonderTacticalDismembermentComponent::ApplyZoneDamage(EDismembermentZone Zone, float DamageAmount, FVector HitLocation)
{
	FTacticalDamageZone* ZoneData = DamageZones.Find(Zone);
	if (!ZoneData || ZoneData->bIsSevered) return;

	ZoneData->ZoneHealth = FMath::Max(0.0f, ZoneData->ZoneHealth - DamageAmount);
	const float HealthPct = ZoneData->ZoneHealth / ZoneData->MaxZoneHealth;

	OnZoneDamaged.Broadcast(Zone, HealthPct);

	if (ZoneData->ZoneHealth <= 0.0f)
	{
		SeverZone(Zone, HitLocation);
	}
}

void UWonderTacticalDismembermentComponent::SeverZone(EDismembermentZone Zone, FVector HitLocation)
{
	FTacticalDamageZone* ZoneData = DamageZones.Find(Zone);
	if (!ZoneData || ZoneData->bIsSevered) return;

	ZoneData->bIsSevered = true;
	ZoneData->ZoneHealth = 0.0f;

	UE_LOG(LogTemp, Warning, TEXT("[WonderDismemberment] CRITICAL SEVER: Zone %d on '%s' has been severed!"),
		static_cast<int32>(Zone), GetOwner() ? *GetOwner()->GetName() : TEXT("Target"));

	// Hide or detach bone on skeletal mesh if available
	if (GetOwner())
	{
		if (USkeletalMeshComponent* SkelMesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>())
		{
			SkelMesh->HideBoneByName(ZoneData->TargetBoneName, PBO_None);
		}
	}

	// Trigger specific aerodynamic / systemic failure reactions
	switch (Zone)
	{
	case EDismembermentZone::LeftHoverTurbine:
	case EDismembermentZone::RightHoverTurbine:
		TriggerAerodynamicSpinOut(1.25f);
		break;

	case EDismembermentZone::LeftWeaponMount:
	case EDismembermentZone::RightWeaponMount:
		UE_LOG(LogTemp, Log, TEXT("[WonderDismemberment] Weapon mount destroyed. Firepower reduced."));
		break;

	case EDismembermentZone::LeftKneeActuator:
	case EDismembermentZone::RightKneeActuator:
		UE_LOG(LogTemp, Log, TEXT("[WonderDismemberment] Knee actuator shattered. Target dropped to posture-broken state!"));
		break;

	case EDismembermentZone::PowerCore:
		UE_LOG(LogTemp, Warning, TEXT("[WonderDismemberment] POWER CORE CATASTROPHIC BREACH!"));
		break;
	}

	OnZoneSevered.Broadcast(Zone, HitLocation);
}

void UWonderTacticalDismembermentComponent::TriggerAerodynamicSpinOut(float Intensity)
{
	bIsSpinningOut = true;

	if (GetOwner())
	{
		if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent()))
		{
			if (Prim->IsSimulatingPhysics())
			{
				Prim->AddAngularImpulseInDegrees(FVector(0.f, 0.f, SpinOutTorque * Intensity), NAME_None, true);
				Prim->AddImpulse(FVector(0.f, 0.f, -25000.0f * Intensity), NAME_None, true); // forced downward listing
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[WonderDismemberment] Aerodynamic spin-out triggered on '%s' (Intensity: %.2f)"),
		GetOwner() ? *GetOwner()->GetName() : TEXT("Target"), Intensity);

	OnAerodynamicSpinOutTriggered.Broadcast(Intensity);
}

bool UWonderTacticalDismembermentComponent::IsZoneSevered(EDismembermentZone Zone) const
{
	if (const FTacticalDamageZone* ZoneData = DamageZones.Find(Zone))
	{
		return ZoneData->bIsSevered;
	}
	return false;
}
