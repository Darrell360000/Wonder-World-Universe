#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WonderAdvancedCombatComponent.generated.h"

UCLASS(ClassGroup=(WonderWorld), meta=(BlueprintSpawnableComponent))
class WONDERTOWERTYCOON_API UWonderAdvancedCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWonderAdvancedCombatComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/* --- GOD OF WAR SONIC DISC RECALL --- */
	UFUNCTION(BlueprintCallable, Category = "Wonder|Combat - Recall")
	void ThrowSonicDisc(FVector ForwardDirection);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Combat - Recall")
	void RecallSonicDisc();

	UPROPERTY(BlueprintReadOnly, Category = "Wonder|Combat - Recall")
	AActor* ActiveThrownDisc = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Wonder|Combat - Recall")
	bool bIsDiscThrown = false;

	UPROPERTY(BlueprintReadOnly, Category = "Wonder|Combat - Recall")
	bool bIsRecalling = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Combat - Recall")
	float DiscSpeed = 2800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Combat - Recall")
	float DiscRecallSpeed = 3800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Combat - Recall")
	float DiscMaxDistance = 3200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Combat - Recall")
	float DiscDamage = 75.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Combat - Recall")
	TSubclassOf<AActor> DiscClass;

	/* --- MGR / SUPERHOT TIME DILATION --- */
	UFUNCTION(BlueprintCallable, Category = "Wonder|Combat - Dilation")
	void ToggleHarmonicDilation();

	UPROPERTY(BlueprintReadOnly, Category = "Wonder|Combat - Dilation")
	bool bInDilationMode = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Combat - Dilation")
	float DilationTimeScale = 0.2f;

	/* --- HADES FAIL-FORWARD RESPAWN --- */
	UFUNCTION(BlueprintCallable, Category = "Wonder|Progression")
	void TriggerCinemaFailForwardRespawn();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Progression")
	FVector CinemaRespawnLocation = FVector(0.0f, 0.0f, 150.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Progression")
	FRotator CinemaRespawnRotation = FRotator(0.0f, 0.0f, 0.0f);

private:
	FVector ThrownStartLocation = FVector::ZeroVector;
	FVector ThrownFlightDirection = FVector::ForwardVector;
	float ThrownFlightDistanceTraveled = 0.0f;
};
