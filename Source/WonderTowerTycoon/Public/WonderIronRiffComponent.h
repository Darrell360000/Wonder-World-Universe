// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WonderIronRiffComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnSubbassGroundSlam, FVector, SlamLocation, float, Radius, float, KnockbackImpulse);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAnvilHydraulicPressureChanged, float, NewPressure);

/**
 * WonderIronRiffComponent
 * Jaxson "Iron Riff" Thorne (The Heavy Industrial Brawler):
 * - Iron-plated "Anvil Crown" top hat with hydraulic shock absorbers & hazard lights.
 * - Pneumatic sonic gauntlets channel heavy sub-bass energy into devastating seismic ground slams.
 */
UCLASS(ClassGroup=(WonderWorld), meta=(BlueprintSpawnableComponent))
class WONDERTOWERTYCOON_API UWonderIronRiffComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWonderIronRiffComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Builds hydraulic compression pressure in the Anvil Crown and gauntlets */
	UFUNCTION(BlueprintCallable, Category = "WonderWorld|IndustrialBrawler")
	void ChargeHydraulicPiston(float DeltaPressure);

	/** Executes the Sub-bass seismic ground slam */
	UFUNCTION(BlueprintCallable, Category = "WonderWorld|IndustrialBrawler")
	bool ExecuteSubbassGroundSlam(FVector ImpactPoint);

	UFUNCTION(BlueprintPure, Category = "WonderWorld|IndustrialBrawler")
	float GetHydraulicPressure() const { return CurrentPressure; }

	UFUNCTION(BlueprintPure, Category = "WonderWorld|IndustrialBrawler")
	bool IsMaxPressure() const { return CurrentPressure >= MaxPressure; }

	UPROPERTY(BlueprintAssignable, Category = "WonderWorld|IndustrialBrawler|Events")
	FOnSubbassGroundSlam OnSubbassGroundSlam;

	UPROPERTY(BlueprintAssignable, Category = "WonderWorld|IndustrialBrawler|Events")
	FOnAnvilHydraulicPressureChanged OnAnvilHydraulicPressureChanged;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WonderWorld|IndustrialBrawler|Stats")
	float CurrentPressure = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WonderWorld|IndustrialBrawler|Stats")
	float MaxPressure = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WonderWorld|IndustrialBrawler|Stats")
	float BaseSlamRadius = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WonderWorld|IndustrialBrawler|Stats")
	float BaseKnockbackImpulse = 2500.0f;
};
