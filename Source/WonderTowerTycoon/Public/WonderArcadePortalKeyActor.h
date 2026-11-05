// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WonderArcadePortalKeyActor.generated.h"

UENUM(BlueprintType)
enum class EWonderPortalKeyType : uint8
{
	ObsidianMonolith_PS6 UMETA(DisplayName = "The Obsidian Monolith Key (PlayStation 6 / Zen 6)"),
	QuantumGrid_Xbox     UMETA(DisplayName = "The Quantum Grid Key (Xbox Project Helix)"),
	HolographicPrism_VR  UMETA(DisplayName = "The Holographic Prism Key (120Hz Spatial VR)")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPortalKeyUnlocked, EWonderPortalKeyType, KeyType, FString, TargetConsoleRealm);

/**
 * WonderArcadePortalKeyActor
 * The in-world retro-futuristic arcade cabinet that launches the 2.5D platformer mini-game
 * and ejects physical portal keys unlocking next-gen platform sectors across the multiverse.
 */
UCLASS()
class WONDERTOWERTYCOON_API AWonderArcadePortalKeyActor : public AActor
{
	GENERATED_BODY()

public:
	AWonderArcadePortalKeyActor();

	virtual void BeginPlay() override;

	/** Interacts with cabinet to boot the 2.5D mini-game challenge */
	UFUNCTION(BlueprintCallable, Category = "WonderWorld|ArcadePortal")
	void BootArcadeMiniGame();

	/** Completes mini-game and dispenses the target hardware portal key */
	UFUNCTION(BlueprintCallable, Category = "WonderWorld|ArcadePortal")
	void DispensePortalKey(EWonderPortalKeyType KeyType);

	UPROPERTY(BlueprintAssignable, Category = "WonderWorld|ArcadePortal|Events")
	FOnPortalKeyUnlocked OnPortalKeyUnlocked;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WonderWorld|ArcadePortal|Components")
	UStaticMeshComponent* CabinetMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WonderWorld|ArcadePortal|Components")
	UStaticMeshComponent* CrtScreenMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WonderWorld|ArcadePortal|Config")
	EWonderPortalKeyType ActiveRewardKey = EWonderPortalKeyType::ObsidianMonolith_PS6;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WonderWorld|ArcadePortal|State")
	bool bMiniGameActive = false;
};
