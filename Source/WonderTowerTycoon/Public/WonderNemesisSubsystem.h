#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WonderNemesisSubsystem.generated.h"

UENUM(BlueprintType)
enum class ENemesisRank : uint8
{
	Enforcer           UMETA(DisplayName = "Street Enforcer (Tier 1)"),
	StreetCaptain      UMETA(DisplayName = "Street Captain (Tier 2)"),
	DistrictWarlord    UMETA(DisplayName = "District Warlord (Tier 3)"),
	ExecutiveOverlord  UMETA(DisplayName = "Executive Overlord (Tier 4)")
};

UENUM(BlueprintType)
enum class ENemesisTrait : uint8
{
	HatResistance    UMETA(DisplayName = "Hat Resistance (Immune to Discus Decapitation)"),
	HoverJammer      UMETA(DisplayName = "Hover Jammer (Grounds Vehicles in 40m Radius)"),
	RevengeAmbush    UMETA(DisplayName = "Revenge Ambush (Spawns Surprise Street Attacks)"),
	CymaticShield    UMETA(DisplayName = "Cymatic Shield (Absorbs Beat Attacks)"),
	SubBassArmor     UMETA(DisplayName = "Sub-Bass Armor (Heavy Ballistic Plating)")
};

USTRUCT(BlueprintType)
struct WONDERTOWERTYCOON_API FSyndicateNemesisCaptain
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Nemesis")
	FName CaptainID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Nemesis")
	FString CaptainName = TEXT("Viktor 'The Sledge' Kuroda");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Nemesis")
	ENemesisRank Rank = ENemesisRank::StreetCaptain;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Nemesis")
	int32 PowerLevel = 25;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Nemesis")
	TArray<ENemesisTrait> Traits;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Nemesis")
	int32 DefeatedPlayerCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Nemesis")
	FString AssignedDistrict = TEXT("Neon Boulevard");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Nemesis")
	bool bIsAlive = true;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNemesisPromoted, const FSyndicateNemesisCaptain&, Captain);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNemesisDefeated, const FSyndicateNemesisCaptain&, Captain);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNemesisAmbushTriggered, const FSyndicateNemesisCaptain&, Captain);

UCLASS()
class WONDERTOWERTYCOON_API UWonderNemesisSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/* --- NEMESIS HIERARCHY --- */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wonder|Nemesis")
	TMap<FName, FSyndicateNemesisCaptain> NemesisRoster;

	UFUNCTION(BlueprintCallable, Category = "Wonder|Nemesis - Actions")
	void RecordPlayerDefeatedBy(FName CaptainID);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Nemesis - Actions")
	void RecordCaptainDefeated(FName CaptainID);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Nemesis - Actions")
	void RegisterNemesisCaptain(const FSyndicateNemesisCaptain& NewCaptain);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Nemesis - Actions")
	bool CheckForRevengeAmbush(FVector PlayerLocation, FSyndicateNemesisCaptain& OutAmbushCaptain);

	UFUNCTION(BlueprintPure, Category = "Wonder|Nemesis - Queries")
	void GetActiveNemesisCaptains(TArray<FSyndicateNemesisCaptain>& OutCaptains) const;

	/* --- EVENTS --- */
	UPROPERTY(BlueprintAssignable, Category = "Wonder|Nemesis|Events")
	FOnNemesisPromoted OnNemesisPromoted;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|Nemesis|Events")
	FOnNemesisDefeated OnNemesisDefeated;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|Nemesis|Events")
	FOnNemesisAmbushTriggered OnNemesisAmbushTriggered;

private:
	void SeedInitialNemesisHierarchy();
};
