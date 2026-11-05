// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WonderIronRiffComponent.h"
#include "JaxsonThorneCharacter.generated.h"

/**
 * JaxsonThorneCharacter
 * Jaxson "Iron Riff" Thorne (The Heavy Industrial Brawler):
 * - Reinforced tactical vest, pneumatic sonic-gauntlet fists with electric amber lightning.
 * - Iron-plated "Anvil Crown" top hat with hydraulic shock absorbers & hazard beacons.
 * - Delivers heavy sub-bass seismic ground slams.
 */
UCLASS()
class WONDERTOWERTYCOON_API AJaxsonThorneCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AJaxsonThorneCharacter();

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WonderWorld|Jaxson|Components")
	UWonderIronRiffComponent* IronRiffComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WonderWorld|Jaxson|Components")
	UStaticMeshComponent* AnvilCrownTopHatMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WonderWorld|Jaxson|Components")
	UStaticMeshComponent* LeftGauntletMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WonderWorld|Jaxson|Components")
	UStaticMeshComponent* RightGauntletMesh;
};
