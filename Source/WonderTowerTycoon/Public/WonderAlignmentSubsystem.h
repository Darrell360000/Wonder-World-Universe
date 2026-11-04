#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WonderAlignmentSubsystem.generated.h"

UENUM(BlueprintType)
enum class EWonderAlignmentTier : uint8
{
	FullCorporateMogul   UMETA(DisplayName = "Full Corporate Mogul (Ruthless Shadow)"),
	MogulLeaning         UMETA(DisplayName = "Mogul Leaning"),
	Neutral              UMETA(DisplayName = "Neutral Pragmatist"),
	GuardianLeaning      UMETA(DisplayName = "Guardian Leaning"),
	FullCivicGuardian    UMETA(DisplayName = "Full Civic Guardian (Wonder Light)")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAlignmentChanged, float, NewScore, EWonderAlignmentTier, NewTier, FString, Reason);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAlignmentPerksRecalculated, float, DressbotDiscount, float, SyndicateAggression, float, CorporateCreditBonus);

UCLASS()
class WONDERTOWERTYCOON_API UWonderAlignmentSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/* --- ALIGNMENT SPECTRUM (-100.0 to +100.0) --- */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wonder|Alignment")
	float AlignmentScore = 15.0f; // Starts slightly civic

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wonder|Alignment")
	EWonderAlignmentTier CurrentTier = EWonderAlignmentTier::Neutral;

	UFUNCTION(BlueprintCallable, Category = "Wonder|Alignment - Actions")
	void ModifyAlignment(float DeltaScore, const FString& Reason);

	UFUNCTION(BlueprintPure, Category = "Wonder|Alignment - Queries")
	float GetAlignmentScore() const { return AlignmentScore; }

	UFUNCTION(BlueprintPure, Category = "Wonder|Alignment - Queries")
	EWonderAlignmentTier GetAlignmentTier() const { return CurrentTier; }

	/* --- PERK SCALING & GAMEPLAY MODIFIERS --- */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wonder|Alignment - Perks")
	float DressbotDiscountRate = 0.0f; // 0.0 to 0.35 (35% discount for Guardians)

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wonder|Alignment - Perks")
	float SyndicateAggressionMultiplier = 1.0f; // 0.8x to 1.6x

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wonder|Alignment - Perks")
	float CorporateCreditBonusMultiplier = 1.0f; // 1.0x to 2.0x (for Moguls)

	UFUNCTION(BlueprintPure, Category = "Wonder|Alignment - Perks")
	float GetDressbotDiscountRate() const { return DressbotDiscountRate; }

	UFUNCTION(BlueprintPure, Category = "Wonder|Alignment - Perks")
	float GetSyndicateAggressionMultiplier() const { return SyndicateAggressionMultiplier; }

	UFUNCTION(BlueprintPure, Category = "Wonder|Alignment - Perks")
	float GetCorporateCreditBonusMultiplier() const { return CorporateCreditBonusMultiplier; }

	/* --- EVENTS --- */
	UPROPERTY(BlueprintAssignable, Category = "Wonder|Alignment|Events")
	FOnAlignmentChanged OnAlignmentChanged;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|Alignment|Events")
	FOnAlignmentPerksRecalculated OnAlignmentPerksRecalculated;

private:
	void RecalculateTierAndPerks();
};
