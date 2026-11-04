#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MunicipalEconomySubsystem.generated.h"

UENUM(BlueprintType)
enum class EMunicipalResource : uint8
{
	Credits,
	NetPower,
	FreshWater,
	Population
};

USTRUCT(BlueprintType)
struct WONDERTOWERTYCOON_API FMunicipalEconomySnapshot
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly) int64 Credits = 25000;
	UPROPERTY(BlueprintReadOnly) float NetPowerDemand = 0.0f;
	UPROPERTY(BlueprintReadOnly) float NetPowerSupply = 0.0f;
	UPROPERTY(BlueprintReadOnly) float FreshWater = 0.0f;
	UPROPERTY(BlueprintReadOnly) float Population = 0.0f;
	UPROPERTY(BlueprintReadOnly) float UpkeepCost = 0.0f;
	UPROPERTY(BlueprintReadOnly) float NetCashPerTick = 0.0f;
	UPROPERTY(BlueprintReadOnly) float OverchargeMultiplier = 1.0f;
	UPROPERTY(BlueprintReadOnly) float OverchargeSecondsRemaining = 0.0f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMunicipalEconomyChanged, const FMunicipalEconomySnapshot&, Snapshot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMunicipalPurchaseRejected, FString, Reason);

UCLASS()
class WONDERTOWERTYCOON_API UMunicipalEconomySubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
public:
	UMunicipalEconomySubsystem();
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual ETickableTickType GetTickableTickType() const override { return ETickableTickType::Always; }

	UFUNCTION(BlueprintCallable, Category="Wonder World|Municipal Economy")
	bool TrySpendCredits(int64 Amount, FString& FailureReason);
	UFUNCTION(BlueprintCallable, Category="Wonder World|Municipal Economy")
	void AddIncomeSource(FString SourceId, float CreditsPerTick, float PowerDemand, float WaterDemand = 0.0f, float Population = 0.0f);
	UFUNCTION(BlueprintCallable, Category="Wonder World|Municipal Economy")
	bool RemoveIncomeSource(FString SourceId);
	UFUNCTION(BlueprintCallable, Category="Wonder World|Municipal Economy")
	void AddUtilitySupply(float PowerSupplyPerTick, float WaterSupplyPerTick);
	UFUNCTION(BlueprintCallable, Category="Wonder World|Municipal Economy")
	void ActivateNetPowerOvercharge(float DurationSeconds, float Multiplier = 2.0f);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Wonder World|Municipal Economy")
	FMunicipalEconomySnapshot GetSnapshot() const;

	UPROPERTY(BlueprintAssignable, Category="Wonder World|Municipal Economy")
	FMunicipalEconomyChanged OnEconomyChanged;
	UPROPERTY(BlueprintAssignable, Category="Wonder World|Municipal Economy")
	FMunicipalPurchaseRejected OnPurchaseRejected;

private:
	struct FIncomeSource
	{
		float CreditsPerTick = 0.0f;
		float PowerDemand = 0.0f;
		float WaterDemand = 0.0f;
		float Population = 0.0f;
		double FractionalCredits = 0.0;
	};
	TMap<FString, FIncomeSource> IncomeSources;
	float NetPowerSupply = 0.0f;
	float FreshWaterSupply = 0.0f;
	float Accumulator = 0.0f;
	float OverchargeRemaining = 0.0f;
	float OverchargeMultiplier = 1.0f;
	int64 Credits = 25000;
	FMunicipalEconomySnapshot LastSnapshot;
	void RecalculateAndEmit();
};
