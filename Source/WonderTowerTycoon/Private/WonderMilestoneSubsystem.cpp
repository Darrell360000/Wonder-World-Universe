#include "WonderMilestoneSubsystem.h"
#include "SyndicateOverlordBoss.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

void UWonderMilestoneSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	bOneMillionCashUnlocked = false;
	bOneMillionNetPowerUnlocked = false;
	CollectedNotesCount = 0;
	MaxNotesCount = 1000;
	UnlockedWeaponWheelSlots = 4;
	UnlockedGarageBays = 2;
	MunicipalRebirthCount = 0;
	RebirthRevenueMultiplier = 1.0f;
}

bool UWonderMilestoneSubsystem::CheckCashMilestone(int64 CurrentCash)
{
	if (CurrentCash >= 1000000 && !bOneMillionCashUnlocked)
	{
		bOneMillionCashUnlocked = true;
		OnMilestoneAchieved.Broadcast(
			TEXT("OneMillionCash"),
			TEXT("Gold Penthouse Suite Unlocked atop Mama & Papa Towers! Commercial supply lines fully automated across all 5 districts.")
		);
		return true;
	}
	return false;
}

bool UWonderMilestoneSubsystem::CheckNetPowerMilestone(float CurrentPowerMW)
{
	if (CurrentPowerMW >= 1000000.0f && !bOneMillionNetPowerUnlocked)
	{
		bOneMillionNetPowerUnlocked = true;
		OnMilestoneAchieved.Broadcast(
			TEXT("OneMillionNetPower"),
			TEXT("Vehicle Altitude Ceiling Permanently Removed! Stratosphere flight and orbital sky platforms are now accessible.")
		);
		return true;
	}
	return false;
}

void UWonderMilestoneSubsystem::CollectCymaticNotes(int32 Amount)
{
	const int32 OldNotes = CollectedNotesCount;
	CollectedNotesCount = FMath::Clamp(CollectedNotesCount + Amount, 0, MaxNotesCount);

	// Progression Unlocks: Every 50 notes awards +1 weapon wheel slot and +1 vehicle bay
	const int32 NewSlots = FMath::Clamp(4 + (CollectedNotesCount / 50), 4, 8);
	const int32 NewBays = FMath::Clamp(2 + (CollectedNotesCount / 50), 2, 10);

	if (NewSlots > UnlockedWeaponWheelSlots || NewBays > UnlockedGarageBays)
	{
		UnlockedWeaponWheelSlots = NewSlots;
		UnlockedGarageBays = NewBays;
		OnMilestoneAchieved.Broadcast(
			TEXT("NotesTierUnlocked"),
			FString::Printf(TEXT("Milestone Reached! Unlocked Weapon Wheel Slot %d and Garage Bay %d."), UnlockedWeaponWheelSlots, UnlockedGarageBays)
		);
	}

	// 1,000 Note Cap: Crown of the Master Architect Prestige Reward
	if (CollectedNotesCount >= MaxNotesCount && OldNotes < MaxNotesCount)
	{
		OnMilestoneAchieved.Broadcast(
			TEXT("MasterArchitectCrown"),
			TEXT("1,000 / 1,000 Cymatic Notes Collected! Unlocked the Crown of the Master Architect (Golden Mechanic Hat) with 2x District Tax Yields.")
		);
	}

	OnCymaticNotesUpdated.Broadcast(CollectedNotesCount, MaxNotesCount, UnlockedWeaponWheelSlots);
}

ASyndicateOverlordBoss* UWonderMilestoneSubsystem::SpawnOneMillionHPOverlordRaid(FVector SpawnLocation)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	UClass* ClassToSpawn = OverlordBossClass ? OverlordBossClass.Get() : ASyndicateOverlordBoss::StaticClass();
	ASyndicateOverlordBoss* Boss = World->SpawnActor<ASyndicateOverlordBoss>(ClassToSpawn, SpawnLocation, FRotator::ZeroRotator, SpawnParams);

	if (Boss)
	{
		// Configure Gargantuan 1,000,000 HP Raid Stats
		Boss->MaxBossHealth = 1000000.0f;
		Boss->CurrentBossHealth = 1000000.0f;
		Boss->SonicBarrierIntegrity = 250000.0f;
		Boss->MechArmorHealth = 500000.0f;
		Boss->bLaserGridActive = true;
		Boss->CurrentPhase = ESyndicateBossPhase::Phase1_SonicBarrierBreach;

		OnOverlordRaidSpawned.Broadcast(Boss);
		OnMilestoneAchieved.Broadcast(
			TEXT("OverlordRaidSpawned"),
			TEXT("ALERT: 1,000,000 HP Syndicate Overlord Raid Spawned! Overcharge substations and time rhythmic deflections to breach.")
		);
	}

	return Boss;
}

bool UWonderMilestoneSubsystem::TriggerMunicipalRebirth(int64& InOutCash)
{
	const int64 RebirthCost = 1000000;
	if (InOutCash < RebirthCost)
	{
		return false;
	}

	InOutCash -= RebirthCost;
	MunicipalRebirthCount += 1;
	RebirthRevenueMultiplier *= 2.0f;

	OnMunicipalRebirthTriggered.Broadcast(MunicipalRebirthCount, RebirthRevenueMultiplier);
	OnMilestoneAchieved.Broadcast(
		TEXT("MunicipalRebirth"),
		FString::Printf(TEXT("Municipal Rebirth Tier %d Achieved! District buildings reset with permanent 2x Revenue Multiplier (Current: %.1fx)."),
			MunicipalRebirthCount, RebirthRevenueMultiplier)
	);

	return true;
}
