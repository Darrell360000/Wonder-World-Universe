// Copyright Epic Games, Inc. All Rights Reserved.

#include "JaxsonThorneCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AJaxsonThorneCharacter::AJaxsonThorneCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	IronRiffComponent = CreateDefaultSubobject<UWonderIronRiffComponent>(TEXT("IronRiffComponent"));

	AnvilCrownTopHatMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AnvilCrownTopHatMesh"));
	AnvilCrownTopHatMesh->SetupAttachment(GetMesh(), FName("head"));

	LeftGauntletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftGauntletMesh"));
	LeftGauntletMesh->SetupAttachment(GetMesh(), FName("hand_l"));

	RightGauntletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightGauntletMesh"));
	RightGauntletMesh->SetupAttachment(GetMesh(), FName("hand_r"));

	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
}

void AJaxsonThorneCharacter::BeginPlay()
{
	Super::BeginPlay();
}
