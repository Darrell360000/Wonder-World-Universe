// WonderLedgerSubsystem.cpp - Unreal Engine 5.8
#include "WonderLedgerSubsystem.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

void UWonderLedgerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    AuditAndBalanceLedger();
}

void UWonderLedgerSubsystem::RegisterCoronationHit(EWonderSpecialistRole Role, bool bGoldenMultiplier)
{
    HatsGained++;
    const double Mult = bGoldenMultiplier ? 2.0 : 1.0;
    switch (Role)
    {
        case EWonderSpecialistRole::Hero: Col1GrindIncomePerSec += 50.0 * Mult; break;
        case EWonderSpecialistRole::Builder: Col1GrindIncomePerSec += 40.0 * Mult; break;
        case EWonderSpecialistRole::CNA: Col1GrindIncomePerSec += 35.0 * Mult; break;
        case EWonderSpecialistRole::DJ: Col1GrindIncomePerSec += 45.0 * Mult; break;
    }
    if (HatsGained % 6 == 0)
    {
        Col3FoundationCapital += 250.0; // Mini-Me Reward Chest
    }
}

void UWonderLedgerSubsystem::RegisterCoronationMiss()
{
    HatsMissed++;
}

void UWonderLedgerSubsystem::TickLedger(float DeltaTime)
{
    const double NetRate = Col1GrindIncomePerSec - Col2OverheadPerSec;
    Col3FoundationCapital = FMath::Max(0.0, Col3FoundationCapital + (NetRate * DeltaTime));
}

void UWonderLedgerSubsystem::RecordGrindIncome(float CashEarned, int32 VitalityCreditsEarned, const FString& SourceLabel)
{
    Column1Grind.CNAAndWetVacMiniGames += FMath::Max(0.0f, CashEarned);
    Column3Foundation.VitalityCreditsTier1 += FMath::Max(0, VitalityCreditsEarned);

    AuditAndBalanceLedger();
    SaveSharedLedgerJson();

    FString MamaMsg = FString::Printf(TEXT("Baby, that %s hustle brought in $%.0f and %d Vitality Credits! Just make sure you rest your legs."), *SourceLabel, CashEarned, VitalityCreditsEarned);
    FString PapaMsg = FString::Printf(TEXT("That's what I'm talking about! Column 1 is climbing and we're movin' on up to Era %d!"), static_cast<int32>(CurrentEra) + 1);
    OnMamaPapaVoiceAdvisory.Broadcast(MamaMsg, PapaMsg);
}

void UWonderLedgerSubsystem::ApplyOverheadRelief(float UtilityReductionAmount)
{
    Column2Overhead.UtilityGridLoadCost = FMath::Max(500.0f, Column2Overhead.UtilityGridLoadCost - UtilityReductionAmount);
    AuditAndBalanceLedger();
    SaveSharedLedgerJson();
}

bool UWonderLedgerSubsystem::AuditAndBalanceLedger()
{
    const float Gross = Column1Grind.GetTotalGrossRevenue();
    const float Overhead = Column2Overhead.GetTotalOperatingExpenses();
    const float NetSurplus = Gross - Overhead;

    Column3Foundation.LiquidReserve = NetSurplus;
    bEvonHubPermissionGranted = (NetSurplus >= 0.0f);
    Column3Foundation.bEmissiveSignageActive = bEvonHubPermissionGranted;

    if (Column3Foundation.IndustrialBondsTier2And3 >= 500)
        CurrentEra = EWonderEra::Era5_PurpleCity;
    else if (Column3Foundation.IndustrialBondsTier2And3 >= 300)
        CurrentEra = EWonderEra::Era4_LunarAnchor;
    else if (Column3Foundation.IndustrialBondsTier2And3 >= 180)
        CurrentEra = EWonderEra::Era3_OrbitalRing;
    else if (Column3Foundation.IndustrialBondsTier2And3 >= 100)
        CurrentEra = EWonderEra::Era2_CloudGrid;
    else
        CurrentEra = EWonderEra::Era1_ConcreteGrind;

    OnLedgerUpdated.Broadcast(Gross, Overhead, Column3Foundation.LiquidReserve);
    return bEvonHubPermissionGranted;
}

bool UWonderLedgerSubsystem::ConvertSurplusToIndustrialBonds(float ReserveAmountToConvert)
{
    if (!bEvonHubPermissionGranted || Column3Foundation.LiquidReserve < ReserveAmountToConvert || ReserveAmountToConvert < 1000.0f)
    {
        OnMamaPapaVoiceAdvisory.Broadcast(
            TEXT("Hold on, sugar! We have to cover Column 2 Overhead bills before we issue more tower bonds."),
            TEXT("Mama's right! Balance Column 2 first so the Evon Hub gives us PERMISSION GRANTED!")
        );
        return false;
    }

    const int32 BondsMinted = FMath::FloorToInt(ReserveAmountToConvert / 1000.0f);
    Column3Foundation.LiquidReserve -= (BondsMinted * 1000.0f);
    Column3Foundation.IndustrialBondsTier2And3 += BondsMinted;
    Column3Foundation.UnlockedTowerTier = FMath::Clamp(1 + (Column3Foundation.IndustrialBondsTier2And3 / 35), 1, 10);

    AuditAndBalanceLedger();
    SaveSharedLedgerJson();
    return true;
}

FString UWonderLedgerSubsystem::GetSharedLedgerFilePath() const
{
    return FPaths::Combine(FPaths::ProjectDir(), TEXT("WonderPipeline"), TEXT("02_Shared_Game_Core"), TEXT("State"), TEXT("WonderLedgerState.json"));
}

bool UWonderLedgerSubsystem::SaveSharedLedgerJson()
{
    FString JsonStr = FString::Printf(
        TEXT("{\n")
        TEXT("  \"schema_version\": \"3.0.0\",\n")
        TEXT("  \"tower_name\": \"Mama Wonder Tower\",\n")
        TEXT("  \"current_era\": %d,\n")
        TEXT("  \"hardware_profile\": \"RTX_5060_32GB_RAM\",\n")
        TEXT("  \"sound_magic_bpm\": 85,\n")
        TEXT("  \"evon_hub_permission_granted\": %s,\n")
        TEXT("  \"ledger_columns\": {\n")
        TEXT("    \"column_1_gross_revenue\": {\n")
        TEXT("      \"cna_and_wetvac_minigames\": %.2f,\n")
        TEXT("      \"arcade_brawler_payouts\": %.2f,\n")
        TEXT("      \"commercial_lease_income\": %.2f,\n")
        TEXT("      \"cities_skylines_tourism_bonus\": %.2f\n")
        TEXT("    },\n")
        TEXT("    \"column_2_operating_expenses\": {\n")
        TEXT("      \"structural_maintenance\": %.2f,\n")
        TEXT("      \"security_and_brawler_staff\": %.2f,\n")
        TEXT("      \"utility_grid_load_cost\": %.2f\n")
        TEXT("    },\n")
        TEXT("    \"column_3_net_reserve_and_upgrades\": {\n")
        TEXT("      \"liquid_reserve\": %.2f,\n")
        TEXT("      \"vitality_credits_tier1\": %d,\n")
        TEXT("      \"industrial_bonds_tier2_3\": %d,\n")
        TEXT("      \"unlocked_tier\": %d,\n")
        TEXT("      \"cities_skylines_lot_size\": \"%s\",\n")
        TEXT("      \"emissive_signage_active\": %s\n")
        TEXT("    }\n")
        TEXT("  }\n")
        TEXT("}\n"),
        static_cast<int32>(CurrentEra) + 1,
        bEvonHubPermissionGranted ? TEXT("true") : TEXT("false"),
        Column1Grind.CNAAndWetVacMiniGames,
        Column1Grind.ArcadeBrawlerPayouts,
        Column1Grind.CommercialLeaseIncome,
        Column1Grind.CitiesSkylinesTourismBonus,
        Column2Overhead.StructuralMaintenance,
        Column2Overhead.SecurityAndBrawlerStaff,
        Column2Overhead.UtilityGridLoadCost,
        Column3Foundation.LiquidReserve,
        Column3Foundation.VitalityCreditsTier1,
        Column3Foundation.IndustrialBondsTier2And3,
        Column3Foundation.UnlockedTowerTier,
        *Column3Foundation.CitiesSkylinesLotSize,
        Column3Foundation.bEmissiveSignageActive ? TEXT("true") : TEXT("false")
    );

    return FFileHelper::SaveStringToFile(JsonStr, *GetSharedLedgerFilePath());
}
