// Copyright Darrell R. Lewis / Wonder World Trilogy. All Rights Reserved.

#include "WonderDualStanceCombatComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

UWonderDualStanceCombatComponent::UWonderDualStanceCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UWonderDualStanceCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentStance = ECymaticCombatStance::SubBassHeavy;
	TimeSinceLastStanceSwap = 10.0f;
}

void UWonderDualStanceCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TimeSinceLastStanceSwap += DeltaTime;

	// Update Phase Stance bullet-time dilation
	if (bBulletTimeActive)
	{
		BulletTimeTimer -= DeltaTime;
		if (BulletTimeTimer <= 0.0f)
		{
			bBulletTimeActive = false;
			if (UWorld* World = GetWorld())
			{
				UGameplayStatics::SetGlobalTimeDilation(World, 1.0f);
			}
		}
	}
}

void UWonderDualStanceCombatComponent::SwapCombatStance()
{
	ECymaticCombatStance NextStance = (CurrentStance == ECymaticCombatStance::SubBassHeavy) 
		? ECymaticCombatStance::HighFrequencyPhase 
		: ECymaticCombatStance::SubBassHeavy;

	SetCombatStance(NextStance);
}

void UWonderDualStanceCombatComponent::SetCombatStance(ECymaticCombatStance NewStance)
{
	if (CurrentStance == NewStance)
	{
		return;
	}

	CurrentStance = NewStance;
	TimeSinceLastStanceSwap = 0.0f; // Reset weave window

	OnCombatStanceSwapped.Broadcast(CurrentStance);
	UE_LOG(LogTemp, Log, TEXT("[WonderCombat] Swapped to stance: %s"), 
		CurrentStance == ECymaticCombatStance::SubBassHeavy ? TEXT("Sub-Bass Heavy") : TEXT("High-Frequency Phase"));
}

float UWonderDualStanceCombatComponent::ExecuteStanceAttack(AActor* TargetEnemy)
{
	float BaseDamage = (CurrentStance == ECymaticCombatStance::SubBassHeavy) ? SubBassHeavyDamage : HighFrequencyPhaseDamage;

	// Check if this attack occurs within the Stance Weave timing window (synergy bonus)
	bool bIsWeaveCombo = (TimeSinceLastStanceSwap <= StanceWeaveWindowSeconds);
	float FinalDamage = BaseDamage;

	if (bIsWeaveCombo)
	{
		FinalDamage *= WeaveBonusDamageMultiplier;
		OnStanceWeaveAttackExecuted.Broadcast(CurrentStance, FinalDamage - BaseDamage);

		UE_LOG(LogTemp, Warning, TEXT("[WonderCombat] STANCE WEAVE COMBO! Stance: %d | Total Damage: %.1f"),
			static_cast<int32>(CurrentStance), FinalDamage);
	}

	return FinalDamage;
}

bool UWonderDualStanceCombatComponent::TryTriggerPhaseDodge()
{
	// Only High-Frequency Phase Stance grants bullet-time dodges
	if (CurrentStance != ECymaticCombatStance::HighFrequencyPhase)
	{
		return false;
	}

	bBulletTimeActive = true;
	BulletTimeTimer = BulletTimeDuration;

	if (UWorld* World = GetWorld())
	{
		// 0.25x slow motion for 0.5 real seconds
		UGameplayStatics::SetGlobalTimeDilation(World, 0.25f);
	}

	OnPhaseBulletTimeDodgeTriggered.Broadcast();
	UE_LOG(LogTemp, Log, TEXT("[WonderCombat] Phase Stance Bullet-Time Dodge activated!"));
	return true;
}

float UWonderDualStanceCombatComponent::GetCurrentDamageMultiplier() const
{
	return (CurrentStance == ECymaticCombatStance::SubBassHeavy) ? 1.5f : 1.0f;
}
