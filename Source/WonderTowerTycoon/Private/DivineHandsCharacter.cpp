// Copyright Epic Games, Inc. All Rights Reserved.

#include "DivineHandsCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ADivineHandsCharacter::ADivineHandsCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	DivineHandsComponent = CreateDefaultSubobject<UWonderDivineHandsComponent>(TEXT("DivineHandsComponent"));

	CrownTopHatMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CrownTopHatMesh"));
	CrownTopHatMesh->SetupAttachment(GetMesh(), FName("head"));

	GetCharacterMovement()->MaxWalkSpeed = 650.0f;
}

void ADivineHandsCharacter::BeginPlay()
{
	Super::BeginPlay();
}
