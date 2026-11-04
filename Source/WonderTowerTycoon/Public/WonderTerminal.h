// Copyright Darrell R. Lewis / Wonder World Trilogy. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WonderTerminal.generated.h"

class AWonderSkyscraperActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTakeoverComplete, AWonderTerminal*, Terminal);

/**
 * AWonderTerminal
 * Interactive Masterframe terminal placed in penthouses and corporate vaults.
 * Interacted with via Darrell's hand-slam to trigger the Hostile Takeover loop.
 */
UCLASS()
class WONDERTOWERTYCOON_API AWonderTerminal : public AActor
{
	GENERATED_BODY()
	
public:	
	AWonderTerminal();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wonder|Terminal")
	UStaticMeshComponent* TerminalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Terminal")
	bool bConverted = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Terminal")
	AWonderSkyscraperActor* ParentSkyscraper = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Terminal")
	float TakeoverRevenueBonus = 25000.0f;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|Conversion|Events")
	FOnTakeoverComplete OnTakeoverComplete;

	UFUNCTION(BlueprintCallable, Category = "Wonder|Conversion")
	void TriggerHostileTakeover(); // on AWonderTerminal, called from a hand-slam interaction
};
