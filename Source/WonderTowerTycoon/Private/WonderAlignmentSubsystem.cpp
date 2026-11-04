#include "WonderAlignmentSubsystem.h"

void UWonderAlignmentSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	RecalculateTierAndPerks();
}

void UWonderAlignmentSubsystem::ModifyAlignment(float DeltaScore, const FString& Reason)
{
	AlignmentScore = FMath::Clamp(AlignmentScore + DeltaScore, -100.0f, 100.0f);
	RecalculateTierAndPerks();

	UE_LOG(LogTemp, Log, TEXT("[WonderAlignment] Score adjusted by %+.1f (%s) -> New Score: %.1f | Tier: %d"),
		DeltaScore, *Reason, AlignmentScore, static_cast<int32>(CurrentTier));

	OnAlignmentChanged.Broadcast(AlignmentScore, CurrentTier, Reason);
}

void UWonderAlignmentSubsystem::RecalculateTierAndPerks()
{
	if (AlignmentScore <= -60.0f)
	{
		CurrentTier = EWonderAlignmentTier::FullCorporateMogul;
	}
	else if (AlignmentScore <= -20.0f)
	{
		CurrentTier = EWonderAlignmentTier::MogulLeaning;
	}
	else if (AlignmentScore < 20.0f)
	{
		CurrentTier = EWonderAlignmentTier::Neutral;
	}
	else if (AlignmentScore < 60.0f)
	{
		CurrentTier = EWonderAlignmentTier::GuardianLeaning;
	}
	else
	{
		CurrentTier = EWonderAlignmentTier::FullCivicGuardian;
	}

	if (AlignmentScore >= 0.0f)
	{
		// Civic Guardian Perks: Up to 35% Dressbot discounts; Syndicate escalates against public hero
		DressbotDiscountRate = FMath::GetMappedRangeValueClamped(UE::Math::TVector2<float>(0.f, 100.f), UE::Math::TVector2<float>(0.f, 0.35f), AlignmentScore);
		SyndicateAggressionMultiplier = FMath::GetMappedRangeValueClamped(UE::Math::TVector2<float>(0.f, 100.f), UE::Math::TVector2<float>(1.0f, 1.6f), AlignmentScore);
		CorporateCreditBonusMultiplier = 1.0f;
	}
	else
	{
		// Corporate Mogul Perks: Up to 2.0x corporate credit yields; lower street heat but higher corporate collusion
		DressbotDiscountRate = 0.0f;
		SyndicateAggressionMultiplier = FMath::GetMappedRangeValueClamped(UE::Math::TVector2<float>(-100.f, 0.f), UE::Math::TVector2<float>(0.75f, 1.0f), AlignmentScore);
		CorporateCreditBonusMultiplier = FMath::GetMappedRangeValueClamped(UE::Math::TVector2<float>(-100.f, 0.f), UE::Math::TVector2<float>(2.0f, 1.0f), AlignmentScore);
	}

	OnAlignmentPerksRecalculated.Broadcast(DressbotDiscountRate, SyndicateAggressionMultiplier, CorporateCreditBonusMultiplier);
}
