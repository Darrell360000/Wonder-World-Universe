#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WonderTacticalDismembermentComponent.generated.h"

UENUM(BlueprintType)
enum class EDismembermentZone : uint8
{
	LeftHoverTurbine   UMETA(DisplayName = "Left Hover Turbine (Aerodynamic Lift)"),
	RightHoverTurbine  UMETA(DisplayName = "Right Hover Turbine (Aerodynamic Lift)"),
	LeftWeaponMount    UMETA(DisplayName = "Left Weapon Mount (Plasma Cannon)"),
	RightWeaponMount   UMETA(DisplayName = "Right Weapon Mount (Micro-Missile Pod)"),
	LeftKneeActuator   UMETA(DisplayName = "Left Knee Actuator (Locomotion Stance)"),
	RightKneeActuator  UMETA(DisplayName = "Right Knee Actuator (Locomotion Stance)"),
	PowerCore          UMETA(DisplayName = "Central Power Core (Critical Overcharge)")
};

USTRUCT(BlueprintType)
struct WONDERTOWERTYCOON_API FTacticalDamageZone
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Dismemberment")
	EDismembermentZone Zone = EDismembermentZone::LeftHoverTurbine;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Dismemberment")
	FName TargetBoneName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Dismemberment")
	float ZoneHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Dismemberment")
	float MaxZoneHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Dismemberment")
	bool bIsSevered = false;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnZoneDamaged, EDismembermentZone, Zone, float, RemainingHealthPercent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnZoneSevered, EDismembermentZone, Zone, FVector, SeverLocation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAerodynamicSpinOutTriggered, float, SpinOutIntensity);

UCLASS(ClassGroup=(WonderWorld), meta=(BlueprintSpawnableComponent))
class WONDERTOWERTYCOON_API UWonderTacticalDismembermentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWonderTacticalDismembermentComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/* --- DAMAGE ZONES --- */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Dismemberment")
	TMap<EDismembermentZone, FTacticalDamageZone> DamageZones;

	UFUNCTION(BlueprintCallable, Category = "Wonder|Dismemberment - Actions")
	void ApplyZoneDamage(EDismembermentZone Zone, float DamageAmount, FVector HitLocation);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Dismemberment - Actions")
	void ApplyBoneDamage(FName HitBoneName, float DamageAmount, FVector HitLocation);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Dismemberment - Actions")
	void SeverZone(EDismembermentZone Zone, FVector HitLocation);

	UFUNCTION(BlueprintPure, Category = "Wonder|Dismemberment - Queries")
	bool IsZoneSevered(EDismembermentZone Zone) const;

	/* --- AERODYNAMIC SPIN-OUT & PHYSICS --- */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wonder|Dismemberment - Physics")
	bool bIsSpinningOut = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Dismemberment - Physics")
	float SpinOutTorque = 45000.0f;

	UFUNCTION(BlueprintCallable, Category = "Wonder|Dismemberment - Actions")
	void TriggerAerodynamicSpinOut(float Intensity = 1.0f);

	/* --- EVENTS --- */
	UPROPERTY(BlueprintAssignable, Category = "Wonder|Dismemberment|Events")
	FOnZoneDamaged OnZoneDamaged;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|Dismemberment|Events")
	FOnZoneSevered OnZoneSevered;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|Dismemberment|Events")
	FOnAerodynamicSpinOutTriggered OnAerodynamicSpinOutTriggered;

private:
	void InitializeDefaultZones();
};
