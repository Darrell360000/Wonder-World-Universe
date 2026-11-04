// WonderGrindMiniGameComponent.cpp - Unreal Engine 5.8
#include "WonderGrindMiniGameComponent.h"
#include "WonderLedgerSubsystem.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"

UWonderGrindMiniGameComponent::UWonderGrindMiniGameComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UWonderGrindMiniGameComponent::PumpWetVacSuction(bool bSyncedTo85BPMBeat)
{
    if (bSubterraneanFoundationDry || CurrentWetVacTankGallons >= WetVacTankCapacity)
    {
        return;
    }

    const float BaseSuctionGallons = bSyncedTo85BPMBeat ? 750.0f : 450.0f;
    const float SpaceInTank = WetVacTankCapacity - CurrentWetVacTankGallons;
    const float Extracted = FMath::Min3(BaseSuctionGallons, SpaceInTank, RemainingFloodGallons);

    CurrentWetVacTankGallons += Extracted;
    RemainingFloodGallons -= Extracted;

    if (RemainingFloodGallons <= 0.0f)
    {
        RemainingFloodGallons = 0.0f;
        bSubterraneanFoundationDry = true;
    }

    OnWetVacProgress.Broadcast(RemainingFloodGallons, CurrentWetVacTankGallons, bSubterraneanFoundationDry);
}

void UWonderGrindMiniGameComponent::DumpWetVacTankAndCashOut()
{
    if (CurrentWetVacTankGallons <= 0.0f) return;

    const float CashReward = (CurrentWetVacTankGallons / 100.0f) * 45.0f;
    const int32 VitalityReward = FMath::RoundToInt((CurrentWetVacTankGallons / 100.0f) * 18.0f);
    CurrentWetVacTankGallons = 0.0f;

    if (UWorld* World = GetWorld())
    {
        if (UGameInstance* GI = World->GetGameInstance())
        {
            if (UWonderLedgerSubsystem* Ledger = GI->GetSubsystem<UWonderLedgerSubsystem>())
            {
                Ledger->RecordGrindIncome(CashReward, VitalityReward, TEXT("59,000-Gallon Wet-Vac Extraction"));
                if (bSubterraneanFoundationDry)
                {
                    Ledger->ApplyOverheadRelief(653.91f);
                }
            }
        }
    }

    OnWetVacProgress.Broadcast(RemainingFloodGallons, CurrentWetVacTankGallons, bSubterraneanFoundationDry);
}

bool UWonderGrindMiniGameComponent::ExecuteCNACareTask(const FString& TaskType)
{
    float StaminaCost = 14.0f;
    float EdemaIncrease = 12.0f;

    if (TaskType.Equals(TEXT("HoyerLiftTransfer"), ESearchCase::IgnoreCase))
    {
        StaminaCost = 22.0f;
        EdemaIncrease = 18.0f;
    }
    else if (TaskType.Equals(TEXT("VitalSignsCheck"), ESearchCase::IgnoreCase))
    {
        StaminaCost = 10.0f;
        EdemaIncrease = 8.0f;
    }

    if (PlayerStamina < StaminaCost || LegEdemaPressure >= 85.0f)
    {
        return false;
    }

    PlayerStamina = FMath::Clamp(PlayerStamina - StaminaCost, 0.0f, 100.0f);
    LegEdemaPressure = FMath::Clamp(LegEdemaPressure + EdemaIncrease, 0.0f, 100.0f);
    CompletedPatientTasks++;
    CareQualityMultiplier = FMath::Clamp(CareQualityMultiplier + 0.15f, 1.0f, 3.0f);

    OnCNAShiftProgress.Broadcast(CompletedPatientTasks, PlayerStamina, CareQualityMultiplier);
    return true;
}

void UWonderGrindMiniGameComponent::TakeMamaElevationRestBreak()
{
    PlayerStamina = FMath::Clamp(PlayerStamina + 45.0f, 0.0f, 100.0f);
    LegEdemaPressure = FMath::Clamp(LegEdemaPressure - 50.0f, 0.0f, 100.0f);
    OnCNAShiftProgress.Broadcast(CompletedPatientTasks, PlayerStamina, CareQualityMultiplier);
}

void UWonderGrindMiniGameComponent::CompleteCNAShiftAndDeposit()
{
    if (CompletedPatientTasks <= 0) return;

    const float ShiftCash = CompletedPatientTasks * 320.0f * CareQualityMultiplier;
    const int32 ShiftVitalityCredits = FMath::RoundToInt(CompletedPatientTasks * 110.0f * CareQualityMultiplier);

    CompletedPatientTasks = 0;
    CareQualityMultiplier = 1.0f;

    if (UWorld* World = GetWorld())
    {
        if (UGameInstance* GI = World->GetGameInstance())
        {
            if (UWonderLedgerSubsystem* Ledger = GI->GetSubsystem<UWonderLedgerSubsystem>())
            {
                Ledger->RecordGrindIncome(ShiftCash, ShiftVitalityCredits, TEXT("CNA CareShift & Patient Advocacy"));
            }
        }
    }
}

bool UWonderGrindMiniGameComponent::BalanceElectricalPhaseOnBeat(float PhaseTimingAccuracy)
{
    if (PhaseTimingAccuracy >= 0.80f)
    {
        if (UWorld* World = GetWorld())
        {
            if (UGameInstance* GI = World->GetGameInstance())
            {
                if (UWonderLedgerSubsystem* Ledger = GI->GetSubsystem<UWonderLedgerSubsystem>())
                {
                    Ledger->ApplyOverheadRelief(450.0f);
                    Ledger->RecordGrindIncome(600.0f, 250, TEXT("85 BPM Electrical Grid Overcharge"));
                }
            }
        }
        return true;
    }
    return false;
}
