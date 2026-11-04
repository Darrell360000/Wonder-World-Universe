// Copyright Darrell R. Lewis / Wonder World Trilogy. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WonderDualStanceCombatComponent.generated.h"

UENUM(BlueprintType)
enum class ECymaticCombatStance : uint8
{
	SubBassHeavy         UMETA(DisplayName = "Sub-Bass Heavy Stance (Armor-Shattering Kinetic Slams)"),
	HighFrequencyPhase   UMETA(DisplayName = "High-Frequency Phase Stance (Aerial Slashes & Bullet-Time Dodges)")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCombatStanceSwapped, ECymaticCombatStance, NewStance);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStanceWeaveAttackExecuted, ECymaticCombatStance, StanceUsed, float, StanceBonusDamage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPhaseBulletTimeDodgeTriggered);

/**
 * UWonderDualStanceCombatComponent
 * Implements 2027 God of War: Laufey / Wo Long 2 inspired Dual-Stance Combat Weaving.
 * Darrell dynamically weaves between Sub-Bass Heavy Stance (armor break / ground shatter)
 * and High-Frequency Phase Stance (rhythmic aerial slashes & bullet-time counter-dodges).
 */
UCLASS(ClassGroup=(WonderWorld), meta=(BlueprintSpawnableComponent))
class WONDERTOWERTYCOON_API UWonderDualStanceCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWonderDualStanceCombatComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/* --- STANCE SWITCHING & WEAVING --- */
	UFUNCTION(BlueprintCallable, Category = "Wonder|Combat|Stance")
	void SwapCombatStance();

	UFUNCTION(BlueprintCallable, Category = "Wonder|Combat|Stance")
	void SetCombatStance(ECymaticCombatStance NewStance);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Combat|Stance")
	float ExecuteStanceAttack(AActor* TargetEnemy);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Combat|Stance")
	bool TryTriggerPhaseDodge();

	/* --- QUERIES --- */
	UFUNCTION(BlueprintPure, Category = "Wonder|Combat|Stance")
	ECymaticCombatStance GetActiveStance() const { return CurrentStance; }

	UFUNCTION(BlueprintPure, Category = "Wonder|Combat|Stance")
	bool IsInSubBassStance() const { return CurrentStance == ECymaticCombatStance::SubBassHeavy; }

	UFUNCTION(BlueprintPure, Category = "Wonder|Combat|Stance")
	bool IsInPhaseStance() const { return CurrentStance == ECymaticCombatStance::HighFrequencyPhase; }

	UFUNCTION(BlueprintPure, Category = "Wonder|Combat|Stance")
	float GetCurrentDamageMultiplier() const;

	/* --- TUNING PARAMETERS --- */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Combat|Tuning")
	float SubBassHeavyDamage = 350.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Combat|Tuning")
	float SubBassArmorPenetration = 0.65f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Combat|Tuning")
	float HighFrequencyPhaseDamage = 180.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Combat|Tuning")
	float PhaseAttackSpeedMultiplier = 1.75f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Combat|Tuning")
	float StanceWeaveWindowSeconds = 1.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Combat|Tuning")
	float WeaveBonusDamageMultiplier = 1.40f;

	/* --- EVENTS --- */
	UPROPERTY(BlueprintAssignable, Category = "Wonder|Combat|Events")
	FOnCombatStanceSwapped OnCombatStanceSwapped;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|Combat|Events")
	FOnStanceWeaveAttackExecuted OnStanceWeaveAttackExecuted;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|Combat|Events")
	FOnPhaseBulletTimeDodgeTriggered OnPhaseBulletTimeDodgeTriggered;

protected:
	virtual void BeginPlay() override;

private:
	ECymaticCombatStance CurrentStance = ECymaticCombatStance::SubBassHeavy;
	float TimeSinceLastStanceSwap = 10.0f;
	bool bBulletTimeActive = false;
	float BulletTimeDuration = 0.5f;
	float BulletTimeTimer = 0.0f;
};
