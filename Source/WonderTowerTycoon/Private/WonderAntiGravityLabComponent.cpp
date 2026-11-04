// Copyright Darrell R. Lewis / Wonder World Trilogy. All Rights Reserved.

#include "WonderAntiGravityLabComponent.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "Engine/OverlapResult.h"

UWonderAntiGravityLabComponent::UWonderAntiGravityLabComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UWonderAntiGravityLabComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentGravityDirection = FVector::DownVector;
	TargetGravityDirection = FVector::DownVector;
	bInsideZeroGVolume = false;
}

void UWonderAntiGravityLabComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Zero-G physics volume damping
	if (bInsideZeroGVolume && GetOwner())
	{
		UPrimitiveComponent* RootPrim = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent());
		if (RootPrim && RootPrim->IsSimulatingPhysics())
		{
			// Counteract standard gravity and apply linear/angular zero-g drift
			RootPrim->AddForce(-GetWorld()->GetDefaultGravityZ() * FVector::UpVector, NAME_None, true);
			RootPrim->SetLinearDamping(ZeroGFrictionDamping);
		}
	}

	// Interpolate personal gravity orientation if shifting
	if (!CurrentGravityDirection.Equals(TargetGravityDirection, 0.01f))
	{
		UpdatePersonalGravityOrientation(DeltaTime);
	}

	// Update active black hole singularity pull
	if (bSingularityActive)
	{
		UpdateSingularityPull(DeltaTime);
	}
}

void UWonderAntiGravityLabComponent::EnterZeroGVolume()
{
	bInsideZeroGVolume = true;
	SetGravityMode(EGravityDriveMode::ZeroGChamber);
	OnZeroGVolumeStateChanged.Broadcast(true);

	UE_LOG(LogTemp, Log, TEXT("[WonderAntiGravity] Entered Zero-G Physics Chamber!"));
}

void UWonderAntiGravityLabComponent::ExitZeroGVolume()
{
	bInsideZeroGVolume = false;
	SetGravityMode(EGravityDriveMode::StandardEarth);
	OnZeroGVolumeStateChanged.Broadcast(false);

	UE_LOG(LogTemp, Log, TEXT("[WonderAntiGravity] Exited Zero-G Physics Chamber!"));
}

void UWonderAntiGravityLabComponent::SetGravityMode(EGravityDriveMode NewMode)
{
	if (CurrentMode == NewMode)
	{
		return;
	}

	CurrentMode = NewMode;

	switch (NewMode)
	{
	case EGravityDriveMode::StandardEarth:
		TargetGravityDirection = FVector::DownVector;
		break;
	case EGravityDriveMode::ZeroGChamber:
		TargetGravityDirection = FVector::ZeroVector;
		break;
	case EGravityDriveMode::InvertedSkyscraper:
		if (GetOwner())
		{
			TargetGravityDirection = -GetOwner()->GetActorForwardVector();
		}
		break;
	case EGravityDriveMode::SingularityVortex:
		break;
	}

	OnGravityModeChanged.Broadcast(CurrentMode);
	UE_LOG(LogTemp, Log, TEXT("[WonderAntiGravity] Mode set to %d"), static_cast<int32>(NewMode));
}

void UWonderAntiGravityLabComponent::ShiftPersonalGravityVector(FVector NewGravityDirection)
{
	TargetGravityDirection = NewGravityDirection.GetSafeNormal();
	CurrentMode = EGravityDriveMode::InvertedSkyscraper;

	OnPersonalGravityInverted.Broadcast(TargetGravityDirection);
	UE_LOG(LogTemp, Log, TEXT("[WonderAntiGravity] Personal gravity shifted to: %s"), *TargetGravityDirection.ToString());
}

void UWonderAntiGravityLabComponent::SpawnGravitonSingularity(FVector VortexLocation, float DurationSeconds)
{
	bSingularityActive = true;
	ActiveSingularityLocation = VortexLocation;
	SingularityTimeRemaining = DurationSeconds;
	CurrentMode = EGravityDriveMode::SingularityVortex;

	OnGravitonSingularitySpawned.Broadcast(VortexLocation, SingularityRadius);
	UE_LOG(LogTemp, Warning, TEXT("[WonderAntiGravity] GRAVITON SINGULARITY VORTEX ACTIVATED at %s for %.1fs"),
		*VortexLocation.ToString(), DurationSeconds);
}

void UWonderAntiGravityLabComponent::DismissSingularity()
{
	bSingularityActive = false;
	SingularityTimeRemaining = 0.0f;
	if (CurrentMode == EGravityDriveMode::SingularityVortex)
	{
		SetGravityMode(bInsideZeroGVolume ? EGravityDriveMode::ZeroGChamber : EGravityDriveMode::StandardEarth);
	}
}

void UWonderAntiGravityLabComponent::TuneHovercraftGravityInversion(float NewEfficiency, float NewAlignmentRate)
{
	InversionDriveEfficiency = FMath::Clamp(NewEfficiency, 0.5f, 2.5f);
	HovercraftInversionAlignmentRate = FMath::Max(1.0f, NewAlignmentRate);

	OnHovercraftInversionTuned.Broadcast(InversionDriveEfficiency, HovercraftInversionAlignmentRate);

	UE_LOG(LogTemp, Log, TEXT("[WonderAntiGravity] Hovercraft Inversion Drive Tuned: Efficiency %.2f | Alignment Rate %.1f"),
		InversionDriveEfficiency, HovercraftInversionAlignmentRate);
}

void UWonderAntiGravityLabComponent::UpdatePersonalGravityOrientation(float DeltaTime)
{
	CurrentGravityDirection = FMath::VInterpTo(CurrentGravityDirection, TargetGravityDirection, DeltaTime, GravityShiftInterpolationSpeed * InversionDriveEfficiency);

	if (GetOwner())
	{
		UPrimitiveComponent* RootPrim = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent());
		if (RootPrim && RootPrim->IsSimulatingPhysics())
		{
			FVector GravityImpulse = CurrentGravityDirection * 980.0f * DeltaTime;
			RootPrim->AddImpulse(GravityImpulse, NAME_None, true);
		}
	}
}

void UWonderAntiGravityLabComponent::UpdateSingularityPull(float DeltaTime)
{
	SingularityTimeRemaining -= DeltaTime;
	if (SingularityTimeRemaining <= 0.0f)
	{
		DismissSingularity();
		return;
	}

	if (!GetWorld())
	{
		return;
	}

	TArray<FOverlapResult> Overlaps;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(SingularityRadius);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	GetWorld()->OverlapMultiByChannel(Overlaps, ActiveSingularityLocation, FQuat::Identity, ECC_WorldDynamic, Sphere, Params);

	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* HitActor = Overlap.GetActor();
		if (HitActor)
		{
			UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(HitActor->GetRootComponent());
			if (Prim && Prim->IsSimulatingPhysics())
			{
				FVector ToVortex = ActiveSingularityLocation - HitActor->GetActorLocation();
				float Distance = ToVortex.Size();
				if (Distance > 50.0f)
				{
					float Falloff = 1.0f - FMath::Clamp(Distance / SingularityRadius, 0.0f, 1.0f);
					FVector PullForce = ToVortex.GetSafeNormal() * (SingularityPullForce * Falloff * 100.0f);
					FVector Tangent = FVector::CrossProduct(ToVortex.GetSafeNormal(), FVector::UpVector) * (SingularityPullForce * 0.35f);
					Prim->AddForce(PullForce + Tangent, NAME_None, true);
				}
			}
		}
	}
}
