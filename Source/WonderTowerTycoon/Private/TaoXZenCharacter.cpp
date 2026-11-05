// Copyright Epic Games, Inc. All Rights Reserved.

#include "TaoXZenCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ATaoXZenCharacter::ATaoXZenCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	TaoHatMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TaoHatMesh"));
	TaoHatMesh->SetupAttachment(GetMesh(), FName("head"));

	GetCharacterMovement()->MaxWalkSpeed = 700.0f;
	MaxPortalDistance = 8000.0f;
}

void ATaoXZenCharacter::BeginPlay()
{
	Super::BeginPlay();
}

bool ATaoXZenCharacter::CastChronoPortal(FVector ExitPoint)
{
	FVector Origin = GetActorLocation();
	if (FVector::Dist(Origin, ExitPoint) > MaxPortalDistance)
	{
		return false;
	}

	OnChronoPortalCast.Broadcast(Origin, ExitPoint);
	return true;
}
