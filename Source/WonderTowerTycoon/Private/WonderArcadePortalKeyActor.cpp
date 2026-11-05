// Copyright Epic Games, Inc. All Rights Reserved.

#include "WonderArcadePortalKeyActor.h"
#include "Components/StaticMeshComponent.h"

AWonderArcadePortalKeyActor::AWonderArcadePortalKeyActor()
{
	PrimaryActorTick.bCanEverTick = false;

	CabinetMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CabinetMesh"));
	RootComponent = CabinetMesh;

	CrtScreenMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CrtScreenMesh"));
	CrtScreenMesh->SetupAttachment(CabinetMesh);

	ActiveRewardKey = EWonderPortalKeyType::ObsidianMonolith_PS6;
	bMiniGameActive = false;
}

void AWonderArcadePortalKeyActor::BeginPlay()
{
	Super::BeginPlay();
}

void AWonderArcadePortalKeyActor::BootArcadeMiniGame()
{
	bMiniGameActive = true;
}

void AWonderArcadePortalKeyActor::DispensePortalKey(EWonderPortalKeyType KeyType)
{
	bMiniGameActive = false;

	FString TargetRealm;
	switch (KeyType)
	{
	case EWonderPortalKeyType::ObsidianMonolith_PS6:
		TargetRealm = TEXT("PlayStation 6 (Zen 6 / GDDR7 Cinematic Action-RPG Sector)");
		break;
	case EWonderPortalKeyType::QuantumGrid_Xbox:
		TargetRealm = TEXT("Xbox Project Helix (Cross-Platform Tactical Sandbox Sector)");
		break;
	case EWonderPortalKeyType::HolographicPrism_VR:
		TargetRealm = TEXT("Virtual Reality (120Hz Spatial Audio & Haptic Realm)");
		break;
	}

	OnPortalKeyUnlocked.Broadcast(KeyType, TargetRealm);
}
