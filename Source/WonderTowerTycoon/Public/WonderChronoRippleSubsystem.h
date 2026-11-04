// Copyright Darrell R. Lewis / Wonder World Trilogy. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "WonderChronoRippleSubsystem.generated.h"

UENUM(BlueprintType)
enum class EDromeMemoryEra : uint8
{
	FoundingDecade_1920        UMETA(DisplayName = "1920: Genesis of Mama Tower"),
	IndustrialTransitBoom_1975 UMETA(DisplayName = "1975: Subterranean Transit Expansion"),
	SyndicateCorporateCoup_2008 UMETA(DisplayName = "2008: The Shadow Grid Corporate Coup")
};

USTRUCT(BlueprintType)
struct FChronoMemoryDecision
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Chrono")
	FString DecisionKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Chrono")
	FString MemoryTitle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Chrono")
	EDromeMemoryEra Era = EDromeMemoryEra::FoundingDecade_1920;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Chrono")
	int32 ChosenPath = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Chrono")
	double TimeStamp = 0.0;
};

USTRUCT(BlueprintType)
struct FDistrictTemporalLayout
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Chrono")
	FString DistrictId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Chrono")
	FString DistrictName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Chrono")
	FString ActiveArchitecturalStyle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Chrono")
	FString AssignedCEO;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Chrono")
	bool bSubwayVaultRouteOpen = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Chrono")
	float EconomicSynergyBonus = 1.0f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnChronoRipplePropagated, FString, DecisionKey, FString, DistrictId, FString, NewStyle, FString, NewCEO);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnArchitecturalStyleMorphed, FString, DistrictId, FString, OldStyle, FString, NewStyle);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCEOLeadershipOverturned, FString, DistrictId, FString, OldCEO, FString, NewCEO);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTemporalSubwayRouteUnlocked, FString, DistrictId);

/**
 * UWonderChronoRippleSubsystem
 * Implements Clockwork Revolution (2027) inspired Historical Chrono-Ripples via Drome Cinema portals.
 * Decisions made inside historical memories dynamically rewrite the present city—
 * morphing skyscraper architectural styles, altering district CEO corporate ownership,
 * and unlocking forgotten subterranean transit routes.
 */
UCLASS()
class WONDERTOWERTYCOON_API UWonderChronoRippleSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/* --- DROME CINEMA DECISION RECORDING --- */
	UFUNCTION(BlueprintCallable, Category = "Wonder|Chrono")
	void RecordHistoricalDecision(FString DecisionKey, FString MemoryTitle, EDromeMemoryEra Era, int32 ChoiceIndex);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Chrono")
	void TriggerChronoRipple(FString DecisionKey, FString AffectedDistrictId, FString NewArchitecturalStyle, FString NewCEO, bool bUnlockSubwayRoute);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Chrono")
	void ApplyArchitecturalShift(FString DistrictId, FString NewStyle);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Chrono")
	void TransferDistrictCEO(FString DistrictId, FString NewCEO);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Chrono")
	void UnlockSubwayRoute(FString DistrictId);

	/* --- QUERIES --- */
	UFUNCTION(BlueprintPure, Category = "Wonder|Chrono")
	bool GetDistrictTemporalLayout(FString DistrictId, FDistrictTemporalLayout& OutLayout) const;

	UFUNCTION(BlueprintPure, Category = "Wonder|Chrono")
	TArray<FDistrictTemporalLayout> GetAllDistrictStates() const;

	UFUNCTION(BlueprintPure, Category = "Wonder|Chrono")
	TArray<FChronoMemoryDecision> GetDecisionHistory() const { return DecisionHistory; }

	/* --- EVENTS --- */
	UPROPERTY(BlueprintAssignable, Category = "Wonder|Chrono|Events")
	FOnChronoRipplePropagated OnChronoRipplePropagated;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|Chrono|Events")
	FOnArchitecturalStyleMorphed OnArchitecturalStyleMorphed;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|Chrono|Events")
	FOnCEOLeadershipOverturned OnCEOLeadershipOverturned;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|Chrono|Events")
	FOnTemporalSubwayRouteUnlocked OnTemporalSubwayRouteUnlocked;

private:
	void InitializeDefaultDistricts();

	UPROPERTY()
	TArray<FChronoMemoryDecision> DecisionHistory;

	UPROPERTY()
	TMap<FString, FDistrictTemporalLayout> DistrictLayouts;
};
