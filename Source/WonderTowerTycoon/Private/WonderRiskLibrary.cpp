#include "WonderRiskLibrary.h"

FWonderRiskResult UWonderRiskLibrary::RollRisk(int32 RiskTier, int32 Seed, float LuckModifier)
{
	FWonderRiskResult Result;

	FRandomStream Stream(Seed == 0 ? FMath::Rand() : Seed);
	const int32 BaseRoll = Stream.RandRange(1, 100);
	const float EffectiveRoll = BaseRoll + (LuckModifier * 10.0f);

	Result.RolledValue = FMath::Clamp(FMath::RoundToInt(EffectiveRoll), 1, 100);

	const int32 SuccessThreshold = FMath::Clamp(40 + (RiskTier * 15), 40, 95);

	if (Result.RolledValue >= SuccessThreshold)
	{
		Result.bSuccess = true;
		Result.Multiplier = 1.0f + (RiskTier * 0.5f);
		Result.ScrapModifier = 50 * RiskTier;
		Result.NarrativeOutcome = FString::Printf(TEXT("Critical Breakthrough! Rolled %d vs target %d."), Result.RolledValue, SuccessThreshold);
	}
	else
	{
		Result.bSuccess = false;
		Result.Multiplier = 0.5f;
		Result.ScrapModifier = -20 * RiskTier;
		Result.NarrativeOutcome = FString::Printf(TEXT("Gambit Complication! Rolled %d vs target %d."), Result.RolledValue, SuccessThreshold);
	}

	return Result;
}

float UWonderRiskLibrary::CalculateBalatroMultiplier(int32 StreakLevel)
{
	switch (StreakLevel)
	{
	case 0:  return 1.0f;
	case 1:  return 2.0f;
	case 2:  return 4.0f;
	case 3:  return 8.0f;
	default: return 16.0f;
	}
}

bool UWonderRiskLibrary::ResolveAlibiVote(int32 PositiveVotes, int32 NegativeVotes, float Threshold)
{
	const int32 TotalVotes = PositiveVotes + NegativeVotes;
	if (TotalVotes <= 0)
	{
		return true; // Benefit of the doubt
	}

	const float Ratio = static_cast<float>(PositiveVotes) / static_cast<float>(TotalVotes);
	return Ratio >= Threshold;
}

float UWonderRiskLibrary::ApplyWearAndRepairFloor(float CurrentCondition, float Degradation)
{
	// Strict 70% floor: gear never breaks below 0.70f (70% efficiency)
	return FMath::Max(0.70f, CurrentCondition - Degradation);
}
