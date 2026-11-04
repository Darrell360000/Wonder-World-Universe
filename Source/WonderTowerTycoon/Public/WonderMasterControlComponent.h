#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WonderMasterControlComponent.generated.h"

class ACharacter;
class APawn;
class UCharacterMovementComponent;
class UWonderVehicleInteractComponent;

UENUM(BlueprintType)
enum class EWonderControlMode : uint8
{
	OnFoot UMETA(DisplayName = "On-Foot (Street Brawler & Parkour)"),
	InVehicle UMETA(DisplayName = "In-Vehicle (Street Drive & Hover Flight)")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnControlModeChanged, EWonderControlMode, NewMode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBioRallyHealthRecovered, float, AmountRecovered);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSynergyComboExecuted, FString, SynergyName);

UCLASS(ClassGroup = (WonderWorld), meta = (BlueprintSpawnableComponent))
class WONDERTOWERTYCOON_API UWonderMasterControlComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWonderMasterControlComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Bind all actions and axes to the given InputComponent */
	UFUNCTION(BlueprintCallable, Category = "Wonder|Controls")
	void SetupControlBindings(UInputComponent* PlayerInputComponent);

	/* --- MODE & STATE --- */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Controls")
	EWonderControlMode ControlMode = EWonderControlMode::OnFoot;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|Controls|Events")
	FOnControlModeChanged OnControlModeChanged;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|Controls|Events")
	FOnBioRallyHealthRecovered OnBioRallyHealthRecovered;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|Controls|Events")
	FOnSynergyComboExecuted OnSynergyComboExecuted;

	/* --- ON-FOOT ACTIONS --- */
	UFUNCTION(BlueprintCallable, Category = "Wonder|Controls|OnFoot")
	void MoveForward(float Value);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Controls|OnFoot")
	void MoveRight(float Value);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Controls|OnFoot")
	void Turn(float Value);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Controls|OnFoot")
	void LookUp(float Value);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Controls|OnFoot")
	void StartSprint();

	UFUNCTION(BlueprintCallable, Category = "Wonder|Controls|OnFoot")
	void StopSprint();

	UFUNCTION(BlueprintCallable, Category = "Wonder|Controls|OnFoot")
	void HandleJump();

	UFUNCTION(BlueprintCallable, Category = "Wonder|Controls|OnFoot")
	void StopJump();

	UFUNCTION(BlueprintCallable, Category = "Wonder|Controls|OnFoot")
	void HandleDodgeRoll();

	UFUNCTION(BlueprintCallable, Category = "Wonder|Controls|OnFoot")
	void HandleComboStrike();

	UFUNCTION(BlueprintCallable, Category = "Wonder|Controls|OnFoot")
	void HandleResonanceParry();

	UFUNCTION(BlueprintCallable, Category = "Wonder|Controls|OnFoot")
	void StartCymaticFuse();

	UFUNCTION(BlueprintCallable, Category = "Wonder|Controls|OnFoot")
	void StopCymaticFuse();

	UFUNCTION(BlueprintCallable, Category = "Wonder|Controls|OnFoot")
	void HandleSubBassAscend();

	UFUNCTION(BlueprintCallable, Category = "Wonder|Controls|OnFoot")
	void HandleBulletTime();

	UFUNCTION(BlueprintCallable, Category = "Wonder|Controls|OnFoot")
	void HandleSonicDiscRecall();

	UFUNCTION(BlueprintCallable, Category = "Wonder|Controls|OnFoot")
	void HandleVehicleHijack();

	UFUNCTION(BlueprintCallable, Category = "Wonder|Controls|HUD")
	void ToggleLedgerHUD();

	/* --- IN-VEHICLE ACTIONS --- */
	UFUNCTION(BlueprintCallable, Category = "Wonder|Controls|InVehicle")
	void VehicleSteer(float Value);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Controls|InVehicle")
	void VehicleThrottle(float Value);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Controls|InVehicle")
	void VehicleBrake(float Value);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Controls|InVehicle")
	void VehicleToggleHover();

	UFUNCTION(BlueprintCallable, Category = "Wonder|Controls|InVehicle")
	void VehicleAscend();

	UFUNCTION(BlueprintCallable, Category = "Wonder|Controls|InVehicle")
	void VehicleDescend();

	UFUNCTION(BlueprintCallable, Category = "Wonder|Controls|InVehicle")
	void VehicleAfterburner();

	UFUNCTION(BlueprintCallable, Category = "Wonder|Controls|InVehicle")
	void VehicleEjectionVault();

	UFUNCTION(BlueprintCallable, Category = "Wonder|Controls|InVehicle")
	void VehicleHandbrake();

	/* --- COMBAT & DAMAGE NOTIFICATION FOR BIO-RALLY --- */
	UFUNCTION(BlueprintCallable, Category = "Wonder|Controls|Combat")
	void NotifyDamageTaken(float DamageAmount);

	/* --- COMBO SYNERGY STATE --- */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wonder|Combos")
	int32 ComboStrikeIndex = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wonder|Combos")
	bool bParryWindowActive = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wonder|Combos")
	float LastParryTime = -10.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wonder|Combos")
	bool bSlingshotEjectActive = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wonder|Combos")
	bool bCymaticFuseHolding = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wonder|Combos")
	AActor* TetheredPropActor = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wonder|Combos")
	float BioRallyRecoverableHealth = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wonder|Combos")
	float BioRallyWindowRemaining = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wonder|Combos")
	bool bBulletTimeActive = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wonder|HUD")
	bool bLedgerHUDVisible = false;

private:
	UPROPERTY()
	ACharacter* CharacterOwner = nullptr;

	UPROPERTY()
	APawn* PawnOwner = nullptr;

	float DefaultWalkSpeed = 600.0f;
	float SprintSpeed = 1150.0f;
	float LastTapAfterburnerTime = -10.0f;
};
