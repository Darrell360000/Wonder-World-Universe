// Copyright Darrell R. Lewis / Wonder World Trilogy. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WonderShieldSawComponent.generated.h"

UENUM(BlueprintType)
enum class EShieldSawState : uint8
{
	Idle            UMETA(DisplayName = "Idle / Holstered in God Hand"),
	SpinningMelee   UMETA(DisplayName = "Spinning Circular Saw Melee"),
	ActiveBlock     UMETA(DisplayName = "Active Block (LMB + RMB Guard)"),
	HeavyParry      UMETA(DisplayName = "Heavy Parry Guard (Tank Shell Deflection)"),
	Thrown          UMETA(DisplayName = "High-Speed Thrown Projectile Flight"),
	Recalling       UMETA(DisplayName = "Magnetic Recall to God Hand Socket"),
	DoorCutting     UMETA(DisplayName = "Cutting Skyscraper Security Door")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShieldSawStateChanged, EShieldSawState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHeavyParryTriggered, FVector, DeflectDirection, AActor*, DeflectedAttacker);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnProjectileDeflected, AActor*, DeflectedProjectile, FVector, ReflectedVelocity);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDoorBreachCompleted, AActor*, BreachedDoor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBarricadeCarved, AActor*, CarvedObstacle, float, DamageDealt);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShieldSawRecalled);

/**
 * UWonderShieldSawComponent
 * Implements the 2025/2026 DOOM: The Dark Ages inspired God Hand Shield Saw (LMB + RMB).
 * Darrell spins the razor-rimmed Tophatter kinetic shield disc into an active circular saw.
 * Provides active blocking, projectile deflection, high-speed throw, and magnetic recall physics
 * for carving through reinforced doors and street barricades.
 */
UCLASS(ClassGroup=(WonderWorld), meta=(BlueprintSpawnableComponent))
class WONDERTOWERTYCOON_API UWonderShieldSawComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWonderShieldSawComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/* --- COMBAT & BLOCK ACTIONS --- */
	UFUNCTION(BlueprintCallable, Category = "Wonder|Combat|ShieldSaw")
	void StartSpinningMelee();

	UFUNCTION(BlueprintCallable, Category = "Wonder|Combat|ShieldSaw")
	void StopSpinningMelee();

	/** Activates LMB + RMB active block mode */
	UFUNCTION(BlueprintCallable, Category = "Wonder|Combat|ShieldSaw")
	void StartActiveBlock();

	UFUNCTION(BlueprintCallable, Category = "Wonder|Combat|ShieldSaw")
	void StopActiveBlock();

	/** Precision parry trigger for heavy tank shells */
	UFUNCTION(BlueprintCallable, Category = "Wonder|Combat|ShieldSaw")
	bool TriggerHeavyParry();

	/** Deflects incoming projectile back along reflection vector */
	UFUNCTION(BlueprintCallable, Category = "Wonder|Combat|ShieldSaw")
	bool TryDeflectProjectile(AActor* IncomingProjectile, FVector ImpactPoint, FVector HitNormal);

	/* --- THROW & RECALL PHYSICS --- */
	UFUNCTION(BlueprintCallable, Category = "Wonder|Combat|ShieldSaw")
	bool ThrowShieldSaw(FVector AimDirection);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Combat|ShieldSaw")
	void RecallShieldSaw();

	/* --- BREACHING & CARVING --- */
	UFUNCTION(BlueprintCallable, Category = "Wonder|Combat|ShieldSaw")
	void StartCuttingDoor(AActor* TargetDoorActor, float CutTimeRequired = 3.0f);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Combat|ShieldSaw")
	void CancelDoorCutting();

	UFUNCTION(BlueprintCallable, Category = "Wonder|Combat|ShieldSaw")
	void CarveBarricade(AActor* ObstacleActor, float DeltaTime);

	/* --- QUERIES --- */
	UFUNCTION(BlueprintPure, Category = "Wonder|Combat|ShieldSaw")
	EShieldSawState GetCurrentSawState() const { return CurrentState; }

	UFUNCTION(BlueprintPure, Category = "Wonder|Combat|ShieldSaw")
	bool IsActiveBlocking() const { return CurrentState == EShieldSawState::ActiveBlock || CurrentState == EShieldSawState::HeavyParry; }

	UFUNCTION(BlueprintPure, Category = "Wonder|Combat|ShieldSaw")
	bool CanParryHeavyAttack() const;

	UFUNCTION(BlueprintPure, Category = "Wonder|Combat|ShieldSaw")
	float GetDoorCuttingProgress() const { return DoorCuttingProgress; }

	UFUNCTION(BlueprintPure, Category = "Wonder|Combat|ShieldSaw")
	FVector GetCurrentDiscLocation() const { return CurrentDiscLocation; }

	/* --- TUNING PARAMETERS --- */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|ShieldSaw|Tuning")
	float SawRPM = 7200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|ShieldSaw|Tuning")
	float BaseDamagePerSecond = 450.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|ShieldSaw|Tuning")
	float HeavyParryWindowDuration = 0.35f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|ShieldSaw|Tuning")
	float ThrowSpeed = 4200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|ShieldSaw|Tuning")
	float MaxThrowDistance = 3500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|ShieldSaw|Tuning")
	float RecallSpeed = 5500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|ShieldSaw|Tuning")
	float CuttingRange = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|ShieldSaw|Tuning")
	float DeflectedProjectileSpeedBoost = 1.5f;

	/* --- EVENTS --- */
	UPROPERTY(BlueprintAssignable, Category = "Wonder|ShieldSaw|Events")
	FOnShieldSawStateChanged OnShieldSawStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|ShieldSaw|Events")
	FOnHeavyParryTriggered OnHeavyParryTriggered;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|ShieldSaw|Events")
	FOnProjectileDeflected OnProjectileDeflected;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|ShieldSaw|Events")
	FOnDoorBreachCompleted OnDoorBreachCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|ShieldSaw|Events")
	FOnBarricadeCarved OnBarricadeCarved;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|ShieldSaw|Events")
	FOnShieldSawRecalled OnShieldSawRecalled;

protected:
	virtual void BeginPlay() override;

private:
	void SetSawState(EShieldSawState NewState);
	void UpdateThrownFlight(float DeltaTime);
	void UpdateRecallFlight(float DeltaTime);
	void UpdateDoorCutting(float DeltaTime);

	UPROPERTY()
	EShieldSawState CurrentState = EShieldSawState::Idle;

	float HeavyParryTimer = 0.0f;
	FVector CurrentDiscLocation = FVector::ZeroVector;
	FVector ThrownVelocity = FVector::ZeroVector;
	FVector ThrowOrigin = FVector::ZeroVector;

	UPROPERTY()
	TWeakObjectPtr<AActor> TargetedDoor = nullptr;
	float DoorCutDuration = 3.0f;
	float DoorCutElapsedTime = 0.0f;
	float DoorCuttingProgress = 0.0f;
};
