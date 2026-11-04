#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WonderMilestoneSubsystem.generated.h"

class ASyndicateOverlordBoss;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMilestoneAchieved, FString, MilestoneID, FString, RewardDescription);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOverlordRaidSpawned, ASyndicateOverlordBoss*, BossActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCymaticNotesUpdated, int32, CurrentNotes, int32, MaxNotes, int32, WeaponSlotsUnlocked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMunicipalRebirthTriggered, int32, NewRebirthCount, float, NewMultiplier);

UCLASS()
class WONDERTOWERTYCOON_API UWonderMilestoneSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/* --- 1. 1,000,000 PHYSICAL CASH MILESTONE --- */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wonder|Milestones - Cash")
	bool bOneMillionCashUnlocked = false;

	UFUNCTION(BlueprintCallable, Category = "Wonder|Milestones - Cash")
	bool CheckCashMilestone(int64 CurrentCash);

	/* --- 2. 1,000,000 NET-POWER MW MILESTONE --- */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wonder|Milestones - Power")
	bool bOneMillionNetPowerUnlocked = false;

	UFUNCTION(BlueprintCallable, Category = "Wonder|Milestones - Power")
	bool CheckNetPowerMilestone(float CurrentPowerMW);

	/* --- 3. 1,000 CYMATIC NOTE COLLECTIBLE ENGINE --- */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wonder|Milestones - Collectibles")
	int32 CollectedNotesCount = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wonder|Milestones - Collectibles")
	int32 MaxNotesCount = 1000;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wonder|Milestones - Unlocks")
	int32 UnlockedWeaponWheelSlots = 4;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wonder|Milestones - Unlocks")
	int32 UnlockedGarageBays = 2;

	UFUNCTION(BlueprintCallable, Category = "Wonder|Milestones - Collectibles")
	void CollectCymaticNotes(int32 Amount = 1);

	/* --- 4. 1,000,000 HP SYNDICATE OVERLORD RAID SPAWNER --- */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Milestones - Boss")
	TSubclassOf<ASyndicateOverlordBoss> OverlordBossClass;

	UFUNCTION(BlueprintCallable, Category = "Wonder|Milestones - Boss")
	ASyndicateOverlordBoss* SpawnOneMillionHPOverlordRaid(FVector SpawnLocation);

	/* --- 5. MUNICIPAL REBIRTH LOOP (2X MULTIPLIER) --- */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wonder|Milestones - Rebirth")
	int32 MunicipalRebirthCount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wonder|Milestones - Rebirth")
	float RebirthRevenueMultiplier = 1.0f;

	UFUNCTION(BlueprintCallable, Category = "Wonder|Milestones - Rebirth")
	bool TriggerMunicipalRebirth(int64& InOutCash);

	/* --- EVENTS & NOTIFICATIONS --- */
	UPROPERTY(BlueprintAssignable, Category = "Wonder|Milestones|Events")
	FOnMilestoneAchieved OnMilestoneAchieved;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|Milestones|Events")
	FOnOverlordRaidSpawned OnOverlordRaidSpawned;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|Milestones|Events")
	FOnCymaticNotesUpdated OnCymaticNotesUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|Milestones|Events")
	FOnMunicipalRebirthTriggered OnMunicipalRebirthTriggered;
};
