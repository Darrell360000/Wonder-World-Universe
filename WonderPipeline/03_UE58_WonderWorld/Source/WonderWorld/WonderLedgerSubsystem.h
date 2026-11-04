// WonderLedgerSubsystem.h - Unreal Engine 5.8
// Mama's Wonder World: Tower Tycoon & Industrial Brawler
// Manages the 3-Column Financial Ledger, Evon Hub Clearance, Specialist Coronation Wages, Mini-Me Referee, and 5 Generational Eras

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WonderLedgerSubsystem.generated.h"

UENUM(BlueprintType)
enum class EWonderSpecialistRole : uint8
{
    Hero UMETA(DisplayName = "Speedster Hero (+50/s)"),
    CNA UMETA(DisplayName = "CNA Caregiver (+35/s)"),
    Builder UMETA(DisplayName = "Construction Builder (+40/s)"),
    DJ UMETA(DisplayName = "Sound Magic DJ (+45/s)")
};

UENUM(BlueprintType)
enum class EWonderEra : uint8
{
    Era1_ConcreteGrind UMETA(DisplayName = "Era 1: Concrete Grind"),
    Era2_CloudGrid     UMETA(DisplayName = "Era 2: Cloud Grid"),
    Era3_OrbitalRing   UMETA(DisplayName = "Era 3: Orbital Ring"),
    Era4_LunarAnchor   UMETA(DisplayName = "Era 4: Lunar Anchor"),
    Era5_PurpleCity    UMETA(DisplayName = "Era 5: Purple City")
};

// COLUMN 1: THE GRIND (Gross Revenue & Street Hustle)
USTRUCT(BlueprintType)
struct WONDERTOWERTYCOON_API FColumn1TheGrind
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Column 1: The Grind")
    float CNAAndWetVacMiniGames = 8500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Column 1: The Grind")
    float ArcadeBrawlerPayouts = 14500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Column 1: The Grind")
    float CommercialLeaseIncome = 48000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Column 1: The Grind")
    float CitiesSkylinesTourismBonus = 9250.0f;

    float GetTotalGrossRevenue() const
    {
        return CNAAndWetVacMiniGames + ArcadeBrawlerPayouts + CommercialLeaseIncome + CitiesSkylinesTourismBonus;
    }
};

// COLUMN 2: THE OVERHEAD (Operating Expenses & Survival Gate)
USTRUCT(BlueprintType)
struct WONDERTOWERTYCOON_API FColumn2TheOverhead
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Column 2: The Overhead")
    float RentAndHouseholdSurvival = 2800.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Column 2: The Overhead")
    float HealthcareAndStaminaCare = 1200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Column 2: The Overhead")
    float StructuralMaintenance = 11200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Column 2: The Overhead")
    float SecurityAndBrawlerStaff = 8400.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Column 2: The Overhead")
    float UtilityGridLoadCost = 5100.0f;

    float GetTotalOperatingExpenses() const
    {
        return RentAndHouseholdSurvival + HealthcareAndStaminaCare + StructuralMaintenance + SecurityAndBrawlerStaff + UtilityGridLoadCost;
    }
};

// COLUMN 3: THE FOUNDATION (Net Reserve, Vitality Credits & Industrial Bonds)
USTRUCT(BlueprintType)
struct WONDERTOWERTYCOON_API FColumn3TheFoundation
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Column 3: The Foundation")
    float LiquidReserve = 51550.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Column 3: The Foundation")
    int32 VitalityCreditsTier1 = 12400;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Column 3: The Foundation")
    int32 IndustrialBondsTier2And3 = 85;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Column 3: The Foundation")
    int32 UnlockedTowerTier = 3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Column 3: The Foundation")
    FString CitiesSkylinesLotSize = TEXT("8m x 8m");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Column 3: The Foundation")
    bool bEmissiveSignageActive = true;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnWonderLedgerUpdated, float, GrossRevenue, float, TotalOverhead, float, NetReserve);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMamaPapaVoiceAdvisory, FString, MamaLine, FString, PapaLine);

UCLASS(BlueprintType)
class WONDERTOWERTYCOON_API UWonderLedgerSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // 5-Era Macro Ledger Structs
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder Ledger")
    FColumn1TheGrind Column1Grind;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder Ledger")
    FColumn2TheOverhead Column2Overhead;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder Ledger")
    FColumn3TheFoundation Column3Foundation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder Ledger|Progression")
    EWonderEra CurrentEra = EWonderEra::Era1_ConcreteGrind;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder Ledger|Evon Hub")
    bool bEvonHubPermissionGranted = true;

    // Real-Time Per-Second Street Coronation & Mini-Me Referee Fields
    UPROPERTY(BlueprintReadOnly, Category = "3-Column Ledger")
    double Col1GrindIncomePerSec = 0.0;

    UPROPERTY(BlueprintReadOnly, Category = "3-Column Ledger")
    double Col2OverheadPerSec = 5.0;

    UPROPERTY(BlueprintReadOnly, Category = "3-Column Ledger")
    double Col3FoundationCapital = 150.0;

    UPROPERTY(BlueprintReadOnly, Category = "Mini-Me Referee")
    int32 HatsGained = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Mini-Me Referee")
    int32 HatsMissed = 0;

    UPROPERTY(BlueprintAssignable, Category = "Wonder Ledger|Events")
    FOnWonderLedgerUpdated OnLedgerUpdated;

    UPROPERTY(BlueprintAssignable, Category = "Wonder Ledger|Events")
    FOnMamaPapaVoiceAdvisory OnMamaPapaVoiceAdvisory;

    // Street Coronation & Per-Second Tick Actions
    UFUNCTION(BlueprintCallable, Category = "Wonder Coronation")
    void RegisterCoronationHit(EWonderSpecialistRole Role, bool bGoldenMultiplier);

    UFUNCTION(BlueprintCallable, Category = "Wonder Coronation")
    void RegisterCoronationMiss();

    UFUNCTION(BlueprintCallable, Category = "3-Column Ledger")
    void TickLedger(float DeltaTime);

    // Macro Era 1-5 & Shared JSON Actions
    UFUNCTION(BlueprintCallable, Category = "Wonder Ledger|Actions")
    void RecordGrindIncome(float CashEarned, int32 VitalityCreditsEarned, const FString& SourceLabel);

    UFUNCTION(BlueprintCallable, Category = "Wonder Ledger|Actions")
    void ApplyOverheadRelief(float UtilityReductionAmount);

    UFUNCTION(BlueprintCallable, Category = "Wonder Ledger|Actions")
    bool AuditAndBalanceLedger();

    UFUNCTION(BlueprintCallable, Category = "Wonder Ledger|Actions")
    bool ConvertSurplusToIndustrialBonds(float ReserveAmountToConvert);

    UFUNCTION(BlueprintCallable, Category = "Wonder Ledger|Sync")
    bool SaveSharedLedgerJson();

    UFUNCTION(BlueprintPure, Category = "Wonder Ledger|Sync")
    FString GetSharedLedgerFilePath() const;
};
