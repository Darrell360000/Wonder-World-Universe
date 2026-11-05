// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MinimeTophatterCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMinimeScoutingStateChanged, bool, bIsDetached);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCopycatTargetTagged, AActor*, TargetActor, FVector, Location);

/**
 * MinimeTophatterCharacter
 * Darrell X-Gen's miniature tactical advisor and hyper-agile sidekick:
 * - Wears a matching miniature digital LED equalizer top hat.
 * - Perches on Darrell's shoulder (clavicle_l) or detaches for tactical scouting.
 * - Scans Copycats, locks geometric light codes into the municipal grid, and boosts Darrell's Rhythm Heat.
 */
UCLASS()
class WONDERTOWERTYCOON_API AMinimeTophatterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMinimeTophatterCharacter();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	/** Attaches Minime to a host character's shoulder socket */
	UFUNCTION(BlueprintCallable, Category = "WonderWorld|Minime")
	void AttachToHost(ACharacter* HostCharacter, FName SocketName = "clavicle_l");

	/** Detaches Minime to enter tactical scout hover mode */
	UFUNCTION(BlueprintCallable, Category = "WonderWorld|Minime")
	void DetachToScout(FVector TargetLocation);

	/** Tags hostile Copycat faction units within radar range */
	UFUNCTION(BlueprintCallable, Category = "WonderWorld|Minime")
	void ScanAndTagCopycats(float ScanRadius = 2000.0f);

	UFUNCTION(BlueprintPure, Category = "WonderWorld|Minime")
	bool IsAttachedToHost() const { return bIsAttached; }

	UPROPERTY(BlueprintAssignable, Category = "WonderWorld|Minime|Events")
	FOnMinimeScoutingStateChanged OnMinimeScoutingStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "WonderWorld|Minime|Events")
	FOnCopycatTargetTagged OnCopycatTargetTagged;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WonderWorld|Minime|Components")
	UStaticMeshComponent* MiniTopHatMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WonderWorld|Minime|State")
	bool bIsAttached = true;

	UPROPERTY(BlueprintReadOnly, Category = "WonderWorld|Minime|State")
	TWeakObjectPtr<ACharacter> CurrentHost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WonderWorld|Minime|Config")
	float HoverFlightSpeed = 1200.0f;
};
