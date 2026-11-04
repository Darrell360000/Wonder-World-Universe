// Copyright Darrell R. Lewis / Wonder World Trilogy. All Rights Reserved.

#include "WonderVehicleAcrobaticsComponent.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/OverlapResult.h"

UWonderVehicleAcrobaticsComponent::UWonderVehicleAcrobaticsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UWonderVehicleAcrobaticsComponent::BeginPlay()
{
	Super::BeginPlay();
	bAcrobaticAirDodgeAvailable = true;
}

void UWonderVehicleAcrobaticsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CurrentManeuver != EVehicleAcrobaticManeuver::None)
	{
		UpdateAcrobaticManeuver(DeltaTime);
	}
}

bool UWonderVehicleAcrobaticsComponent::HandleJumpInput(FVector DirectionalInput, bool bIsAirborne)
{
	float CurrentTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
	float TimeSinceLastJump = CurrentTime - LastJumpPressTimestamp;
	LastJumpPressTimestamp = CurrentTime;

	// Ground jump: register tap and reset air dodge
	if (!bIsAirborne)
	{
		bAcrobaticAirDodgeAvailable = true;
		return false;
	}

	// Airborne: check if double-tap occurred within double-tap window
	if (bAcrobaticAirDodgeAvailable && TimeSinceLastJump <= DoubleTapWindowSeconds)
	{
		bAcrobaticAirDodgeAvailable = false;

		// Determine maneuver by directional input
		float ForwardVal = DirectionalInput.X;
		float RightVal = DirectionalInput.Y;
		float UpVal = DirectionalInput.Z;

		if (UpVal < -0.5f)
		{
			TriggerManeuver(EVehicleAcrobaticManeuver::HeavyBodySlam);
		}
		else if (FMath::Abs(ForwardVal) >= FMath::Abs(RightVal))
		{
			if (ForwardVal >= 0.2f)
			{
				TriggerManeuver(EVehicleAcrobaticManeuver::ForwardFlip);
			}
			else if (ForwardVal <= -0.2f)
			{
				TriggerManeuver(EVehicleAcrobaticManeuver::Backflip);
			}
			else
			{
				TriggerManeuver(EVehicleAcrobaticManeuver::ForwardFlip);
			}
		}
		else
		{
			if (RightVal > 0.2f)
			{
				TriggerManeuver(EVehicleAcrobaticManeuver::BarrelRollRight);
			}
			else
			{
				TriggerManeuver(EVehicleAcrobaticManeuver::BarrelRollLeft);
			}
		}

		return true;
	}

	return false;
}

void UWonderVehicleAcrobaticsComponent::TriggerManeuver(EVehicleAcrobaticManeuver Maneuver)
{
	if (!GetOwner() || Maneuver == EVehicleAcrobaticManeuver::None)
	{
		return;
	}

	CurrentManeuver = Maneuver;
	ManeuverTimer = 0.0f;
	ManeuverDuration = (Maneuver == EVehicleAcrobaticManeuver::HeavyBodySlam) ? 1.0f : 0.65f;

	UPrimitiveComponent* RootPrim = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent());

	FVector OwnerForward = GetOwner()->GetActorForwardVector();
	FVector OwnerRight = GetOwner()->GetActorRightVector();
	FVector OwnerUp = GetOwner()->GetActorUpVector();

	if (RootPrim && RootPrim->IsSimulatingPhysics())
	{
		switch (Maneuver)
		{
		case EVehicleAcrobaticManeuver::ForwardFlip:
			RootPrim->AddImpulse(OwnerForward * FlipLinearImpulse, NAME_None, true);
			RootPrim->SetPhysicsAngularVelocityInDegrees(OwnerRight * FlipAngularVelocity, false);
			break;

		case EVehicleAcrobaticManeuver::Backflip:
			RootPrim->AddImpulse((-OwnerForward + OwnerUp * 0.5f) * FlipLinearImpulse, NAME_None, true);
			RootPrim->SetPhysicsAngularVelocityInDegrees(-OwnerRight * FlipAngularVelocity, false);
			break;

		case EVehicleAcrobaticManeuver::BarrelRollRight:
			RootPrim->AddImpulse(OwnerRight * FlipLinearImpulse, NAME_None, true);
			RootPrim->SetPhysicsAngularVelocityInDegrees(OwnerForward * FlipAngularVelocity, false);
			break;

		case EVehicleAcrobaticManeuver::BarrelRollLeft:
			RootPrim->AddImpulse(-OwnerRight * FlipLinearImpulse, NAME_None, true);
			RootPrim->SetPhysicsAngularVelocityInDegrees(-OwnerForward * FlipAngularVelocity, false);
			break;

		case EVehicleAcrobaticManeuver::HeavyBodySlam:
			RootPrim->AddImpulse(FVector::DownVector * BodySlamDownwardForce, NAME_None, true);
			break;

		default:
			break;
		}
	}

	OnVehicleAcrobaticManeuverStarted.Broadcast(CurrentManeuver);

	UE_LOG(LogTemp, Log, TEXT("[WonderAcrobatics] Performed Maneuver: %d"), static_cast<int32>(Maneuver));
}

void UWonderVehicleAcrobaticsComponent::ExecuteHeavyBodySlam()
{
	TriggerManeuver(EVehicleAcrobaticManeuver::HeavyBodySlam);
}

void UWonderVehicleAcrobaticsComponent::UpdateAcrobaticManeuver(float DeltaTime)
{
	ManeuverTimer += DeltaTime;

	if (CurrentManeuver == EVehicleAcrobaticManeuver::HeavyBodySlam)
	{
		DetectSlamImpactGround();
	}

	if (ManeuverTimer >= ManeuverDuration)
	{
		CurrentManeuver = EVehicleAcrobaticManeuver::None;
		ManeuverTimer = 0.0f;
	}
}

void UWonderVehicleAcrobaticsComponent::DetectSlamImpactGround()
{
	if (!GetOwner())
	{
		return;
	}

	FVector StartLoc = GetOwner()->GetActorLocation();
	FVector EndLoc = StartLoc + FVector::DownVector * 200.0f;

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());

	if (GetWorld() && GetWorld()->LineTraceSingleByChannel(HitResult, StartLoc, EndLoc, ECC_Visibility, QueryParams))
	{
		if (HitResult.bBlockingHit)
		{
			ApplyShockwaveToNearbyVehicles(HitResult.ImpactPoint);
			CurrentManeuver = EVehicleAcrobaticManeuver::None;
			ManeuverTimer = 0.0f;
		}
	}
}

void UWonderVehicleAcrobaticsComponent::ApplyShockwaveToNearbyVehicles(FVector ImpactOrigin)
{
	if (!GetWorld())
	{
		return;
	}

	TArray<FOverlapResult> Overlaps;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(BodySlamImpactRadius);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	GetWorld()->OverlapMultiByChannel(Overlaps, ImpactOrigin, FQuat::Identity, ECC_WorldDynamic, Sphere, Params);

	int32 HitCount = 0;
	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* HitActor = Overlap.GetActor();
		if (HitActor && HitActor != GetOwner())
		{
			UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(HitActor->GetRootComponent());
			if (PrimComp && PrimComp->IsSimulatingPhysics())
			{
				FVector LaunchDir = (HitActor->GetActorLocation() - ImpactOrigin).GetSafeNormal() + FVector::UpVector * 0.5f;
				FVector LaunchImpulse = LaunchDir.GetSafeNormal() * BodySlamKnockbackImpulse;

				PrimComp->AddImpulse(LaunchImpulse, NAME_None, true);
				OnSyndicateVehicleLaunched.Broadcast(HitActor, LaunchImpulse);
				HitCount++;
			}
		}
	}

	OnBodySlamLanded.Broadcast(ImpactOrigin, BodySlamImpactRadius, HitCount);

	UE_LOG(LogTemp, Warning, TEXT("[WonderAcrobatics] Heavy Body Slam Landed! Vehicles Launched: %d"), HitCount);
}
