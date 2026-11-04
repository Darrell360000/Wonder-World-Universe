// Copyright Darrell R. Lewis / Wonder World Trilogy. All Rights Reserved.

#include "WonderShieldSawComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "GameFramework/ProjectileMovementComponent.h"

UWonderShieldSawComponent::UWonderShieldSawComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UWonderShieldSawComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner())
	{
		CurrentDiscLocation = GetOwner()->GetActorLocation();
	}
}

void UWonderShieldSawComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Update Parry Window Countdown
	if (CurrentState == EShieldSawState::HeavyParry)
	{
		HeavyParryTimer -= DeltaTime;
		if (HeavyParryTimer <= 0.0f)
		{
			// Fall back to active block if holding or idle
			SetSawState(EShieldSawState::ActiveBlock);
		}
	}
	else if (CurrentState == EShieldSawState::Thrown)
	{
		UpdateThrownFlight(DeltaTime);
	}
	else if (CurrentState == EShieldSawState::Recalling)
	{
		UpdateRecallFlight(DeltaTime);
	}
	else if (CurrentState == EShieldSawState::DoorCutting)
	{
		UpdateDoorCutting(DeltaTime);
	}
	else if (CurrentState == EShieldSawState::Idle || CurrentState == EShieldSawState::SpinningMelee || CurrentState == EShieldSawState::ActiveBlock)
	{
		if (GetOwner())
		{
			CurrentDiscLocation = GetOwner()->GetActorLocation();
		}
	}
}

void UWonderShieldSawComponent::SetSawState(EShieldSawState NewState)
{
	if (CurrentState == NewState)
	{
		return;
	}

	CurrentState = NewState;
	OnShieldSawStateChanged.Broadcast(NewState);

	UE_LOG(LogTemp, Log, TEXT("[WonderShieldSaw] State changed to: %d"), static_cast<int32>(NewState));
}

void UWonderShieldSawComponent::StartSpinningMelee()
{
	if (CurrentState == EShieldSawState::Idle)
	{
		SetSawState(EShieldSawState::SpinningMelee);
	}
}

void UWonderShieldSawComponent::StopSpinningMelee()
{
	if (CurrentState == EShieldSawState::SpinningMelee)
	{
		SetSawState(EShieldSawState::Idle);
	}
}

void UWonderShieldSawComponent::StartActiveBlock()
{
	if (CurrentState == EShieldSawState::Idle || CurrentState == EShieldSawState::SpinningMelee)
	{
		SetSawState(EShieldSawState::ActiveBlock);
	}
}

void UWonderShieldSawComponent::StopActiveBlock()
{
	if (CurrentState == EShieldSawState::ActiveBlock || CurrentState == EShieldSawState::HeavyParry)
	{
		SetSawState(EShieldSawState::Idle);
	}
}

bool UWonderShieldSawComponent::TriggerHeavyParry()
{
	if (CurrentState == EShieldSawState::Thrown || CurrentState == EShieldSawState::Recalling)
	{
		return false;
	}

	HeavyParryTimer = HeavyParryWindowDuration;
	SetSawState(EShieldSawState::HeavyParry);

	if (GetOwner())
	{
		FVector Forward = GetOwner()->GetActorForwardVector();
		OnHeavyParryTriggered.Broadcast(Forward, nullptr);
	}

	return true;
}

bool UWonderShieldSawComponent::CanParryHeavyAttack() const
{
	return CurrentState == EShieldSawState::HeavyParry && HeavyParryTimer > 0.0f;
}

bool UWonderShieldSawComponent::TryDeflectProjectile(AActor* IncomingProjectile, FVector ImpactPoint, FVector HitNormal)
{
	if (!IncomingProjectile || !IsActiveBlocking())
	{
		return false;
	}

	// Calculate reflection vector
	FVector ForwardDir = GetOwner() ? GetOwner()->GetActorForwardVector() : -HitNormal;
	FVector ReflectedVelocity = ForwardDir * (ThrowSpeed * DeflectedProjectileSpeedBoost);

	// If the projectile has a ProjectileMovementComponent, redirect its velocity
	if (UProjectileMovementComponent* ProjMove = IncomingProjectile->FindComponentByClass<UProjectileMovementComponent>())
	{
		ProjMove->Velocity = ReflectedVelocity;
	}
	else
	{
		IncomingProjectile->SetActorRotation(ReflectedVelocity.Rotation());
	}

	OnProjectileDeflected.Broadcast(IncomingProjectile, ReflectedVelocity);
	UE_LOG(LogTemp, Log, TEXT("[WonderShieldSaw] Projectile '%s' deflected with speed %.1f"),
		*IncomingProjectile->GetName(), ReflectedVelocity.Size());

	return true;
}

bool UWonderShieldSawComponent::ThrowShieldSaw(FVector AimDirection)
{
	if (CurrentState == EShieldSawState::Thrown || CurrentState == EShieldSawState::Recalling)
	{
		return false;
	}

	if (!GetOwner())
	{
		return false;
	}

	ThrowOrigin = GetOwner()->GetActorLocation();
	CurrentDiscLocation = ThrowOrigin;
	ThrownVelocity = AimDirection.GetSafeNormal() * ThrowSpeed;

	SetSawState(EShieldSawState::Thrown);
	return true;
}

void UWonderShieldSawComponent::RecallShieldSaw()
{
	if (CurrentState == EShieldSawState::Thrown)
	{
		SetSawState(EShieldSawState::Recalling);
	}
}

void UWonderShieldSawComponent::UpdateThrownFlight(float DeltaTime)
{
	CurrentDiscLocation += ThrownVelocity * DeltaTime;

	float DistanceTraveled = FVector::Dist(ThrowOrigin, CurrentDiscLocation);
	if (DistanceTraveled >= MaxThrowDistance)
	{
		RecallShieldSaw();
	}
}

void UWonderShieldSawComponent::UpdateRecallFlight(float DeltaTime)
{
	if (!GetOwner())
	{
		SetSawState(EShieldSawState::Idle);
		return;
	}

	FVector TargetPos = GetOwner()->GetActorLocation();
	FVector DirToOwner = (TargetPos - CurrentDiscLocation).GetSafeNormal();

	CurrentDiscLocation += DirToOwner * RecallSpeed * DeltaTime;

	if (FVector::Dist(CurrentDiscLocation, TargetPos) < 150.0f)
	{
		CurrentDiscLocation = TargetPos;
		SetSawState(EShieldSawState::Idle);
		OnShieldSawRecalled.Broadcast();
	}
}

void UWonderShieldSawComponent::StartCuttingDoor(AActor* TargetDoorActor, float CutTimeRequired)
{
	if (!TargetDoorActor || !GetOwner())
	{
		return;
	}

	float Dist = FVector::Dist(GetOwner()->GetActorLocation(), TargetDoorActor->GetActorLocation());
	if (Dist > CuttingRange)
	{
		return;
	}

	TargetedDoor = TargetDoorActor;
	DoorCutDuration = FMath::Max(0.5f, CutTimeRequired);
	DoorCutElapsedTime = 0.0f;
	DoorCuttingProgress = 0.0f;

	SetSawState(EShieldSawState::DoorCutting);
}

void UWonderShieldSawComponent::CancelDoorCutting()
{
	if (CurrentState == EShieldSawState::DoorCutting)
	{
		TargetedDoor.Reset();
		DoorCuttingProgress = 0.0f;
		DoorCutElapsedTime = 0.0f;
		SetSawState(EShieldSawState::Idle);
	}
}

void UWonderShieldSawComponent::UpdateDoorCutting(float DeltaTime)
{
	if (!TargetedDoor.IsValid())
	{
		CancelDoorCutting();
		return;
	}

	DoorCutElapsedTime += DeltaTime;
	DoorCuttingProgress = FMath::Clamp(DoorCutElapsedTime / DoorCutDuration, 0.0f, 1.0f);

	if (DoorCuttingProgress >= 1.0f)
	{
		AActor* BreachedActor = TargetedDoor.Get();
		TargetedDoor.Reset();
		SetSawState(EShieldSawState::Idle);

		OnDoorBreachCompleted.Broadcast(BreachedActor);
		UE_LOG(LogTemp, Log, TEXT("[WonderShieldSaw] Security door breached successfully!"));
	}
}

void UWonderShieldSawComponent::CarveBarricade(AActor* ObstacleActor, float DeltaTime)
{
	if (!ObstacleActor)
	{
		return;
	}

	float Damage = BaseDamagePerSecond * DeltaTime;
	OnBarricadeCarved.Broadcast(ObstacleActor, Damage);
	UE_LOG(LogTemp, Log, TEXT("[WonderShieldSaw] Carving barricade '%s' for %.1f damage"), *ObstacleActor->GetName(), Damage);
}
