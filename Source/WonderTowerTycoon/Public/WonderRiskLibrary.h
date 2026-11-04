#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WonderRiskLibrary.generated.h"

USTRUCT(BlueprintType)
struct FWonderRiskResult
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wonder|Risk")
	bool bSuccess = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wonder|Risk")
	int32 RolledValue = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wonder|Risk")
	float Multiplier = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wonder|Risk")
	FString NarrativeOutcome = TEXT("");

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wonder|Risk")
	int32 ScrapModifier = 0;
};

/**
 * Blueprint function library for Wonder Dice risk evaluation, Balatro streak multipliers,
 * 70% wear floors, and Alibi Court vote resolutions.
 */
UCLASS()
class WONDERTOWERTYCOON_API UWonderRiskLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Roll risk dice for social or tactical gambits with seed reproducibility.
	 */
	UFUNCTION(BlueprintCallable, Category = "Wonder|Risk")
	static FWonderRiskResult RollRisk(int32 RiskTier, int32 Seed, float LuckModifier = 0.0f);

	/**
	 * Balatro-style multiplier ladder: 1x, 2x, 4x, 8x, 16x.
	 */
	UFUNCTION(BlueprintPure, Category = "Wonder|Risk")
	static float CalculateBalatroMultiplier(int32 StreakLevel);

	/**
	 * Resolves peer or AI court alibi votes based on majority threshold.
	 */
	UFUNCTION(BlueprintPure, Category = "Wonder|Risk")
	static bool ResolveAlibiVote(int32 PositiveVotes, int32 NegativeVotes, float Threshold = 0.5f);

	/**
	 * Pillar 3 Wear & Repair Rule: equipment degrades to a strict 70% minimum floor, never breaking or deleting.
	 */
	UFUNCTION(BlueprintPure, Category = "Wonder|Gear")
	static float ApplyWearAndRepairFloor(float CurrentCondition, float Degradation);
};
