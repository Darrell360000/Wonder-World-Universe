// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MamaWonderCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEvonHubPermissionIssued, FString, SectorId, bool, bGranted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMunicipalDividendDisbursed, int64, DividendAmountUsd);

/**
 * MamaWonderCharacter
 * Empress of the Purple City Universe & Matriarch of the Twin Towers:
 * - Wears the royal purple tailored tuxedo, gold chain, and "SOUND MAGIC" LED equalizer crown top hat.
 * - Wields the Conductor's Baton to orchestrate municipal zoning, twin tower dividends, and the 3-Column Ledger.
 * - Grants EVON HUB system clearance ("PERMISSION GRANTED") for Wonder Wardens and district expansion.
 */
UCLASS()
class WONDERTOWERTYCOON_API AMamaWonderCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMamaWonderCharacter();

	virtual void BeginPlay() override;

	/** Grants Evon Hub system permission for heavy enforcement & tower expansion */
	UFUNCTION(BlueprintCallable, Category = "WonderWorld|MamaWonder")
	void GrantEvonHubPermission(const FString& SectorId);

	/** Disburses municipal dividends directly to citizen accounts */
	UFUNCTION(BlueprintCallable, Category = "WonderWorld|MamaWonder")
	void DisburseMunicipalDividends(int64 AmountUsd);

	UPROPERTY(BlueprintAssignable, Category = "WonderWorld|MamaWonder|Events")
	FOnEvonHubPermissionIssued OnEvonHubPermissionIssued;

	UPROPERTY(BlueprintAssignable, Category = "WonderWorld|MamaWonder|Events")
	FOnMunicipalDividendDisbursed OnMunicipalDividendDisbursed;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WonderWorld|MamaWonder|Components")
	UStaticMeshComponent* MamaEqualizerCrownMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WonderWorld|MamaWonder|Components")
	UStaticMeshComponent* ConductorBatonMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WonderWorld|MamaWonder|Config")
	bool bPermissionGranted = true;
};
