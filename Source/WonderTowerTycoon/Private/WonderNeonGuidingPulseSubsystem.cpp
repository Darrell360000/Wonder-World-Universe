#include "WonderNeonGuidingPulseSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Components/LightComponent.h"
#include "Engine/World.h"

void UWonderNeonGuidingPulseSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	PulseTimer = 0.0f;
}

TStatId UWonderNeonGuidingPulseSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UWonderNeonGuidingPulseSubsystem, STATGROUP_Tickables);
}

void UWonderNeonGuidingPulseSubsystem::SetMissionWaypoint(FVector WaypointLocation)
{
	ActiveWaypointLocation = WaypointLocation;
	bIsGuidingActive = true;
	PulseTimer = 0.0f;

	UE_LOG(LogTemp, Log, TEXT("[WonderNavigation] Active mission waypoint set at (X=%.1f, Y=%.1f, Z=%.1f). Guiding neon wind initiated."),
		WaypointLocation.X, WaypointLocation.Y, WaypointLocation.Z);

	OnMissionWaypointSet.Broadcast(WaypointLocation);
}

void UWonderNeonGuidingPulseSubsystem::ClearMissionWaypoint()
{
	bIsGuidingActive = false;
	ActiveWaypointLocation = FVector::ZeroVector;

	UE_LOG(LogTemp, Log, TEXT("[WonderNavigation] Mission waypoint cleared. Guiding neon wind stopped."));

	OnMissionWaypointCleared.Broadcast();
}

FVector UWonderNeonGuidingPulseSubsystem::GetCurrentGuidingDirection(FVector PlayerLocation) const
{
	if (!bIsGuidingActive) return FVector::ZeroVector;
	return (ActiveWaypointLocation - PlayerLocation).GetSafeNormal();
}

void UWonderNeonGuidingPulseSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsGuidingActive || !GetWorld()) return;

	SequenceStreetlightsAndMist(DeltaTime);
}

void UWonderNeonGuidingPulseSubsystem::SequenceStreetlightsAndMist(float DeltaTime)
{
	const float BeatInterval = 60.0f / FMath::Max(1.0f, TargetBPM);
	PulseTimer += DeltaTime;

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn) return;

	const FVector PlayerLoc = PlayerPawn->GetActorLocation();
	const FVector GuidingDir = (ActiveWaypointLocation - PlayerLoc).GetSafeNormal();
	const float PulsePhase = FMath::Clamp(PulseTimer / BeatInterval, 0.0f, 1.0f);

	if (PulseTimer >= BeatInterval)
	{
		PulseTimer = FMath::Fmod(PulseTimer, BeatInterval);

		// Broadcast rhythmic diegetic pulse to audio, streetlights, and volumetric mist
		OnWaypointPulseTriggered.Broadcast(ActiveWaypointLocation, GuidingDir, 1.0f);
	}
}
