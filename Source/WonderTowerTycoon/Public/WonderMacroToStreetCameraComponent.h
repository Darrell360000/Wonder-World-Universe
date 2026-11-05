// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WonderMacroToStreetCameraComponent.generated.h"

UENUM(BlueprintType)
enum class EWonderCameraPerspective : uint8
{
	SatelliteMacroCity UMETA(DisplayName = "Satellite Macro City-Builder View"),
	MidLevelDefense    UMETA(DisplayName = "Mid-Level Tower Defense View"),
	StreetLevelBrawler UMETA(DisplayName = "Street-Level Character Brawler View")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCameraPerspectiveTransitioned, EWonderCameraPerspective, NewPerspective, FVector, FocusPoint);

/**
 * WonderMacroToStreetCameraComponent
 * Manages seamless transitions between:
 * - Macro City-Building satellite perspective (Cities: Skylines scale)
 * - Street-level third-person action brawler view (locking onto Darrell X-Gen / squad)
 */
UCLASS(ClassGroup=(WonderWorld), meta=(BlueprintSpawnableComponent))
class WONDERTOWERTYCOON_API UWonderMacroToStreetCameraComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWonderMacroToStreetCameraComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Smoothly zooms from satellite overview down to target street building / player */
	UFUNCTION(BlueprintCallable, Category = "WonderWorld|CameraDirector")
	void SmoothZoomToStreet(FVector TargetStreetLocation);

	/** Smoothly zooms up from street pavement to high-altitude city satellite view */
	UFUNCTION(BlueprintCallable, Category = "WonderWorld|CameraDirector")
	void SmoothZoomToSatellite(FVector CityCenterLocation);

	UFUNCTION(BlueprintPure, Category = "WonderWorld|CameraDirector")
	EWonderCameraPerspective GetCurrentPerspective() const { return CurrentPerspective; }

	UFUNCTION(BlueprintPure, Category = "WonderWorld|CameraDirector")
	bool IsTransitioning() const { return bIsTransitioning; }

	UPROPERTY(BlueprintAssignable, Category = "WonderWorld|CameraDirector|Events")
	FOnCameraPerspectiveTransitioned OnCameraPerspectiveTransitioned;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WonderWorld|CameraDirector|State")
	EWonderCameraPerspective CurrentPerspective = EWonderCameraPerspective::SatelliteMacroCity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WonderWorld|CameraDirector|Config")
	float ZoomDuration = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WonderWorld|CameraDirector|Config")
	float SatelliteAltitude = 35000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WonderWorld|CameraDirector|Config")
	FVector StreetCameraOffset = FVector(-600.0f, 0.0f, 250.0f);

	bool bIsTransitioning = false;
	float TransitionElapsed = 0.0f;
	FVector StartCamLocation;
	FVector TargetCamLocation;
	FRotator StartCamRotation;
	FRotator TargetCamRotation;
};
