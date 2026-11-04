// DarrellXGenCharacter.h - Unreal Engine 5.8
// Slot 1 Playable Top-Hatter Hero: Darrell X-Gen ("The Master of Sound Magic" / "The Sonic Tactician")
// Accompanied by Minime Tophatter on clavicle_l, wielding the Equalizer X-Hat and Conductor's Baton Wand.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DarrellXGenCharacter.generated.h"

class UWonderMasterControlComponent;
class UWonderVehicleInteractComponent;
class UWonderPhoneComponent;
class UWonderAdvancedCombatComponent;
class UWonderTophatComponent;
class UWonderGadgetArsenalComponent;
class UWonderWardrobeComponent;
class UWonderCameraDirectorComponent;
class UWonderSagaDirectorComponent;
class UWonderLandAuctionComponent;
class UWonderFreeflowCombatComponent;
class UWonderShieldSawComponent;
class UWonderStratagemComponent;
class UWonderAntiGravityLabComponent;
class UWonderMermaidMagicComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSoundMagicCast, FString, AbilityName, float, NewRhythmHeat);

UCLASS()
class WONDERTOWERTYCOON_API ADarrellXGenCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ADarrellXGenCharacter();

protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controls")
    TObjectPtr<UWonderMasterControlComponent> MasterControlComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controls")
    TObjectPtr<UWonderVehicleInteractComponent> VehicleInteractComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controls")
    TObjectPtr<UWonderPhoneComponent> PhoneComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controls")
    TObjectPtr<UWonderAdvancedCombatComponent> AdvancedCombatComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tophat")
    UWonderTophatComponent* TophatComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gadgets")
    UWonderGadgetArsenalComponent* GadgetArsenalComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wardrobe")
    UWonderWardrobeComponent* WardrobeComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    UWonderCameraDirectorComponent* CameraDirectorComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Saga")
    UWonderSagaDirectorComponent* SagaDirectorComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Auction")
    UWonderLandAuctionComponent* LandAuctionComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    UWonderFreeflowCombatComponent* FreeflowCombatComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|ShieldSaw")
    UWonderShieldSawComponent* ShieldSawComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stratagem")
    UWonderStratagemComponent* StratagemComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AntiGravity")
    UWonderAntiGravityLabComponent* AntiGravityLabComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MermaidMagic")
    UWonderMermaidMagicComponent* MermaidMagicComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Magic")
    float RhythmHeat = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Magic")
    float TargetBPM = 85.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Magic")
    float GodHandParryWindowSeconds = 0.35f;

    UPROPERTY(BlueprintAssignable, Category = "Sound Magic|Events")
    FOnSoundMagicCast OnSoundMagicCast;

    /* --- DIRECT INPUT HANDLERS --- */
    UFUNCTION(BlueprintCallable, Category = "Controls")
    void HandleJump();

    UFUNCTION(BlueprintCallable, Category = "Controls")
    void HandleStopJump();

    UFUNCTION(BlueprintCallable, Category = "Controls")
    void HandleAttack();

    UFUNCTION(BlueprintCallable, Category = "Controls")
    void HandleParry();

    UFUNCTION(BlueprintCallable, Category = "Controls")
    void HandleVehicleInteract();

    UFUNCTION(BlueprintCallable, Category = "Controls")
    void HandleToggleLedger();

    UFUNCTION(BlueprintCallable, Category = "Camera")
    void HandleToggleCameraPerspective();

    /* --- SHIELD SAW COMBAT HANDLERS --- */
    UFUNCTION(BlueprintCallable, Category = "Combat|ShieldSaw")
    void HandleShieldSawBlock();

    UFUNCTION(BlueprintCallable, Category = "Combat|ShieldSaw")
    void HandleShieldSawRelease();

    UFUNCTION(BlueprintCallable, Category = "Combat|ShieldSaw")
    void HandleShieldSawThrow();

    /* --- MUNICIPAL STRATAGEM D-PAD HANDLERS --- */
    UFUNCTION(BlueprintCallable, Category = "Stratagem")
    void HandleStratagemDirectionUp();

    UFUNCTION(BlueprintCallable, Category = "Stratagem")
    void HandleStratagemDirectionDown();

    UFUNCTION(BlueprintCallable, Category = "Stratagem")
    void HandleStratagemDirectionLeft();

    UFUNCTION(BlueprintCallable, Category = "Stratagem")
    void HandleStratagemDirectionRight();

    /* --- ANTI-GRAVITY & SINGULARITY HOOKS --- */
    UFUNCTION(BlueprintCallable, Category = "AntiGravity")
    void HandleTriggerSingularity();

    UFUNCTION(BlueprintCallable, Category = "AntiGravity")
    void HandleInvertGravityToWall();

    /* --- CO-OP MERMAID MAGIC HOOKS --- */
    UFUNCTION(BlueprintCallable, Category = "MermaidMagic")
    void HandleMermaidHydroGeyser();

    UFUNCTION(BlueprintCallable, Category = "MermaidMagic")
    void HandleMermaidMaelstrom();

    UFUNCTION(BlueprintCallable, Category = "Controls")
    void TogglePhone();

    UFUNCTION(BlueprintCallable, Category = "Controls")
    void HandleRecallSonicDisc();

    UFUNCTION(BlueprintCallable, Category = "Controls")
    void HandleToggleHarmonicDilation();

    UFUNCTION(BlueprintCallable, Category = "Controls")
    void HandleFailForwardRespawn();

    UFUNCTION(BlueprintCallable, Category = "Controls")
    void HandleUsePrimaryHatAbility();

    UFUNCTION(BlueprintCallable, Category = "Controls")
    void HandleUseSecondaryHatAbility();

    UFUNCTION(BlueprintCallable, Category = "Gadgets")
    void HandleCycleGadgetMode();

    UFUNCTION(BlueprintCallable, Category = "Gadgets")
    void HandleTriggerPrimaryGadget();

    UFUNCTION(BlueprintCallable, Category = "Gadgets")
    void HandleReleasePrimaryGadget();

    // Casts Frequency Shockwave (+15.0f Rhythm Heat per beat hit), shattering Phase 1 Syndicate Overlord sonic barriers
    UFUNCTION(BlueprintCallable, Category = "Sound Magic|Combat")
    void CastSonicShockwave(FVector TargetLocation);

    // Slows time (Matrix-style) around foes on the 85 BPM beat
    UFUNCTION(BlueprintCallable, Category = "Sound Magic|Combat")
    void ActivateTempoLock(float DurationSeconds = 3.0f);

    // Ultimate 360-degree screen-clearing audio finisher at 100.0f Rhythm Heat
    UFUNCTION(BlueprintCallable, Category = "Sound Magic|Combat")
    bool UnleashWonderHatBoomerang();
};
