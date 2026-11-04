// SyndicateOverlordBoss.cpp - Unreal Engine 5.8
#include "SyndicateOverlordBoss.h"
#include "WonderLedgerSubsystem.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"

ASyndicateOverlordBoss::ASyndicateOverlordBoss()
{
    PrimaryActorTick.bCanEverTick = false;
}

bool ASyndicateOverlordBoss::ApplyDarrellSonicShockwave(float ShockwavePower)
{
    if (CurrentPhase != ESyndicateBossPhase::Phase1_SonicBarrierBreach)
    {
        return false;
    }

    SonicBarrierIntegrity = FMath::Max(0.0f, SonicBarrierIntegrity - FMath::Max(0.0f, ShockwavePower));
    if (SonicBarrierIntegrity <= 0.0f)
    {
        CurrentPhase = ESyndicateBossPhase::Phase2_LaserGridSabotage;
        OnBossPhaseChanged.Broadcast(
            CurrentPhase,
            TEXT("Sonic Barrier shattered! Hot-swap to Vesper Vance or Spider-Pulse (Slot 2 / 5B) to sabotage the Laser Grid!")
        );
        return true;
    }
    return false;
}

bool ASyndicateOverlordBoss::ExecuteVesperLaserGridSabotage()
{
    if (CurrentPhase != ESyndicateBossPhase::Phase2_LaserGridSabotage)
    {
        return false;
    }

    bLaserGridActive = false;
    CurrentPhase = ESyndicateBossPhase::Phase3_ArmoredJuggernaut;
    OnBossPhaseChanged.Broadcast(
        CurrentPhase,
        TEXT("Laser Grid disabled! Hot-swap to Jaxson 'Iron Riff' Thorne (Slot 4) to shatter the Armored Juggernaut Mech!")
    );
    return true;
}

bool ASyndicateOverlordBoss::ApplyJaxsonBassCannonSmash(float SubBassDamage)
{
    if (CurrentPhase != ESyndicateBossPhase::Phase3_ArmoredJuggernaut)
    {
        return false;
    }

    MechArmorHealth = FMath::Max(0.0f, MechArmorHealth - FMath::Max(0.0f, SubBassDamage));
    if (MechArmorHealth <= 0.0f)
    {
        CurrentPhase = ESyndicateBossPhase::Defeated;
        OnBossPhaseChanged.Broadcast(
            CurrentPhase,
            TEXT("SYNDICATE OVERLORD DEFEATED! Multiverse Portal Key unlocked and +$14,500 Brawler Payout deposited into Column 1!")
        );

        if (UWorld* World = GetWorld())
        {
            if (UGameInstance* GI = World->GetGameInstance())
            {
                if (UWonderLedgerSubsystem* Ledger = GI->GetSubsystem<UWonderLedgerSubsystem>())
                {
                    Ledger->RecordGrindIncome(14500.0f, 1500, TEXT("Syndicate Overlord 3-Phase Boss Victory"));
                }
            }
        }
        return true;
    }
    return false;
}
