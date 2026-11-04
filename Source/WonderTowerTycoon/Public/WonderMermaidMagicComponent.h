// Copyright Darrell R. Lewis / Wonder World Trilogy. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WonderMermaidMagicComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOceanicBarrierStateChanged, bool, bIsActive);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSirensSongCymaticHit, FVector, ShockwaveOrigin, int32, EnemiesStunned);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHydroGeyserLaunched, float, LaunchForce);

/**
 * UWonderMermaidMagicComponent
 * Implements the Mermaid Magic (Aquatic Cymatics & Hydro-Sorcery) system for MICCA X-Gen Heroine.
 * Powers high-pressure hydro-geyser vertical launches, 85 BPM siren resonance sonar stuns,
 * polarized saltwater oceanic barriers, and maelstrom whirlpool singularities.
 */
UCLASS(ClassGroup=(WonderWorld), meta=(BlueprintSpawnableComponent))
class WONDERTOWERTYCOON_API UWonderMermaidMagicComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWonderMermaidMagicComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/* --- MERMAID MAGIC ABILITIES --- */
	UFUNCTION(BlueprintCallable, Category = "Wonder|Mermaid Magic")
	void TriggerHydroGeyser();

	UFUNCTION(BlueprintCallable, Category = "Wonder|Mermaid Magic")
	void CastSirensResonanceSong();

	UFUNCTION(BlueprintCallable, Category = "Wonder|Mermaid Magic")
	void ActivateOceanicBarrier(bool bEnable);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Mermaid Magic")
	void SummonMaelstromSingularity();

	UFUNCTION(BlueprintCallable, Category = "Wonder|Mermaid Magic")
	void ToggleSubmergedSwimming(bool bSubmerged);

	/* --- CONFIGURATION & STATS --- */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Mermaid Magic")
	float GeyserLaunchForce = 2200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Mermaid Magic")
	float SirensSongRadius = 1200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Mermaid Magic")
	float MaelstromRadius = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Mermaid Magic")
	float MaelstromPullStrength = 3500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Mermaid Magic")
	float TargetBPM = 85.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Wonder|Mermaid Magic")
	bool bIsSwimmingSubmerged = false;

	UPROPERTY(BlueprintReadOnly, Category = "Wonder|Mermaid Magic")
	bool bOceanicBarrierActive = false;

	/* --- EVENTS --- */
	UPROPERTY(BlueprintAssignable, Category = "Wonder|Mermaid Magic|Events")
	FOnOceanicBarrierStateChanged OnOceanicBarrierStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|Mermaid Magic|Events")
	FOnSirensSongCymaticHit OnSirensSongCymaticHit;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|Mermaid Magic|Events")
	FOnHydroGeyserLaunched OnHydroGeyserLaunched;

private:
	FTimerHandle MaelstromDurationTimer;
	bool bMaelstromActive = false;
	FVector MaelstromLocation = FVector::ZeroVector;
};
