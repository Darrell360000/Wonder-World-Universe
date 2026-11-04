#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "WonderNeonGuidingPulseSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnWaypointPulseTriggered, FVector, WaypointLocation, FVector, GuidingDirection, float, PulsePhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMissionWaypointSet, FVector, WaypointLocation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMissionWaypointCleared);

UCLASS()
class WONDERTOWERTYCOON_API UWonderNeonGuidingPulseSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	/* --- WAYPOINT NAVIGATION --- */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wonder|Navigation")
	FVector ActiveWaypointLocation = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wonder|Navigation")
	bool bIsGuidingActive = false;

	UFUNCTION(BlueprintCallable, Category = "Wonder|Navigation - Actions")
	void SetMissionWaypoint(FVector WaypointLocation);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Navigation - Actions")
	void ClearMissionWaypoint();

	/* --- DIEGETIC PULSE & VOLUMETRIC TUNING --- */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Navigation - Tuning")
	float TargetBPM = 85.0f; // 85 BPM audio pulse clock

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Navigation - Tuning")
	FLinearColor GuidingMistColor = FLinearColor(0.65f, 0.1f, 1.0f, 1.0f); // Neon purple

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Navigation - Tuning")
	float StreetlightPulseRadius = 5000.0f;

	UFUNCTION(BlueprintPure, Category = "Wonder|Navigation - Queries")
	FVector GetCurrentGuidingDirection(FVector PlayerLocation) const;

	/* --- EVENTS --- */
	UPROPERTY(BlueprintAssignable, Category = "Wonder|Navigation|Events")
	FOnWaypointPulseTriggered OnWaypointPulseTriggered;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|Navigation|Events")
	FOnMissionWaypointSet OnMissionWaypointSet;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|Navigation|Events")
	FOnMissionWaypointCleared OnMissionWaypointCleared;

private:
	float PulseTimer = 0.0f;
	void SequenceStreetlightsAndMist(float DeltaTime);
};
