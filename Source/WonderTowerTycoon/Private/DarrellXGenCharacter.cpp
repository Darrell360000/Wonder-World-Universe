// DarrellXGenCharacter.cpp - Unreal Engine 5.8
#include "DarrellXGenCharacter.h"
#include "WonderMasterControlComponent.h"
#include "WonderVehicleInteractComponent.h"
#include "WonderPhoneComponent.h"
#include "WonderAdvancedCombatComponent.h"
#include "WonderTophatComponent.h"
#include "WonderGadgetArsenalComponent.h"
#include "WonderWardrobeComponent.h"
#include "WonderCameraDirectorComponent.h"
#include "WonderSagaDirectorComponent.h"
#include "WonderLandAuctionComponent.h"
#include "WonderFreeflowCombatComponent.h"
#include "WonderShieldSawComponent.h"
#include "WonderStratagemComponent.h"
#include "WonderAntiGravityLabComponent.h"
#include "WonderMermaidMagicComponent.h"
#include "WonderLedgerSubsystem.h"
#include "Engine/Engine.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"

ADarrellXGenCharacter::ADarrellXGenCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // Enable fluid 360-degree third-person locomotion
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        MoveComp->bConstrainToPlane = false;
        MoveComp->bOrientRotationToMovement = true;
        MoveComp->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
        MoveComp->JumpZVelocity = 650.0f;
        MoveComp->AirControl = 0.35f;
        MoveComp->MaxWalkSpeed = 600.0f;
    }

    // Attach Master Control Component
    MasterControlComponent = CreateDefaultSubobject<UWonderMasterControlComponent>(TEXT("MasterControlComponent"));
    if (MasterControlComponent)
    {
        MasterControlComponent->ControlMode = EWonderControlMode::OnFoot;
    }

    // Attach Vehicle Interact Component
    VehicleInteractComponent = CreateDefaultSubobject<UWonderVehicleInteractComponent>(TEXT("VehicleInteractComponent"));

    // Attach Wonder Phone Component
    PhoneComponent = CreateDefaultSubobject<UWonderPhoneComponent>(TEXT("PhoneComponent"));

    // Attach Advanced Combat Component (Sonic Disc Recall, Harmonic Dilation, Fail-Forward)
    AdvancedCombatComponent = CreateDefaultSubobject<UWonderAdvancedCombatComponent>(TEXT("AdvancedCombatComponent"));

    // Attach Modular Tophatter Gear Component
    TophatComponent = CreateDefaultSubobject<UWonderTophatComponent>(TEXT("TophatComponent"));

    // Attach Unified Gadget Arsenal (Gravity Gun, Camo, AR Visor)
    GadgetArsenalComponent = CreateDefaultSubobject<UWonderGadgetArsenalComponent>(TEXT("GadgetArsenalComponent"));

    // Attach Scrap Mechanic 6-Piece Wardrobe & Dressbot Component
    WardrobeComponent = CreateDefaultSubobject<UWonderWardrobeComponent>(TEXT("WardrobeComponent"));

    // Attach Third/First Person Hybrid Camera Director
    CameraDirectorComponent = CreateDefaultSubobject<UWonderCameraDirectorComponent>(TEXT("CameraDirectorComponent"));

    // Attach Master Wonder Saga Director Component
    SagaDirectorComponent = CreateDefaultSubobject<UWonderSagaDirectorComponent>(TEXT("SagaDirectorComponent"));

    // Attach Real-Time Land Auction Bourse Component
    LandAuctionComponent = CreateDefaultSubobject<UWonderLandAuctionComponent>(TEXT("LandAuctionComponent"));

    // Attach Arkham-Style Freeflow Target Snapping Combat Component
    FreeflowCombatComponent = CreateDefaultSubobject<UWonderFreeflowCombatComponent>(TEXT("FreeflowCombatComponent"));

    // Attach God Hand Shield Saw Component (DOOM: The Dark Ages 2025 inspired)
    ShieldSawComponent = CreateDefaultSubobject<UWonderShieldSawComponent>(TEXT("ShieldSawComponent"));

    // Attach Municipal Stratagem Call-In Component (Helldivers 2 inspired)
    StratagemComponent = CreateDefaultSubobject<UWonderStratagemComponent>(TEXT("StratagemComponent"));

    // Attach Anti-Gravity R&D Lab & Singularity Component (2027 Vanguard)
    AntiGravityLabComponent = CreateDefaultSubobject<UWonderAntiGravityLabComponent>(TEXT("AntiGravityLabComponent"));

    // Attach Mermaid Magic & Aquatic Hydro-Sorcery Component (Canon v18.0)
    MermaidMagicComponent = CreateDefaultSubobject<UWonderMermaidMagicComponent>(TEXT("MermaidMagicComponent"));
}

void ADarrellXGenCharacter::BeginPlay()
{
    Super::BeginPlay();
}

void ADarrellXGenCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (PlayerInputComponent)
    {
        // Direct Action Bindings for Jump, Attack, Parry, Vehicle Interact, and Phone
        PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ADarrellXGenCharacter::HandleJump);
        PlayerInputComponent->BindAction("Jump", IE_Released, this, &ADarrellXGenCharacter::HandleStopJump);

        PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &ADarrellXGenCharacter::HandleAttack);
        PlayerInputComponent->BindAction("ComboStrike", IE_Pressed, this, &ADarrellXGenCharacter::HandleAttack);

        PlayerInputComponent->BindAction("Parry", IE_Pressed, this, &ADarrellXGenCharacter::HandleParry);
        PlayerInputComponent->BindAction("ResonanceParry", IE_Pressed, this, &ADarrellXGenCharacter::HandleParry);

        PlayerInputComponent->BindAction("VehicleInteract", IE_Pressed, this, &ADarrellXGenCharacter::HandleVehicleInteract);
        PlayerInputComponent->BindAction("TogglePerspective", IE_Pressed, this, &ADarrellXGenCharacter::HandleToggleCameraPerspective);
        PlayerInputComponent->BindAction("ToggleCamera", IE_Pressed, this, &ADarrellXGenCharacter::HandleToggleCameraPerspective);
        PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ADarrellXGenCharacter::HandleVehicleInteract);
        PlayerInputComponent->BindAction("VehicleHijack", IE_Pressed, this, &ADarrellXGenCharacter::HandleVehicleInteract);

        PlayerInputComponent->BindAction("TogglePhone", IE_Pressed, this, &ADarrellXGenCharacter::TogglePhone);

        PlayerInputComponent->BindAction("SonicDiscRecall", IE_Pressed, this, &ADarrellXGenCharacter::HandleRecallSonicDisc);
        PlayerInputComponent->BindAction("BulletTime", IE_Pressed, this, &ADarrellXGenCharacter::HandleToggleHarmonicDilation);

        PlayerInputComponent->BindAction("HatPrimaryAbility", IE_Pressed, this, &ADarrellXGenCharacter::HandleUsePrimaryHatAbility);
        PlayerInputComponent->BindAction("HatSecondaryAbility", IE_Pressed, this, &ADarrellXGenCharacter::HandleUseSecondaryHatAbility);

        PlayerInputComponent->BindAction("CycleGadget", IE_Pressed, this, &ADarrellXGenCharacter::HandleCycleGadgetMode);
        PlayerInputComponent->BindAction("TriggerGadget", IE_Pressed, this, &ADarrellXGenCharacter::HandleTriggerPrimaryGadget);
        PlayerInputComponent->BindAction("TriggerGadget", IE_Released, this, &ADarrellXGenCharacter::HandleReleasePrimaryGadget);

        // Core 3-Pillar Direct Key Fallbacks (Guarantees immediate responsiveness)
        PlayerInputComponent->BindKey(EKeys::F8, IE_Pressed, this, &ADarrellXGenCharacter::HandleToggleLedger);
        PlayerInputComponent->BindAction("ToggleLedger", IE_Pressed, this, &ADarrellXGenCharacter::HandleToggleLedger);
        PlayerInputComponent->BindAction("ToggleLedgerHUD", IE_Pressed, this, &ADarrellXGenCharacter::HandleToggleLedger);

        PlayerInputComponent->BindKey(EKeys::RightMouseButton, IE_Pressed, this, &ADarrellXGenCharacter::HandleParry);
        PlayerInputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &ADarrellXGenCharacter::HandleAttack);
        PlayerInputComponent->BindKey(EKeys::F, IE_Pressed, this, &ADarrellXGenCharacter::HandleVehicleInteract);

        // Mermaid Magic Co-Op Input Hooks (Canon v18.0)
        PlayerInputComponent->BindKey(EKeys::C, IE_Pressed, this, &ADarrellXGenCharacter::HandleMermaidHydroGeyser);
        PlayerInputComponent->BindKey(EKeys::Q, IE_Pressed, this, &ADarrellXGenCharacter::HandleMermaidMaelstrom);

        // Master Control bindings (Axes & Flow Combos)
        if (MasterControlComponent)
        {
            MasterControlComponent->SetupControlBindings(PlayerInputComponent);
        }
    }
}

void ADarrellXGenCharacter::HandleJump()
{
    if (MasterControlComponent)
    {
        MasterControlComponent->HandleJump();
    }
    else
    {
        Jump();
    }
}

void ADarrellXGenCharacter::HandleStopJump()
{
    if (MasterControlComponent)
    {
        MasterControlComponent->StopJump();
    }
    else
    {
        StopJumping();
    }
}

void ADarrellXGenCharacter::HandleAttack()
{
    if (MasterControlComponent)
    {
        MasterControlComponent->HandleComboStrike();
    }
    CastSonicShockwave(GetActorLocation() + GetActorForwardVector() * 200.0f);
}

void ADarrellXGenCharacter::HandleParry()
{
    if (MasterControlComponent)
    {
        MasterControlComponent->HandleResonanceParry();
    }
}

void ADarrellXGenCharacter::HandleVehicleInteract()
{
    if (VehicleInteractComponent)
    {
        VehicleInteractComponent->TryEnterNearestVehicle(this);
    }
    else if (MasterControlComponent)
    {
        MasterControlComponent->HandleVehicleHijack();
    }
}

void ADarrellXGenCharacter::HandleToggleLedger()
{
    if (MasterControlComponent)
    {
        MasterControlComponent->ToggleLedgerHUD();
    }

    if (UGameInstance* GI = GetGameInstance())
    {
        if (UWonderLedgerSubsystem* Ledger = GI->GetSubsystem<UWonderLedgerSubsystem>())
        {
            float Gross = Ledger->Column1Grind.GetTotalGrossRevenue();
            float Overhead = Ledger->Column2Overhead.GetTotalOperatingExpenses();
            float Net = Ledger->Col3FoundationCapital;

            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Emerald,
                    FString::Printf(TEXT("[3-COLUMN FINANCIAL LEDGER (F8)]\n• COL 1 (The Grind - Gross Revenue): $%.2f (+$%.1f/s)\n• COL 2 (The Overhead - Expenses): $%.2f (-$%.1f/s)\n• COL 3 (The Foundation - Net Capital): $%.2f"),
                        Gross, Ledger->Col1GrindIncomePerSec,
                        Overhead, Ledger->Col2OverheadPerSec,
                        Net));
            }
        }
    }
}

void ADarrellXGenCharacter::HandleToggleCameraPerspective()
{
    if (CameraDirectorComponent)
    {
        CameraDirectorComponent->TogglePerspective();
    }
}

void ADarrellXGenCharacter::TogglePhone()
{
    if (PhoneComponent)
    {
        PhoneComponent->TogglePhone();
    }
}

void ADarrellXGenCharacter::HandleRecallSonicDisc()
{
    if (AdvancedCombatComponent)
    {
        if (AdvancedCombatComponent->bIsDiscThrown)
        {
            AdvancedCombatComponent->RecallSonicDisc();
        }
        else
        {
            AdvancedCombatComponent->ThrowSonicDisc(GetActorForwardVector());
        }
    }
}

void ADarrellXGenCharacter::HandleToggleHarmonicDilation()
{
    if (AdvancedCombatComponent)
    {
        AdvancedCombatComponent->ToggleHarmonicDilation();
    }
}

void ADarrellXGenCharacter::HandleFailForwardRespawn()
{
    if (AdvancedCombatComponent)
    {
        AdvancedCombatComponent->TriggerCinemaFailForwardRespawn();
    }
}

void ADarrellXGenCharacter::HandleUsePrimaryHatAbility()
{
    if (TophatComponent)
    {
        TophatComponent->UsePrimaryHatAbility();
    }
}

void ADarrellXGenCharacter::HandleUseSecondaryHatAbility()
{
    if (TophatComponent)
    {
        TophatComponent->UseSecondaryHatAbility();
    }
}

void ADarrellXGenCharacter::HandleCycleGadgetMode()
{
    if (GadgetArsenalComponent)
    {
        GadgetArsenalComponent->CycleGadgetMode();
    }
}

void ADarrellXGenCharacter::HandleTriggerPrimaryGadget()
{
    if (GadgetArsenalComponent)
    {
        GadgetArsenalComponent->TriggerPrimaryGadget();
    }
}

void ADarrellXGenCharacter::HandleReleasePrimaryGadget()
{
    if (GadgetArsenalComponent)
    {
        GadgetArsenalComponent->ReleasePrimaryGadget();
    }
}

void ADarrellXGenCharacter::CastSonicShockwave(FVector TargetLocation)
{
    RhythmHeat = FMath::Clamp(RhythmHeat + 15.0f, 0.0f, 100.0f);
    OnSoundMagicCast.Broadcast(TEXT("FrequencyShockwave"), RhythmHeat);
}

void ADarrellXGenCharacter::ActivateTempoLock(float DurationSeconds)
{
    RhythmHeat = FMath::Clamp(RhythmHeat + 10.0f, 0.0f, 100.0f);
    OnSoundMagicCast.Broadcast(TEXT("TempoLock"), RhythmHeat);
}

bool ADarrellXGenCharacter::UnleashWonderHatBoomerang()
{
    if (RhythmHeat < 100.0f)
    {
        return false;
    }

    RhythmHeat = 0.0f;
    OnSoundMagicCast.Broadcast(TEXT("WonderHatBoomerang"), RhythmHeat);
    return true;
}

void ADarrellXGenCharacter::HandleShieldSawBlock()
{
    if (ShieldSawComponent)
    {
        ShieldSawComponent->StartActiveBlock();
    }
}

void ADarrellXGenCharacter::HandleShieldSawRelease()
{
    if (ShieldSawComponent)
    {
        ShieldSawComponent->StopActiveBlock();
    }
}

void ADarrellXGenCharacter::HandleShieldSawThrow()
{
    if (ShieldSawComponent)
    {
        FVector AimDir = GetActorForwardVector();
        ShieldSawComponent->ThrowShieldSaw(AimDir);
    }
}

void ADarrellXGenCharacter::HandleStratagemDirectionUp()
{
    if (StratagemComponent)
    {
        FVector AimPoint = GetActorLocation() + GetActorForwardVector() * 1500.0f;
        StratagemComponent->InputDirection(EStratagemInputDirection::Up, AimPoint);
    }
}

void ADarrellXGenCharacter::HandleStratagemDirectionDown()
{
    if (StratagemComponent)
    {
        FVector AimPoint = GetActorLocation() + GetActorForwardVector() * 1500.0f;
        StratagemComponent->InputDirection(EStratagemInputDirection::Down, AimPoint);
    }
}

void ADarrellXGenCharacter::HandleStratagemDirectionLeft()
{
    if (StratagemComponent)
    {
        FVector AimPoint = GetActorLocation() + GetActorForwardVector() * 1500.0f;
        StratagemComponent->InputDirection(EStratagemInputDirection::Left, AimPoint);
    }
}

void ADarrellXGenCharacter::HandleStratagemDirectionRight()
{
    if (StratagemComponent)
    {
        FVector AimPoint = GetActorLocation() + GetActorForwardVector() * 1500.0f;
        StratagemComponent->InputDirection(EStratagemInputDirection::Right, AimPoint);
    }
}

void ADarrellXGenCharacter::HandleTriggerSingularity()
{
    if (AntiGravityLabComponent)
    {
        FVector VortexPoint = GetActorLocation() + GetActorForwardVector() * 800.0f + FVector::UpVector * 200.0f;
        AntiGravityLabComponent->SpawnGravitonSingularity(VortexPoint, 6.0f);
    }
}

void ADarrellXGenCharacter::HandleInvertGravityToWall()
{
    if (AntiGravityLabComponent)
    {
        FVector WallNormal = -GetActorForwardVector();
        AntiGravityLabComponent->ShiftPersonalGravityVector(WallNormal);
    }
}

void ADarrellXGenCharacter::HandleMermaidHydroGeyser()
{
    if (MermaidMagicComponent)
    {
        MermaidMagicComponent->TriggerHydroGeyser();
    }
}

void ADarrellXGenCharacter::HandleMermaidMaelstrom()
{
    if (MermaidMagicComponent)
    {
        MermaidMagicComponent->SummonMaelstromSingularity();
    }
}
