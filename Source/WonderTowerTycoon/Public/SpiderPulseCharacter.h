// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SpiderPulseCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSpiderPulseGrappleFired, FVector, StartPoint, FVector, HitPoint);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBioPulseDischarged, float, StunRadius);

/**
 * SpiderPulseCharacter
 * The Female Top-Hatter Hero ("The Spider-Pulse Icon"):
 * - High-mobility heroine wearing a pink-and-white aerodynamic suit & theatrical pink-banded top hat.
 * - Cybernetic forearm gauntlets channel rhythmic audio-pulse energy.
 * - Executes vertical skyscraper web-line swinging and bio-electric pulse crowd stuns.
 */
UCLASS()
class WONDERTOWERTYCOON_API ASpiderPulseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASpiderPulseCharacter();

	virtual void BeginPlay() override;

	/** Fires a rhythmic bio-luminescent spider pulse web-line to target surface */
	UFUNCTION(BlueprintCallable, Category = "WonderWorld|SpiderPulse")
	bool FireSpiderGrapple(FVector TargetLocation);

	/** Discharges an omnidirectional bio-electric pulse that stuns hostile Copycat units */
	UFUNCTION(BlueprintCallable, Category = "WonderWorld|SpiderPulse")
	void DischargeBioPulse();

	UPROPERTY(BlueprintAssignable, Category = "WonderWorld|SpiderPulse|Events")
	FOnSpiderPulseGrappleFired OnSpiderPulseGrappleFired;

	UPROPERTY(BlueprintAssignable, Category = "WonderWorld|SpiderPulse|Events")
	FOnBioPulseDischarged OnBioPulseDischarged;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WonderWorld|SpiderPulse|Components")
	UStaticMeshComponent* SpiderPulseTopHat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WonderWorld|SpiderPulse|Stats")
	float GrappleMaxRange = 6000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WonderWorld|SpiderPulse|Stats")
	float BioPulseRadius = 1200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WonderWorld|SpiderPulse|Stats")
	float StunDuration = 3.5f;
};
