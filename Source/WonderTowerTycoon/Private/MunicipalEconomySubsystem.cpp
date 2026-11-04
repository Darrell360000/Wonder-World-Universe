#include "MunicipalEconomySubsystem.h"
#include "Engine/World.h"
#include <cmath>
#include <limits>

UMunicipalEconomySubsystem::UMunicipalEconomySubsystem()
{
}


void UMunicipalEconomySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	RecalculateAndEmit();
}

void UMunicipalEconomySubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!FMath::IsFinite(DeltaTime) || DeltaTime <= 0.0f)
	{
		return;
	}

	const float PreviousAccumulator = Accumulator;
	const double TotalElapsed = FMath::Min(static_cast<double>(Accumulator) + DeltaTime,
		static_cast<double>(MAX_int32));
	const int32 WholeSeconds = static_cast<int32>(std::floor(FMath::Min(TotalElapsed, static_cast<double>(MAX_int32))));
	Accumulator = static_cast<float>(TotalElapsed - WholeSeconds);

	// Only payout intervals that end before the overcharge expires receive the bonus.
	const double FirstPayoutIn = 1.0 - PreviousAccumulator;
	const double BoostedIntervalCount = OverchargeRemaining > FirstPayoutIn
		? FMath::Min(static_cast<double>(WholeSeconds), std::ceil(OverchargeRemaining - FirstPayoutIn))
		: 0.0;
	const double MultipliedSeconds = BoostedIntervalCount * OverchargeMultiplier
		+ (WholeSeconds - BoostedIntervalCount);

	for (TPair<FString, FIncomeSource>& Pair : IncomeSources)
	{
		FIncomeSource& Source = Pair.Value;
		const double AccruedCredits = static_cast<double>(Source.CreditsPerTick) * MultipliedSeconds
			+ Source.FractionalCredits;
		double WholeCredits = 0.0;
		Source.FractionalCredits = std::modf(AccruedCredits, &WholeCredits);
		if (WholeCredits >= static_cast<double>(std::numeric_limits<int64>::max()))
		{
			Credits = MAX_int64;
			Source.FractionalCredits = 0.0;
		}
		else if (WholeCredits <= static_cast<double>(std::numeric_limits<int64>::min()))
		{
			Credits = MIN_int64;
			Source.FractionalCredits = 0.0;
		}
		else
		{
			const int64 CreditDelta = static_cast<int64>(WholeCredits);
			if (CreditDelta > 0 && Credits > MAX_int64 - CreditDelta)
			{
				Credits = MAX_int64;
			}
			else if (CreditDelta < 0 && Credits < MIN_int64 - CreditDelta)
			{
				Credits = MIN_int64;
			}
			else
			{
				Credits += CreditDelta;
			}
		}
	}

	const bool bOverchargeExpired = OverchargeRemaining > 0.0f && DeltaTime >= OverchargeRemaining;
	OverchargeRemaining = FMath::Max(0.0f, OverchargeRemaining - DeltaTime);
	if (OverchargeRemaining == 0.0f)
	{
		OverchargeMultiplier = 1.0f;
	}
	if (WholeSeconds > 0 || bOverchargeExpired)
	{
		RecalculateAndEmit();
	}
}

TStatId UMunicipalEconomySubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UMunicipalEconomySubsystem, STATGROUP_Tickables);
}

bool UMunicipalEconomySubsystem::TrySpendCredits(int64 Amount, FString& FailureReason)
{
	if (Amount < 0)
	{
		FailureReason = TEXT("A purchase cannot have a negative price.");
		OnPurchaseRejected.Broadcast(FailureReason);
		return false;
	}
	if (Amount > Credits)
	{
		FailureReason = FString::Printf(TEXT("Not enough credits (need %lld, have %lld)."), Amount, Credits);
		OnPurchaseRejected.Broadcast(FailureReason);
		return false;
	}
	Credits -= Amount;
	RecalculateAndEmit();
	return true;
}

void UMunicipalEconomySubsystem::AddIncomeSource(FString SourceId, float CreditsPerTick,
	float PowerDemand, float WaterDemand, float Population)
{
	if (SourceId.IsEmpty())
	{
		return;
	}

	FIncomeSource Source;
	// Signed credit rates allow a source to represent either income or upkeep.
	Source.CreditsPerTick = FMath::IsFinite(CreditsPerTick) ? CreditsPerTick : 0.0f;
	Source.PowerDemand = FMath::IsFinite(PowerDemand) ? FMath::Max(0.0f, PowerDemand) : 0.0f;
	Source.WaterDemand = FMath::IsFinite(WaterDemand) ? FMath::Max(0.0f, WaterDemand) : 0.0f;
	Source.Population = FMath::IsFinite(Population) ? FMath::Max(0.0f, Population) : 0.0f;
	IncomeSources.Add(MoveTemp(SourceId), Source);
	RecalculateAndEmit();
}

bool UMunicipalEconomySubsystem::RemoveIncomeSource(FString SourceId)
{
	const bool Removed = IncomeSources.Remove(SourceId) > 0;
	if (Removed) RecalculateAndEmit();
	return Removed;
}

void UMunicipalEconomySubsystem::AddUtilitySupply(float PowerSupplyPerTick, float WaterSupplyPerTick)
{
	const double SafePower = FMath::IsFinite(PowerSupplyPerTick) ? FMath::Max(0.0f, PowerSupplyPerTick) : 0.0;
	const double SafeWater = FMath::IsFinite(WaterSupplyPerTick) ? FMath::Max(0.0f, WaterSupplyPerTick) : 0.0;
	NetPowerSupply = static_cast<float>(FMath::Min(static_cast<double>(NetPowerSupply) + SafePower,
		static_cast<double>(MAX_flt)));
	FreshWaterSupply = static_cast<float>(FMath::Min(static_cast<double>(FreshWaterSupply) + SafeWater,
		static_cast<double>(MAX_flt)));
	RecalculateAndEmit();
}

void UMunicipalEconomySubsystem::ActivateNetPowerOvercharge(float DurationSeconds, float Multiplier)
{
	const float SafeDuration = FMath::IsFinite(DurationSeconds) ? FMath::Max(0.0f, DurationSeconds) : 0.0f;
	const float SafeMultiplier = FMath::IsFinite(Multiplier) ? FMath::Max(1.0f, Multiplier) : 1.0f;
	OverchargeRemaining = FMath::Max(OverchargeRemaining, SafeDuration);
	OverchargeMultiplier = OverchargeRemaining > 0.0f ? SafeMultiplier : 1.0f;
	RecalculateAndEmit();
}

FMunicipalEconomySnapshot UMunicipalEconomySubsystem::GetSnapshot() const
{
	return LastSnapshot;
}

void UMunicipalEconomySubsystem::RecalculateAndEmit()
{
	FMunicipalEconomySnapshot Snapshot;
	Snapshot.Credits = Credits;
	for (const TPair<FString, FIncomeSource>& Pair : IncomeSources)
	{
		Snapshot.NetPowerDemand += Pair.Value.PowerDemand;
		Snapshot.FreshWater += Pair.Value.WaterDemand;
		Snapshot.Population += Pair.Value.Population;
		if (Pair.Value.CreditsPerTick < 0.0f)
		{
			Snapshot.UpkeepCost += -Pair.Value.CreditsPerTick;
			Snapshot.NetCashPerTick += Pair.Value.CreditsPerTick;
		}
		else
		{
			Snapshot.NetCashPerTick += Pair.Value.CreditsPerTick * OverchargeMultiplier;
		}
	}
	Snapshot.NetPowerSupply = NetPowerSupply;
	Snapshot.FreshWater = FreshWaterSupply - Snapshot.FreshWater;
	Snapshot.OverchargeMultiplier = OverchargeMultiplier;
	Snapshot.OverchargeSecondsRemaining = OverchargeRemaining;
	LastSnapshot = Snapshot;
	OnEconomyChanged.Broadcast(LastSnapshot);
}
