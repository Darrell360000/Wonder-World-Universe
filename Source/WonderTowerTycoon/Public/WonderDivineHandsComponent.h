// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WonderDivineHandsComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnTowerModuleSnapped, FName, ModuleType, FVector, GridCoordinate, bool, bSnappingSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEtherealHandsStateChanged, bool, bIsActive);

/**
 * WonderDivineHandsComponent
 * The Divine Hands Top-Hatter ("God's Hands Hat"):
 * - Translucent ethereal hands hover above the golden crown hat.
 * - Shapes macro-architecture and snaps modular skyscraper blocks into place on the 8m x 8m grid.
 */
UCLASS(ClassGroup=(WonderWorld), meta=(BlueprintSpawnableComponent))
class WONDERTOWERTYCOON_API UWonderDivineHandsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWonderDivineHandsComponent();

	virtual void BeginPlay() override;

	/** Toggles ethereal hands projection for macro architectural editing */
	UFUNCTION(BlueprintCallable, Category = "WonderWorld|MacroArchitecture")
	void SetEtherealHandsActive(bool bActive);

	/** Snaps a target modular block into the 3D municipal grid */
	UFUNCTION(BlueprintCallable, Category = "WonderWorld|MacroArchitecture")
	bool SnapTowerModuleToGrid(FName ModuleType, FVector WorldPosition, FVector& OutSnappedPosition);

	UPROPERTY(BlueprintAssignable, Category = "WonderWorld|MacroArchitecture|Events")
	FOnTowerModuleSnapped OnTowerModuleSnapped;

	UPROPERTY(BlueprintAssignable, Category = "WonderWorld|MacroArchitecture|Events")
	FOnEtherealHandsStateChanged OnEtherealHandsStateChanged;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WonderWorld|MacroArchitecture|Grid")
	float GridCellSize = 800.0f; // 8m standard block

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WonderWorld|MacroArchitecture|Grid")
	float FloorHeight = 400.0f;  // 4m standard floor height

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WonderWorld|MacroArchitecture|State")
	bool bEtherealHandsActive = true;
};
