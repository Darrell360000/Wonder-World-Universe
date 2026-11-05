// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TaoXZenCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChronoPortalCast, FVector, PortalOrigin, FVector, ExitPoint);

/**
 * TaoXZenCharacter
 * Tao X Zen (The Wonder Hat / Chrono-Warden):
 * - Midnight-blue circuitry suit and wizard-style top hat banded with glowing pink-and-gold Taoist sigils.
 * - Miniature purple cosmic vortex portal floats above the crown hat.
 * - Opens dimensional rifts, alters local time dilation, and redirects kinetic incoming damage.
 */
UCLASS()
class WONDERTOWERTYCOON_API ATaoXZenCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ATaoXZenCharacter();

	virtual void BeginPlay() override;

	/** Opens a localized purple cosmic wormhole between two points */
	UFUNCTION(BlueprintCallable, Category = "WonderWorld|TaoXZen")
	bool CastChronoPortal(FVector ExitPoint);

	UPROPERTY(BlueprintAssignable, Category = "WonderWorld|TaoXZen|Events")
	FOnChronoPortalCast OnChronoPortalCast;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WonderWorld|TaoXZen|Components")
	UStaticMeshComponent* TaoHatMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WonderWorld|TaoXZen|Stats")
	float MaxPortalDistance = 8000.0f;
};
