// Copyright Darrell R. Lewis / Wonder World Trilogy. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WonderVehicleAcrobaticsComponent.generated.h"

UENUM(BlueprintType)
enum class EVehicleAcrobaticManeuver : uint8
{
	None              UMETA(DisplayName = "None / Ground Driving"),
	ForwardFlip       UMETA(DisplayName = "Forward Flip Slam (Rocket League Style)"),
	Backflip          UMETA(DisplayName = "Backflip Vertical Vault"),
	BarrelRollLeft    UMETA(DisplayName = "Left Barrel Roll / Air-Dodge"),
	BarrelRollRight   UMETA(DisplayName = "Right Barrel Roll / Air-Dodge"),
	HeavyBodySlam     UMETA(DisplayName = "Heavy Ground Pound / Body Slam")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVehicleAcrobaticManeuverStarted, EVehicleAcrobaticManeuver, Maneuver);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnBodySlamLanded, FVector, ImpactLocation, float, Radius, int32, VehiclesHitCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSyndicateVehicleLaunched, AActor*, LaunchedVehicle, FVector, LaunchVelocity);

/**
 * UWonderVehicleAcrobaticsComponent
 * Implements Rocket League inspired aerial acrobatics for Wonder World hovercraft & street vehicles.
 * Provides double-tap jump directional aerial flips, left/right barrel rolls, and heavy vehicle body-slams
 * that launch enemy syndicate patrol cars across intersections and through skyscraper glass.
 */
UCLASS(ClassGroup=(WonderWorld), meta=(BlueprintSpawnableComponent))
class WONDERTOWERTYCOON_API UWonderVehicleAcrobaticsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWonderVehicleAcrobaticsComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/* --- ACROBATIC INPUT HOOKS --- */
	UFUNCTION(BlueprintCallable, Category = "Wonder|Vehicle|Acrobatics")
	bool HandleJumpInput(FVector DirectionalInput, bool bIsAirborne);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Vehicle|Acrobatics")
	void TriggerManeuver(EVehicleAcrobaticManeuver Maneuver);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Vehicle|Acrobatics")
	void ExecuteHeavyBodySlam();

	/* --- QUERIES --- */
	UFUNCTION(BlueprintPure, Category = "Wonder|Vehicle|Acrobatics")
	EVehicleAcrobaticManeuver GetActiveManeuver() const { return CurrentManeuver; }

	UFUNCTION(BlueprintPure, Category = "Wonder|Vehicle|Acrobatics")
	bool IsPerformingAcrobatics() const { return CurrentManeuver != EVehicleAcrobaticManeuver::None; }

	/* --- TUNING PARAMETERS --- */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Vehicle|Acrobatics")
	float DoubleTapWindowSeconds = 0.28f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Vehicle|Acrobatics")
	float FlipLinearImpulse = 3500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Vehicle|Acrobatics")
	float FlipAngularVelocity = 720.0f; // deg/sec

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Vehicle|Acrobatics")
	float BodySlamDownwardForce = 8500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Vehicle|Acrobatics")
	float BodySlamImpactRadius = 900.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Vehicle|Acrobatics")
	float BodySlamDamage = 450.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Vehicle|Acrobatics")
	float BodySlamKnockbackImpulse = 4000.0f;

	/* --- EVENTS --- */
	UPROPERTY(BlueprintAssignable, Category = "Wonder|Vehicle|Acrobatics|Events")
	FOnVehicleAcrobaticManeuverStarted OnVehicleAcrobaticManeuverStarted;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|Vehicle|Acrobatics|Events")
	FOnBodySlamLanded OnBodySlamLanded;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|Vehicle|Acrobatics|Events")
	FOnSyndicateVehicleLaunched OnSyndicateVehicleLaunched;

protected:
	virtual void BeginPlay() override;

private:
	void UpdateAcrobaticManeuver(float DeltaTime);
	void DetectSlamImpactGround();
	void ApplyShockwaveToNearbyVehicles(FVector ImpactOrigin);

	EVehicleAcrobaticManeuver CurrentManeuver = EVehicleAcrobaticManeuver::None;
	float ManeuverTimer = 0.0f;
	float ManeuverDuration = 0.65f;

	float LastJumpPressTimestamp = -10.0f;
	bool bAcrobaticAirDodgeAvailable = true;
};
