// Copyright Epic Games, Inc. All Rights Reserved.

#include "MamaWonderCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AMamaWonderCharacter::AMamaWonderCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	MamaEqualizerCrownMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MamaEqualizerCrownMesh"));
	MamaEqualizerCrownMesh->SetupAttachment(GetMesh(), FName("head"));

	ConductorBatonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ConductorBatonMesh"));
	ConductorBatonMesh->SetupAttachment(GetMesh(), FName("hand_r"));

	GetCharacterMovement()->MaxWalkSpeed = 650.0f;
	bPermissionGranted = true;
}

void AMamaWonderCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AMamaWonderCharacter::GrantEvonHubPermission(const FString& SectorId)
{
	bPermissionGranted = true;
	OnEvonHubPermissionIssued.Broadcast(SectorId, true);
}

void AMamaWonderCharacter::DisburseMunicipalDividends(int64 AmountUsd)
{
	OnMunicipalDividendDisbursed.Broadcast(AmountUsd);
}
