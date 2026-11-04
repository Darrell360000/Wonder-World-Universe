#include "WonderNemesisSubsystem.h"

void UWonderNemesisSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	SeedInitialNemesisHierarchy();
}

void UWonderNemesisSubsystem::SeedInitialNemesisHierarchy()
{
	if (NemesisRoster.Num() > 0) return;

	// Captain 1: Viktor 'The Sledge' Kuroda
	FSyndicateNemesisCaptain Viktor;
	Viktor.CaptainID = FName("Viktor_Kuroda");
	Viktor.CaptainName = TEXT("Viktor 'The Sledge' Kuroda");
	Viktor.Rank = ENemesisRank::StreetCaptain;
	Viktor.PowerLevel = 35;
	Viktor.Traits.Add(ENemesisTrait::HatResistance);
	Viktor.Traits.Add(ENemesisTrait::RevengeAmbush);
	Viktor.AssignedDistrict = TEXT("Neon Boulevard");
	Viktor.bIsAlive = true;
	NemesisRoster.Add(Viktor.CaptainID, Viktor);

	// Captain 2: Viper Jax
	FSyndicateNemesisCaptain Jax;
	Jax.CaptainID = FName("Viper_Jax");
	Jax.CaptainName = TEXT("Viper Jax (Grid Saboteur)");
	Jax.Rank = ENemesisRank::DistrictWarlord;
	Jax.PowerLevel = 55;
	Jax.Traits.Add(ENemesisTrait::HoverJammer);
	Jax.Traits.Add(ENemesisTrait::CymaticShield);
	Jax.AssignedDistrict = TEXT("The Iron Works");
	Jax.bIsAlive = true;
	NemesisRoster.Add(Jax.CaptainID, Jax);

	// Captain 3: Echo Rex
	FSyndicateNemesisCaptain Echo;
	Echo.CaptainID = FName("Echo_Rex");
	Echo.CaptainName = TEXT("Echo Rex");
	Echo.Rank = ENemesisRank::Enforcer;
	Echo.PowerLevel = 20;
	Echo.Traits.Add(ENemesisTrait::RevengeAmbush);
	Echo.AssignedDistrict = TEXT("Sub-City Transit");
	Echo.bIsAlive = true;
	NemesisRoster.Add(Echo.CaptainID, Echo);
}

void UWonderNemesisSubsystem::RecordPlayerDefeatedBy(FName CaptainID)
{
	FSyndicateNemesisCaptain* Captain = NemesisRoster.Find(CaptainID);
	if (!Captain) return;

	Captain->DefeatedPlayerCount++;
	Captain->PowerLevel += 15;

	// Dynamic Promotion Thresholds
	if (Captain->Rank == ENemesisRank::Enforcer && Captain->PowerLevel >= 25)
	{
		Captain->Rank = ENemesisRank::StreetCaptain;
		Captain->Traits.AddUnique(ENemesisTrait::HatResistance);
	}
	else if (Captain->Rank == ENemesisRank::StreetCaptain && Captain->PowerLevel >= 50)
	{
		Captain->Rank = ENemesisRank::DistrictWarlord;
		Captain->Traits.AddUnique(ENemesisTrait::HoverJammer);
	}
	else if (Captain->Rank == ENemesisRank::DistrictWarlord && Captain->PowerLevel >= 80)
	{
		Captain->Rank = ENemesisRank::ExecutiveOverlord;
		Captain->Traits.AddUnique(ENemesisTrait::CymaticShield);
	}

	UE_LOG(LogTemp, Warning, TEXT("[WonderNemesis] PROMOTION: '%s' defeated Darrell! New Power: %d | Rank: %d"),
		*Captain->CaptainName, Captain->PowerLevel, static_cast<int32>(Captain->Rank));

	OnNemesisPromoted.Broadcast(*Captain);
}

void UWonderNemesisSubsystem::RecordCaptainDefeated(FName CaptainID)
{
	FSyndicateNemesisCaptain* Captain = NemesisRoster.Find(CaptainID);
	if (!Captain) return;

	Captain->bIsAlive = false;

	UE_LOG(LogTemp, Log, TEXT("[WonderNemesis] DEFEATED: Syndicate Captain '%s' has been neutralized by Darrell!"),
		*Captain->CaptainName);

	OnNemesisDefeated.Broadcast(*Captain);
}

void UWonderNemesisSubsystem::RegisterNemesisCaptain(const FSyndicateNemesisCaptain& NewCaptain)
{
	NemesisRoster.Add(NewCaptain.CaptainID, NewCaptain);
	OnNemesisPromoted.Broadcast(NewCaptain);
}

bool UWonderNemesisSubsystem::CheckForRevengeAmbush(FVector PlayerLocation, FSyndicateNemesisCaptain& OutAmbushCaptain)
{
	for (auto& Pair : NemesisRoster)
	{
		FSyndicateNemesisCaptain& Captain = Pair.Value;
		if (Captain.bIsAlive && Captain.Traits.Contains(ENemesisTrait::RevengeAmbush))
		{
			// 35% chance to trigger revenge ambush
			if (FMath::FRand() < 0.35f)
			{
				OutAmbushCaptain = Captain;
				UE_LOG(LogTemp, Warning, TEXT("[WonderNemesis] AMBUSH TRIGGERED! '%s' has intercepted Darrell!"),
					*Captain.CaptainName);

				OnNemesisAmbushTriggered.Broadcast(Captain);
				return true;
			}
		}
	}
	return false;
}

void UWonderNemesisSubsystem::GetActiveNemesisCaptains(TArray<FSyndicateNemesisCaptain>& OutCaptains) const
{
	OutCaptains.Empty();
	for (const auto& Pair : NemesisRoster)
	{
		if (Pair.Value.bIsAlive)
		{
			OutCaptains.Add(Pair.Value);
		}
	}
}
