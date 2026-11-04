// Copyright Darrell R. Lewis / Wonder World Trilogy. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "WonderInclemencyWeatherSubsystem.generated.h"

UENUM(BlueprintType)
enum class EWeatherCycleState : uint8
{
	Plenty_ClearNeonNight   UMETA(DisplayName = "Plenty (Clear Neon Night)"),
	Transitioning           UMETA(DisplayName = "Transitioning"),
	Inclemency_Resonance    UMETA(DisplayName = "Inclemency (Resonance Tempest)")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeatherPhaseChanged, EWeatherCycleState, NewPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnResonanceLightningStrike, FVector, StrikeLocation, float, StrikeDamage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFeralRogueMechSpawned, FVector, SpawnLocation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPuddleReflectionsModulated, float, ReflectionIntensity, float, SurfaceWetness);

/**
 * UWonderInclemencyWeatherSubsystem
 * Implements Monster Hunter Wilds (2025) inspired dynamic Acoustic Inclemency Weather Cycles.
 * Dynamically shifts between 'Plenty' (Clear Neon) and 'Inclemency' (Resonance Tempest)
 * seamlessly without loading screens, modulating vehicle aerodynamics, lightning strike hazards,
 * and road puddle reflections dynamically.
 */
UCLASS()
class WONDERTOWERTYCOON_API UWonderInclemencyWeatherSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UWonderInclemencyWeatherSubsystem, STATGROUP_Tickables); }

	/* --- WEATHER CONTROLS --- */
	UFUNCTION(BlueprintCallable, Category = "Wonder|Weather")
	void SetWeatherPhase(EWeatherCycleState TargetPhase, float TransitionDuration = 10.0f);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Weather")
	void TriggerLightningStrike(FVector TargetLocation);

	/* --- GAMEPLAY & ENVIRONMENTAL QUERIES --- */
	UFUNCTION(BlueprintPure, Category = "Wonder|Weather")
	EWeatherCycleState GetCurrentWeatherState() const { return CurrentState; }

	UFUNCTION(BlueprintPure, Category = "Wonder|Weather")
	float GetCommerceMultiplier() const;

	UFUNCTION(BlueprintPure, Category = "Wonder|Weather")
	float GetVehicleSpeedMultiplier() const;

	UFUNCTION(BlueprintPure, Category = "Wonder|Weather")
	float GetAerodynamicDragMultiplier() const { return AerodynamicDragMultiplier; }

	UFUNCTION(BlueprintPure, Category = "Wonder|Weather")
	FVector GetWindTurbulenceVector() const { return CurrentWindTurbulence; }

	UFUNCTION(BlueprintPure, Category = "Wonder|Weather")
	float GetStormIntensity() const { return StormIntensity; }

	UFUNCTION(BlueprintPure, Category = "Wonder|Weather")
	float GetRoadPuddleReflectionIntensity() const { return RoadPuddleReflectionIntensity; }

	UFUNCTION(BlueprintPure, Category = "Wonder|Weather")
	float GetRoadSurfaceWetness() const { return RoadSurfaceWetness; }

	/* --- TUNING PARAMETERS --- */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Weather|Tuning")
	float PlentyCommerceBoost = 1.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Weather|Tuning")
	float PlentySpeedBoost = 1.20f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Weather|Tuning")
	float MaxWindTurbulenceForce = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Weather|Tuning")
	float LightningStrikeDamage = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Weather|Tuning")
	float TargetBPM = 85.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Weather|Tuning")
	float MaxPuddleReflectionIntensity = 1.0f;

	/* --- EVENTS --- */
	UPROPERTY(BlueprintAssignable, Category = "Wonder|Weather|Events")
	FOnWeatherPhaseChanged OnWeatherPhaseChanged;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|Weather|Events")
	FOnResonanceLightningStrike OnResonanceLightningStrike;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|Weather|Events")
	FOnFeralRogueMechSpawned OnFeralRogueMechSpawned;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|Weather|Events")
	FOnPuddleReflectionsModulated OnPuddleReflectionsModulated;

private:
	void UpdateWeatherTransition(float DeltaTime);
	void UpdateResonanceTempest(float DeltaTime);
	void ModulateEnvironmentalEffects(float DeltaTime);

	EWeatherCycleState CurrentState = EWeatherCycleState::Plenty_ClearNeonNight;
	EWeatherCycleState DestinationState = EWeatherCycleState::Plenty_ClearNeonNight;

	float StormIntensity = 0.0f; // 0.0 = Pure Plenty, 1.0 = Max Inclemency
	float TargetStormIntensity = 0.0f;
	float TransitionRate = 0.1f;

	// Dynamic Aerodynamics & Visuals
	FVector CurrentWindTurbulence = FVector::ZeroVector;
	float AerodynamicDragMultiplier = 1.0f;
	float RoadPuddleReflectionIntensity = 0.0f;
	float RoadSurfaceWetness = 0.0f;

	float BeatTimer = 0.0f;
	float LightningCooldown = 4.0f;
};
