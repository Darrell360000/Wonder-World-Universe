// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WonderDivineHandsComponent.h"
#include "DivineHandsCharacter.generated.h"

/**
 * DivineHandsCharacter
 * The Divine Hands Top-Hatter ("God's Hands Hat"):
 * - Celestial warden in a white-and-gold coat whose top hat is crowned by floating ethereal translucent hands.
 * - Shapes macro-architecture and snaps modular tower blocks onto the 8m x 8m municipal grid.
 */
UCLASS()
class WONDERTOWERTYCOON_API ADivineHandsCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ADivineHandsCharacter();

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WonderWorld|DivineHands|Components")
	UWonderDivineHandsComponent* DivineHandsComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WonderWorld|DivineHands|Components")
	UStaticMeshComponent* CrownTopHatMesh;
};
