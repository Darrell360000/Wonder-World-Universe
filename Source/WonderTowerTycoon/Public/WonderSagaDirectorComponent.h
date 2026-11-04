// Copyright Darrell R. Lewis / Wonder World Trilogy. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WonderSagaDirectorComponent.generated.h"

UENUM(BlueprintType)
enum class EWonderAct : uint8
{
	ActI_SovereignStreets,
	ActII_ShadowGrid,
	ActIII_ApexCitadel
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActChanged, EWonderAct, NewAct);

UENUM(BlueprintType)
enum class ESagaChapterMode : uint8
{
	Part1_SovereignStreets UMETA(DisplayName = "Part 1: Sovereign Streets (Commercial Release 1)"),
	Part2_ShadowGrid       UMETA(DisplayName = "Part 2: The Shadow Grid (Commercial Release 2)"),
	Part3_ApexCitadel      UMETA(DisplayName = "Part 3: The Apex Citadel (Commercial Release 3)"),
	Part4_LunarFrontier    UMETA(DisplayName = "Part 4: Tranquility Shipyards & Jovian Oceans"),
	Full_Trilogy_Omnibus   UMETA(DisplayName = "Full Trilogy Omnibus (Complete Master Edition)")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSagaModeChanged, ESagaChapterMode, NewMode);

UCLASS(ClassGroup=(WonderWorld), meta=(BlueprintSpawnableComponent))
class WONDERTOWERTYCOON_API UWonderSagaDirectorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWonderSagaDirectorComponent();

protected:
	virtual void BeginPlay() override;

public:
	/* --- ACT PROGRESSION CONTROLLER --- */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wonder|Saga")
	EWonderAct CurrentAct = EWonderAct::ActI_SovereignStreets;

	UFUNCTION(BlueprintCallable, Category = "Wonder|Saga")
	void AdvanceAct();

	UPROPERTY(BlueprintAssignable, Category = "Wonder|Saga|Events")
	FOnActChanged OnActChanged; // mechanics listen and unlock themselves

	/* --- SAGA COMMERCIAL CONFIGURATION --- */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Saga")
	ESagaChapterMode ActiveSagaMode = ESagaChapterMode::Full_Trilogy_Omnibus;

	UFUNCTION(BlueprintCallable, Category = "Wonder|Saga")
	void ApplySagaRestrictions();

	UFUNCTION(BlueprintCallable, Category = "Wonder|Saga")
	void SetSagaMode(ESagaChapterMode NewMode);

	/* --- CHAPTER PROGRESSION QUERY HELPERS --- */
	UFUNCTION(BlueprintPure, Category = "Wonder|Saga - Queries")
	bool IsSubwayGridAccessible() const;

	UFUNCTION(BlueprintPure, Category = "Wonder|Saga - Queries")
	bool IsCitadelApexAccessible() const;

	UFUNCTION(BlueprintPure, Category = "Wonder|Saga - Queries")
	bool IsStratosphericFlightAllowed() const;

	UFUNCTION(BlueprintPure, Category = "Wonder|Saga - Queries")
	bool IsLunarShipyardsAccessible() const;

	UFUNCTION(BlueprintPure, Category = "Wonder|Saga - Queries")
	bool IsWarpTravelAccessible() const;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|Saga|Events")
	FOnSagaModeChanged OnSagaModeChanged;
};
