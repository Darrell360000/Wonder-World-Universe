// Copyright Epic Games, Inc. All Rights Reserved.

#include "SpiderPulseCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ASpiderPulseCharacter::ASpiderPulseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpiderPulseTopHat = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpiderPulseTopHat"));
	SpiderPulseTopHat->SetupAttachment(GetMesh(), FName("head"));

	GetCharacterMovement()->MaxWalkSpeed = 950.0f;
	GetCharacterMovement()->JumpZVelocity = 850.0f;
	GetCharacterMovement()->AirControl = 0.55f;

	GrappleMaxRange = 6000.0f;
	BioPulseRadius = 1200.0f;
	StunDuration = 3.5f;
}

void ASpiderPulseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

bool ASpiderPulseCharacter::FireSpiderGrapple(FVector TargetLocation)
{
	FVector Origin = GetActorLocation();
	float Distance = FVector::Dist(Origin, TargetLocation);

	if (Distance > GrappleMaxRange)
	{
		return false;
	}

	FVector Direction = (TargetLocation - Origin).GetSafeNormal();
	LaunchCharacter(Direction * 2200.0f + FVector(0, 0, 400.0f), true, true);

	OnSpiderPulseGrappleFired.Broadcast(Origin, TargetLocation);
	return true;
}

void ASpiderPulseCharacter::DischargeBioPulse()
{
	OnBioPulseDischarged.Broadcast(BioPulseRadius);
}
