// WonderGrindMiniGameComponent.h - Unreal Engine 5.8
// Mama's Wonder World: Era 1 "Concrete Grind" Mini-Games:
// 1) 59,000-Gallon Wet-Vac Flood Extraction Challenge
// 2) CNA CareShift & Stamina Pacing Mini-Game (Hoyer Lift, Vital Signs, Mama Rest Elevation)
// 3) 85 BPM Electrical Grid Load Balancer

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WonderGrindMiniGameComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnWetVacProgress, float, GallonsRemaining, float, CurrentTankGallons, bool, bFoundationCleared);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCNAShiftProgress, int32, PatientsCaredFor, float, CurrentStamina, float, ShiftComboMultiplier);

UCLASS(ClassGroup=(WonderWorld), meta=(BlueprintSpawnableComponent))
class WONDERTOWERTYCOON_API UWonderGrindMiniGameComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UWonderGrindMiniGameComponent();

    // MINI-GAME 1: 59,000-GALLON WET-VAC FLOOD EXTRACTION
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grind Mini-Games|Wet-Vac")
    float TotalFloodGallonsToClear = 59000.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grind Mini-Games|Wet-Vac")
    float RemainingFloodGallons = 59000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grind Mini-Games|Wet-Vac")
    float WetVacTankCapacity = 2500.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grind Mini-Games|Wet-Vac")
    float CurrentWetVacTankGallons = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grind Mini-Games|Wet-Vac")
    bool bSubterraneanFoundationDry = false;

    UPROPERTY(BlueprintAssignable, Category = "Grind Mini-Games|Wet-Vac")
    FOnWetVacProgress OnWetVacProgress;

    UFUNCTION(BlueprintCallable, Category = "Grind Mini-Games|Wet-Vac")
    void PumpWetVacSuction(bool bSyncedTo85BPMBeat);

    UFUNCTION(BlueprintCallable, Category = "Grind Mini-Games|Wet-Vac")
    void DumpWetVacTankAndCashOut();

    // MINI-GAME 2: CNA SHIFT, HOYER LIFT & MAMA STAMINA PACING
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grind Mini-Games|CNA Shift")
    float PlayerStamina = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grind Mini-Games|CNA Shift")
    float LegEdemaPressure = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grind Mini-Games|CNA Shift")
    int32 CompletedPatientTasks = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grind Mini-Games|CNA Shift")
    float CareQualityMultiplier = 1.0f;

    UPROPERTY(BlueprintAssignable, Category = "Grind Mini-Games|CNA Shift")
    FOnCNAShiftProgress OnCNAShiftProgress;

    UFUNCTION(BlueprintCallable, Category = "Grind Mini-Games|CNA Shift")
    bool ExecuteCNACareTask(const FString& TaskType);

    UFUNCTION(BlueprintCallable, Category = "Grind Mini-Games|CNA Shift")
    void TakeMamaElevationRestBreak();

    UFUNCTION(BlueprintCallable, Category = "Grind Mini-Games|CNA Shift")
    void CompleteCNAShiftAndDeposit();

    // MINI-GAME 3: 85 BPM ELECTRICAL GRID BALANCER
    UFUNCTION(BlueprintCallable, Category = "Grind Mini-Games|Electrical Grid")
    bool BalanceElectricalPhaseOnBeat(float PhaseTimingAccuracy);
};
