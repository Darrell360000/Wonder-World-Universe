// Copyright Darrell R. Lewis / Wonder World Trilogy. All Rights Reserved.

#include "WonderInclemencyWeatherSubsystem.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

void UWonderInclemencyWeatherSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	CurrentState = EWeatherCycleState::Plenty_ClearNeonNight;
	DestinationState = EWeatherCycleState::Plenty_ClearNeonNight;
	StormIntensity = 0.0f;
	TargetStormIntensity = 0.0f;
	AerodynamicDragMultiplier = 1.0f;
	RoadPuddleReflectionIntensity = 0.0f;
	RoadSurfaceWetness = 0.0f;

	UE_LOG(LogTemp, Log, TEXT("[WonderWeather] UWonderInclemencyWeatherSubsystem Initialized. Weather Phase: Plenty."));
}

void UWonderInclemencyWeatherSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UWonderInclemencyWeatherSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Update Transition blend
	if (CurrentState == EWeatherCycleState::Transitioning)
	{
		UpdateWeatherTransition(DeltaTime);
	}

	// Update Tempest Dynamics (Turbulence, 85 BPM sync, lightning hazards)
	if (StormIntensity > 0.05f)
	{
		UpdateResonanceTempest(DeltaTime);
	}
	else
	{
		CurrentWindTurbulence = FVector::ZeroVector;
	}

	// Modulate Aerodynamics and Road Puddle Reflections dynamically
	ModulateEnvironmentalEffects(DeltaTime);
}

void UWonderInclemencyWeatherSubsystem::SetWeatherPhase(EWeatherCycleState TargetPhase, float TransitionDuration)
{
	if (TargetPhase == EWeatherCycleState::Transitioning || TargetPhase == CurrentState)
	{
		return;
	}

	DestinationState = TargetPhase;
	CurrentState = EWeatherCycleState::Transitioning;

	TargetStormIntensity = (TargetPhase == EWeatherCycleState::Inclemency_Resonance) ? 1.0f : 0.0f;
	float SafeDuration = FMath::Max(1.0f, TransitionDuration);
	TransitionRate = FMath::Abs(TargetStormIntensity - StormIntensity) / SafeDuration;

	OnWeatherPhaseChanged.Broadcast(CurrentState);

	UE_LOG(LogTemp, Log, TEXT("[WonderWeather] Transitioning weather toward %s over %.1f seconds."),
		(DestinationState == EWeatherCycleState::Inclemency_Resonance ? TEXT("Inclemency") : TEXT("Plenty")),
		SafeDuration);
}

void UWonderInclemencyWeatherSubsystem::UpdateWeatherTransition(float DeltaTime)
{
	StormIntensity = FMath::FInterpTo(StormIntensity, TargetStormIntensity, DeltaTime, TransitionRate * 2.0f);

	if (FMath::IsNearlyEqual(StormIntensity, TargetStormIntensity, 0.02f))
	{
		StormIntensity = TargetStormIntensity;
		CurrentState = DestinationState;
		OnWeatherPhaseChanged.Broadcast(CurrentState);

		UE_LOG(LogTemp, Log, TEXT("[WonderWeather] Weather reached final phase: %s"),
			(CurrentState == EWeatherCycleState::Inclemency_Resonance ? TEXT("Inclemency (Resonance Tempest)") : TEXT("Plenty (Clear Neon Night)")));
	}
}

void UWonderInclemencyWeatherSubsystem::UpdateResonanceTempest(float DeltaTime)
{
	// 85 BPM pulse calculation: T = 60 / 85 = 0.70588 seconds
	float BeatPeriod = 60.0f / FMath::Max(1.0f, TargetBPM);
	BeatTimer += DeltaTime;

	// Calculate swirling wind turbulence vector based on intensity
	float TimeSeconds = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
	float WindAngle = TimeSeconds * 1.5f;
	FVector BaseWind = FVector(FMath::Cos(WindAngle), FMath::Sin(WindAngle), FMath::Sin(TimeSeconds * 3.0f) * 0.3f);
	CurrentWindTurbulence = BaseWind * MaxWindTurbulenceForce * StormIntensity;

	// Periodic Lightning & Feral Mech Spawns during strong storms
	LightningCooldown -= DeltaTime;
	if (LightningCooldown <= 0.0f && StormIntensity > 0.5f)
	{
		LightningCooldown = FMath::RandRange(4.0f, 8.0f);

		APlayerController* PC = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
		if (PC && PC->GetPawn())
		{
			FVector PlayerLoc = PC->GetPawn()->GetActorLocation();
			FVector RandomOffset = FVector(FMath::RandRange(-4000.0f, 4000.0f), FMath::RandRange(-4000.0f, 4000.0f), 0.0f);
			FVector StrikePoint = PlayerLoc + RandomOffset;

			TriggerLightningStrike(StrikePoint);

			// Chance to spawn feral rogue mech on strike location
			if (FMath::FRand() < 0.4f)
			{
				OnFeralRogueMechSpawned.Broadcast(StrikePoint);
				UE_LOG(LogTemp, Warning, TEXT("[WonderWeather] Feral Rogue Mech emerged from resonance lightning at: %s"), *StrikePoint.ToString());
			}
		}
	}
}

void UWonderInclemencyWeatherSubsystem::ModulateEnvironmentalEffects(float DeltaTime)
{
	// Modulate Aerodynamic Drag: 1.0 (baseline) to 1.75 (heavy storm resistance)
	AerodynamicDragMultiplier = FMath::Lerp(1.0f, 1.75f, StormIntensity);

	// Modulate Road Puddle Reflections & Surface Wetness
	// Plenty = dry asphalt (0.05), Inclemency = high mirror puddle reflections (1.0)
	float TargetWetness = FMath::Lerp(0.05f, 1.0f, StormIntensity);
	float TargetReflections = FMath::Lerp(0.15f, MaxPuddleReflectionIntensity, StormIntensity);

	RoadSurfaceWetness = FMath::FInterpTo(RoadSurfaceWetness, TargetWetness, DeltaTime, 0.5f);
	RoadPuddleReflectionIntensity = FMath::FInterpTo(RoadPuddleReflectionIntensity, TargetReflections, DeltaTime, 0.5f);

	OnPuddleReflectionsModulated.Broadcast(RoadPuddleReflectionIntensity, RoadSurfaceWetness);
}

void UWonderInclemencyWeatherSubsystem::TriggerLightningStrike(FVector TargetLocation)
{
	OnResonanceLightningStrike.Broadcast(TargetLocation, LightningStrikeDamage);
	UE_LOG(LogTemp, Log, TEXT("[WonderWeather] Resonance Lightning struck at: %s (Damage: %.1f)"), *TargetLocation.ToString(), LightningStrikeDamage);
}

float UWonderInclemencyWeatherSubsystem::GetCommerceMultiplier() const
{
	// Plenty buffs commerce by 25%; Inclemency drops it to 75%
	return FMath::Lerp(PlentyCommerceBoost, 0.75f, StormIntensity);
}

float UWonderInclemencyWeatherSubsystem::GetVehicleSpeedMultiplier() const
{
	// Plenty buffs vehicle speed by 20%; Inclemency turbulence dampens speed
	return FMath::Lerp(PlentySpeedBoost, 0.85f, StormIntensity);
}
