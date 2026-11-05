// Copyright Epic Games, Inc. All Rights Reserved.

#include "WonderMacroToStreetCameraComponent.h"
#include "GameFramework/Actor.h"

UWonderMacroToStreetCameraComponent::UWonderMacroToStreetCameraComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	CurrentPerspective = EWonderCameraPerspective::SatelliteMacroCity;
	ZoomDuration = 1.5f;
	SatelliteAltitude = 35000.0f;
	StreetCameraOffset = FVector(-600.0f, 0.0f, 250.0f);
	bIsTransitioning = false;
	TransitionElapsed = 0.0f;
}

void UWonderMacroToStreetCameraComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UWonderMacroToStreetCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsTransitioning)
	{
		TransitionElapsed += DeltaTime;
		float Alpha = FMath::Clamp(TransitionElapsed / ZoomDuration, 0.0f, 1.0f);
		float SmoothAlpha = FMath::SmoothStep(0.0f, 1.0f, Alpha);

		FVector NewLoc = FMath::Lerp(StartCamLocation, TargetCamLocation, SmoothAlpha);
		FRotator NewRot = FMath::Lerp(StartCamRotation, TargetCamRotation, SmoothAlpha);

		AActor* Owner = GetOwner();
		if (Owner)
		{
			Owner->SetActorLocationAndRotation(NewLoc, NewRot);
		}

		if (Alpha >= 1.0f)
		{
			bIsTransitioning = false;
			OnCameraPerspectiveTransitioned.Broadcast(CurrentPerspective, TargetCamLocation);
		}
	}
}

void UWonderMacroToStreetCameraComponent::SmoothZoomToStreet(FVector TargetStreetLocation)
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	StartCamLocation = Owner->GetActorLocation();
	StartCamRotation = Owner->GetActorRotation();

	TargetCamLocation = TargetStreetLocation + StreetCameraOffset;
	TargetCamRotation = FRotator(-15.0f, 0.0f, 0.0f);

	TransitionElapsed = 0.0f;
	bIsTransitioning = true;
	CurrentPerspective = EWonderCameraPerspective::StreetLevelBrawler;
}

void UWonderMacroToStreetCameraComponent::SmoothZoomToSatellite(FVector CityCenterLocation)
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	StartCamLocation = Owner->GetActorLocation();
	StartCamRotation = Owner->GetActorRotation();

	TargetCamLocation = FVector(CityCenterLocation.X, CityCenterLocation.Y, SatelliteAltitude);
	TargetCamRotation = FRotator(-85.0f, 0.0f, 0.0f);

	TransitionElapsed = 0.0f;
	bIsTransitioning = true;
	CurrentPerspective = EWonderCameraPerspective::SatelliteMacroCity;
}
