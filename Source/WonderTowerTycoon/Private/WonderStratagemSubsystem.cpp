// Copyright Darrell R. Lewis / Wonder World Trilogy. All Rights Reserved.

#include "WonderStratagemSubsystem.h"
#include "Engine/World.h"

void UWonderStratagemSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	RegisterDefaultStratagems();

	UE_LOG(LogTemp, Log, TEXT("[WonderStratagem] UWonderStratagemSubsystem initialized with %d Municipal Stratagems."), RegisteredStratagems.Num());
}

void UWonderStratagemSubsystem::RegisterDefaultStratagems()
{
	RegisteredStratagems.Empty();

	// 1. Net-Power Orbital Lightning: ↑ → ↓ ↓ ↑
	{
		FStratagemDefinition Orbital;
		Orbital.Type = EStratagemType::OrbitalLightning;
		Orbital.DisplayName = TEXT("Net-Power Orbital Lightning");
		Orbital.CodeSequence = { EStratagemDirection::Up, EStratagemDirection::Right, EStratagemDirection::Down, EStratagemDirection::Down, EStratagemDirection::Up };
		Orbital.NetPowerCost = 50.0f;
		Orbital.CooldownSeconds = 20.0f;
		RegisteredStratagems.Add(Orbital);
	}

	// 2. Autonomous Tophatter Defense Turret: ↓ ↑ → ↑
	{
		FStratagemDefinition Turret;
		Turret.Type = EStratagemType::TophatterTurret;
		Turret.DisplayName = TEXT("Autonomous Tophatter Defense Turret");
		Turret.CodeSequence = { EStratagemDirection::Down, EStratagemDirection::Up, EStratagemDirection::Right, EStratagemDirection::Up };
		Turret.NetPowerCost = 35.0f;
		Turret.CooldownSeconds = 30.0f;
		RegisteredStratagems.Add(Turret);
	}

	// 3. Heavy Crane Smash Drop: → ↓ ← ↑ ↓
	{
		FStratagemDefinition Crane;
		Crane.Type = EStratagemType::HeavyCraneSmash;
		Crane.DisplayName = TEXT("Heavy Crane Smash Drop");
		Crane.CodeSequence = { EStratagemDirection::Right, EStratagemDirection::Down, EStratagemDirection::Left, EStratagemDirection::Up, EStratagemDirection::Down };
		Crane.NetPowerCost = 75.0f;
		Crane.CooldownSeconds = 45.0f;
		RegisteredStratagems.Add(Crane);
	}

	// 4. Instant Hovercraft Airdrop: ↑ ↑ ← → ↓
	{
		FStratagemDefinition Hovercraft;
		Hovercraft.Type = EStratagemType::InstantHovercraft;
		Hovercraft.DisplayName = TEXT("Instant Hovercraft Airdrop");
		Hovercraft.CodeSequence = { EStratagemDirection::Up, EStratagemDirection::Up, EStratagemDirection::Left, EStratagemDirection::Right, EStratagemDirection::Down };
		Hovercraft.NetPowerCost = 25.0f;
		Hovercraft.CooldownSeconds = 15.0f;
		RegisteredStratagems.Add(Hovercraft);
	}
}

bool UWonderStratagemSubsystem::InputDirection(EStratagemDirection Direction, FVector TargetAimLocation)
{
	CurrentSequence.Add(Direction);
	OnStratagemSequenceUpdated.Broadcast(CurrentSequence);

	// Check if this input matches any prefix in registered stratagems
	bool bMatchesAnyPrefix = false;
	for (const FStratagemDefinition& Def : RegisteredStratagems)
	{
		if (CurrentSequence.Num() <= Def.CodeSequence.Num())
		{
			bool bPrefixValid = true;
			for (int32 i = 0; i < CurrentSequence.Num(); ++i)
			{
				if (CurrentSequence[i] != Def.CodeSequence[i])
				{
					bPrefixValid = false;
					break;
				}
			}

			if (bPrefixValid)
			{
				bMatchesAnyPrefix = true;
				break;
			}
		}
	}

	// If sequence broke all possible codes, reset
	if (!bMatchesAnyPrefix)
	{
		ClearInputSequence();
		OnStratagemFailed.Broadcast(TEXT("Invalid D-Pad Code Sequence"));
		return false;
	}

	// Evaluate if a complete stratagem was completed
	return EvaluateCurrentSequence(TargetAimLocation);
}

void UWonderStratagemSubsystem::ClearInputSequence()
{
	CurrentSequence.Empty();
	OnStratagemSequenceUpdated.Broadcast(CurrentSequence);
}

bool UWonderStratagemSubsystem::EvaluateCurrentSequence(FVector TargetLocation)
{
	for (const FStratagemDefinition& Def : RegisteredStratagems)
	{
		if (CurrentSequence == Def.CodeSequence)
		{
			double CurrentTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0;
			if (CooldownTimers.Contains(Def.Type) && CurrentTime < CooldownTimers[Def.Type])
			{
				float Remaining = CooldownTimers[Def.Type] - CurrentTime;
				ClearInputSequence();
				OnStratagemFailed.Broadcast(FString::Printf(TEXT("Stratagem on cooldown (%.1fs remaining)"), Remaining));
				return false;
			}

			// Activate stratagem
			CooldownTimers.Add(Def.Type, CurrentTime + Def.CooldownSeconds);
			ClearInputSequence();
			OnStratagemExecuted.Broadcast(Def.Type, TargetLocation);

			UE_LOG(LogTemp, Warning, TEXT("[WonderStratagem] MUNICIPAL STRATAGEM DEPLOYED: %s at %s"),
				*Def.DisplayName, *TargetLocation.ToString());
			return true;
		}
	}

	return false;
}
