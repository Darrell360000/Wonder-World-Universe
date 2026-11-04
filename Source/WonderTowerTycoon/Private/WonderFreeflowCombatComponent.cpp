#include "WonderFreeflowCombatComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/OverlapResult.h"

UWonderFreeflowCombatComponent::UWonderFreeflowCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UWonderFreeflowCombatComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UWonderFreeflowCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Combo Decay Timer
	if (CurrentCombo > 0)
	{
		ComboTimer -= DeltaTime;
		if (ComboTimer <= 0.0f)
		{
			ResetCombo();
		}
	}

	// Arkham-Style Target Snapping Interpolation
	if (bIsSnapping && GetOwner())
	{
		FVector CurrentLoc = GetOwner()->GetActorLocation();
		FVector NewLoc = FMath::VInterpTo(CurrentLoc, SnapDestination, DeltaTime, SnapInterpolationSpeed);
		GetOwner()->SetActorLocation(NewLoc, true);

		if (FVector::DistSquared(NewLoc, SnapDestination) < 900.0f) // within 30 units
		{
			bIsSnapping = false;
		}
	}
}

AActor* UWonderFreeflowCombatComponent::FindBestFreeflowTarget(FVector InputDirection) const
{
	AActor* Owner = GetOwner();
	if (!Owner || !GetWorld()) return nullptr;

	FVector ForwardDir = InputDirection.IsNearlyZero() ? Owner->GetActorForwardVector() : InputDirection.GetSafeNormal();
	FVector Origin = Owner->GetActorLocation();

	TArray<FOverlapResult> Overlaps;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(MaxSnapDistance);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner);

	GetWorld()->OverlapMultiByChannel(Overlaps, Origin, FQuat::Identity, ECC_Pawn, Sphere, Params);

	AActor* BestTarget = nullptr;
	float BestScore = -100.0f;

	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* Candidate = Overlap.GetActor();
		if (!Candidate || Candidate == Owner) continue;

		// Prioritize enemies or any other pawn in the combat arena
		FVector ToTarget = (Candidate->GetActorLocation() - Origin);
		float Dist = ToTarget.Size();
		if (Dist <= 0.001f || Dist > MaxSnapDistance) continue;

		FVector ToTargetNorm = ToTarget / Dist;
		float Dot = FVector::DotProduct(ForwardDir, ToTargetNorm);

		// Must be generally in front of player input direction (cone > 0.1)
		if (Dot > 0.1f)
		{
			float DistanceScore = 1.0f - (Dist / MaxSnapDistance);
			float TotalScore = (Dot * 0.65f) + (DistanceScore * 0.35f);

			if (TotalScore > BestScore)
			{
				BestScore = TotalScore;
				BestTarget = Candidate;
			}
		}
	}

	return BestTarget;
}

bool UWonderFreeflowCombatComponent::ExecuteDirectionalFreeflowStrike(FVector InputDirection)
{
	AActor* Owner = GetOwner();
	if (!Owner) return false;

	AActor* Target = FindBestFreeflowTarget(InputDirection);

	if (Target)
	{
		CurrentSnapTarget = Target;
		bIsSnapping = true;

		// Calculate snap destination just in front of target
		FVector ToTarget = (Target->GetActorLocation() - Owner->GetActorLocation()).GetSafeNormal();
		SnapDestination = Target->GetActorLocation() - (ToTarget * 105.0f);

		// Orient player toward target
		FRotator TargetRot = ToTarget.Rotation();
		TargetRot.Pitch = 0.0f;
		TargetRot.Roll = 0.0f;
		Owner->SetActorRotation(TargetRot);

		// Deal Arkham Freeflow Combo Damage
		const float Damage = BaseStrikeDamage * (1.0f + (CurrentCombo * 0.12f));
		UGameplayStatics::ApplyDamage(Target, Damage, nullptr, Owner, UDamageType::StaticClass());

		CurrentCombo++;
		ComboTimer = MaxComboWindowSeconds;

		UE_LOG(LogTemp, Log, TEXT("[WonderFreeflow] Strike Landed on '%s'! Combo: %d | Damage: %.1f"),
			*Target->GetName(), CurrentCombo, Damage);

		OnFreeflowStrikeLanded.Broadcast(Target, CurrentCombo, Damage);
		return true;
	}

	// Missed or whiff in air
	CurrentCombo = FMath::Max(0, CurrentCombo - 1);
	return false;
}

bool UWonderFreeflowCombatComponent::ExecuteBeatTimedParryCounter(float BeatOffsetTolerance)
{
	AActor* Owner = GetOwner();
	if (!Owner || !GetWorld()) return false;

	// 85 BPM beat period: 60 / 85 = 0.70588 seconds
	const float BeatInterval = 60.0f / 85.0f;
	const float TimeSeconds = GetWorld()->GetTimeSeconds();
	const float BeatPhase = FMath::Fmod(TimeSeconds, BeatInterval);

	const bool bIsOnBeat = (BeatPhase <= BeatOffsetTolerance) || (BeatPhase >= (BeatInterval - BeatOffsetTolerance));

	AActor* CounterTarget = FindBestFreeflowTarget(Owner->GetActorForwardVector());

	if (bIsOnBeat)
	{
		CurrentCombo += 2;
		ComboTimer = MaxComboWindowSeconds;

		if (CounterTarget)
		{
			const float CounterDamage = BaseStrikeDamage * 2.2f;
			UGameplayStatics::ApplyDamage(CounterTarget, CounterDamage, nullptr, Owner, UDamageType::StaticClass());
		}

		UE_LOG(LogTemp, Log, TEXT("[WonderFreeflow] PERFECT BEAT COUNTER! Combo boosted to %d (+2)"), CurrentCombo);
		OnBeatCounterExecuted.Broadcast(CounterTarget, true);
		return true;
	}

	// Off-beat counter
	if (CounterTarget)
	{
		UGameplayStatics::ApplyDamage(CounterTarget, BaseStrikeDamage, nullptr, Owner, UDamageType::StaticClass());
	}

	OnBeatCounterExecuted.Broadcast(CounterTarget, false);
	return false;
}

void UWonderFreeflowCombatComponent::ResetCombo()
{
	if (CurrentCombo > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("[WonderFreeflow] Combo chain broken. Final Combo: %d"), CurrentCombo);
		OnComboReset.Broadcast(CurrentCombo);
		CurrentCombo = 0;
	}
	ComboTimer = 0.0f;
	bIsSnapping = false;
	CurrentSnapTarget = nullptr;
}
