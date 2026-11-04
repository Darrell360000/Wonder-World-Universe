#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WonderFreeflowCombatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnFreeflowStrikeLanded, AActor*, Target, int32, NewCombo, float, DamageDealt);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBeatCounterExecuted, AActor*, Target, bool, bPerfectBeat);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnComboReset, int32, FinalCombo);

UCLASS(ClassGroup=(WonderWorld), meta=(BlueprintSpawnableComponent))
class WONDERTOWERTYCOON_API UWonderFreeflowCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWonderFreeflowCombatComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/* --- FREEFLOW COMBAT ACTIONS --- */
	UFUNCTION(BlueprintCallable, Category = "Wonder|Combat - Freeflow")
	bool ExecuteDirectionalFreeflowStrike(FVector InputDirection);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Combat - Freeflow")
	bool ExecuteBeatTimedParryCounter(float BeatOffsetTolerance = 0.18f);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Combat - Freeflow")
	void ResetCombo();

	/* --- TUNING PARAMETERS --- */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Combat - Tuning")
	float MaxSnapDistance = 950.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Combat - Tuning")
	float SnapInterpolationSpeed = 24.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Combat - Tuning")
	float BaseStrikeDamage = 35.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Combat - Tuning")
	float MaxComboWindowSeconds = 2.8f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wonder|Combat - State")
	int32 CurrentCombo = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wonder|Combat - State")
	float ComboTimer = 0.0f;

	/* --- TARGET SCANNING --- */
	UFUNCTION(BlueprintCallable, Category = "Wonder|Combat - Queries")
	AActor* FindBestFreeflowTarget(FVector InputDirection) const;

	/* --- EVENTS --- */
	UPROPERTY(BlueprintAssignable, Category = "Wonder|Combat|Events")
	FOnFreeflowStrikeLanded OnFreeflowStrikeLanded;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|Combat|Events")
	FOnBeatCounterExecuted OnBeatCounterExecuted;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|Combat|Events")
	FOnComboReset OnComboReset;

private:
	UPROPERTY()
	AActor* CurrentSnapTarget;

	bool bIsSnapping = false;
	FVector SnapDestination;
};
