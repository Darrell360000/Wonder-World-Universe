// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WonderSquadManagerSubsystem.generated.h"

UENUM(BlueprintType)
enum class EWonderHeroRole : uint8
{
	DarrellXGen     UMETA(DisplayName = "Darrell X-Gen (Sound Magic)"),
	MinimeTactical  UMETA(DisplayName = "Minime Tophatter (Tactical Advisor)"),
	SpiderPulse     UMETA(DisplayName = "Spider-Pulse (Rhythm Skirmisher)"),
	DivineHands     UMETA(DisplayName = "Divine Hands (Macro Architect)"),
	JaxsonThorne    UMETA(DisplayName = "Jaxson Thorne (Industrial Brawler)"),
	TaoXZen         UMETA(DisplayName = "Tao X Zen (Chrono Warden)"),
	MamaWonder      UMETA(DisplayName = "Mama Wonder (Empress & Mayor)")
};

USTRUCT(BlueprintType)
struct FWonderHeroInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WonderWorld|Squad")
	EWonderHeroRole Role = EWonderHeroRole::DarrellXGen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WonderWorld|Squad")
	FString HeroName = "Darrell X-Gen";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WonderWorld|Squad")
	FString SignatureHat = "Equalizer X-Hat (SOUND MAGIC)";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WonderWorld|Squad")
	FString SignatureWeapon = "Dual Conductor Baton Wands";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WonderWorld|Squad")
	bool bIsUnlocked = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WonderWorld|Squad")
	bool bIsInActiveSquad = true;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnActiveHeroChanged, EWonderHeroRole, NewRole, FString, HeroName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSquadFormationUpdated, int32, SquadSize);

/**
 * WonderSquadManagerSubsystem
 * Manages player hero switching, companion party formations, and AI support directives.
 */
UCLASS()
class WONDERTOWERTYCOON_API UWonderSquadManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Switches the active controlled player character to target hero */
	UFUNCTION(BlueprintCallable, Category = "WonderWorld|Squad")
	bool SwitchActiveHero(EWonderHeroRole TargetRole);

	/** Returns all registered heroes */
	UFUNCTION(BlueprintPure, Category = "WonderWorld|Squad")
	TArray<FWonderHeroInfo> GetHeroRoster() const { return HeroRoster; }

	/** Returns current active hero role */
	UFUNCTION(BlueprintPure, Category = "WonderWorld|Squad")
	EWonderHeroRole GetCurrentActiveHero() const { return CurrentActiveHero; }

	UPROPERTY(BlueprintAssignable, Category = "WonderWorld|Squad|Events")
	FOnActiveHeroChanged OnActiveHeroChanged;

	UPROPERTY(BlueprintAssignable, Category = "WonderWorld|Squad|Events")
	FOnSquadFormationUpdated OnSquadFormationUpdated;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WonderWorld|Squad")
	EWonderHeroRole CurrentActiveHero = EWonderHeroRole::DarrellXGen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WonderWorld|Squad")
	TArray<FWonderHeroInfo> HeroRoster;
};
