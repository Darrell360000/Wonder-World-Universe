// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WonderRhythmHeatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRhythmHeatChanged, float, NewHeat, float, MaxHeat);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCymaticShockwaveTriggered, float, ShockwaveRadius);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDistrictRoughnessPurified, FString, DistrictName, float, PurifiedAmount);

/**
 * WonderRhythmHeatComponent
 * Darrell X-Gen's signature Sound Magic conductor system:
 * - Builds up Rhythm Heat via timed melee/wand strikes and music-synced actions.
 * - Unleashes Cymatic Shockwaves that purify Copycat faction corruption and reduce district roughness.
 */
UCLASS(ClassGroup=(WonderWorld), meta=(BlueprintSpawnableComponent))
class WONDERTOWERTYCOON_API UWonderRhythmHeatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWonderRhythmHeatComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Adds rhythm heat when landing beat-synced conductor wand attacks */
	UFUNCTION(BlueprintCallable, Category = "WonderWorld|RhythmMagic")
	void AddRhythmHeat(float Amount);

	/** Consumes accumulated rhythm heat to unleash a radial cymatic musical shockwave */
	UFUNCTION(BlueprintCallable, Category = "WonderWorld|RhythmMagic")
	bool UnleashCymaticShockwave();

	/** Purifies Copycat faction presence in the targeted district */
	UFUNCTION(BlueprintCallable, Category = "WonderWorld|RhythmMagic")
	void PurifyDistrict(const FString& DistrictName, float Radius);

	UFUNCTION(BlueprintPure, Category = "WonderWorld|RhythmMagic")
	float GetHeatPercent() const { return (MaxRhythmHeat > 0.0f) ? (CurrentRhythmHeat / MaxRhythmHeat) : 0.0f; }

	UFUNCTION(BlueprintPure, Category = "WonderWorld|RhythmMagic")
	bool IsOverdriveReady() const { return CurrentRhythmHeat >= MaxRhythmHeat; }

	UPROPERTY(BlueprintAssignable, Category = "WonderWorld|RhythmMagic|Events")
	FOnRhythmHeatChanged OnRhythmHeatChanged;

	UPROPERTY(BlueprintAssignable, Category = "WonderWorld|RhythmMagic|Events")
	FOnCymaticShockwaveTriggered OnCymaticShockwaveTriggered;

	UPROPERTY(BlueprintAssignable, Category = "WonderWorld|RhythmMagic|Events")
	FOnDistrictRoughnessPurified OnDistrictRoughnessPurified;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WonderWorld|RhythmMagic|Stats")
	float CurrentRhythmHeat = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WonderWorld|RhythmMagic|Stats")
	float MaxRhythmHeat = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WonderWorld|RhythmMagic|Stats")
	float HeatDecayRate = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WonderWorld|RhythmMagic|Stats")
	float BaseShockwaveRadius = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WonderWorld|RhythmMagic|Stats")
	float OverdriveMultiplier = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WonderWorld|RhythmMagic|VFX")
	FName WandSocketName = "Muzzle_Wand";
};
