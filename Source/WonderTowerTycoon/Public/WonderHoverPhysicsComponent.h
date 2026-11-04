#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "WonderTowerTycoon.h"
#include "WonderHoverPhysicsComponent.generated.h"

UCLASS(ClassGroup=(WonderWorld), meta=(BlueprintSpawnableComponent))
class WONDERTOWERTYCOON_API UWonderHoverPhysicsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWonderHoverPhysicsComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/* --- TUNING PARAMETERS --- */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Hover - Suspension")
	float DesiredHoverHeight = 32.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Hover - Suspension")
	float MinHoverHeight = 12.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Hover - Suspension")
	float MaxHoverHeight = 96.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Hover - Suspension")
	float SpringStiffness = 350.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Hover - Suspension")
	float Damping = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Hover - Suspension")
	float StabilizationStrength = 85.0f;

	/* --- NIAGARA VISUALS --- */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Hover - Visuals")
	UNiagaraSystem* ThrusterNiagaraSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Hover - Visuals")
	FLinearColor ThrusterGlowColor = FLinearColor(0.1f, 0.6f, 1.0f, 1.0f);

	/* --- CONTROLS --- */
	UFUNCTION(BlueprintCallable, Category = "Wonder|Hover - Controls")
	void ToggleHover();

	UFUNCTION(BlueprintCallable, Category = "Wonder|Hover - Controls")
	void Ascend(float Rate = 1.0f);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Hover - Controls")
	void Descend(float Rate = 1.0f);

	void HandleAscend();
	void HandleDescend();

	UFUNCTION(BlueprintCallable, Category = "Wonder|Hover - Controls")
	void SetupHoverInputComponent(UInputComponent* Input);

	UPROPERTY(BlueprintReadOnly, Category = "Wonder|Hover - State")
	bool bHoverActive = false;

private:
	UPROPERTY()
	UPrimitiveComponent* RootPrim;

	UPROPERTY()
	TArray<UNiagaraComponent*> SpawnedThrusterFX;

	void ApplyHoverForces(float DeltaTime);
	void UpdateThrusterVisuals();
};
