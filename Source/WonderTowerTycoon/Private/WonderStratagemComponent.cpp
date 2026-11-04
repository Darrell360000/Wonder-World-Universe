// Copyright Darrell R. Lewis / Wonder World Trilogy. All Rights Reserved.

#include "WonderStratagemComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

UWonderStratagemComponent::UWonderStratagemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UWonderStratagemComponent::BeginPlay()
{
	Super::BeginPlay();
	InitializeDefaultRecipes();
}

void UWonderStratagemComponent::InitializeDefaultRecipes()
{
	RegisteredRecipes.Empty();

	// 1. Net-Power Lightning Beam: ↑ → ↓ ↓ ↑
	{
		FStratagemRecipe Lightning;
		Lightning.AirdropType = EStratagemAirdropType::OrbitalLightningBeam;
		Lightning.DisplayName = TEXT("Net-Power Orbital Lightning Beam");
		Lightning.CodeSequence = {
			EStratagemInputDirection::Up,
			EStratagemInputDirection::Right,
			EStratagemInputDirection::Down,
			EStratagemInputDirection::Down,
			EStratagemInputDirection::Up
		};
		Lightning.NetPowerCost = 50.0f;
		Lightning.CooldownDuration = 18.0f;
		RegisteredRecipes.Add(Lightning);
	}

	// 2. Autonomous Tophatter Defense Turret: ↓ ↑ → ↑
	{
		FStratagemRecipe Turret;
		Turret.AirdropType = EStratagemAirdropType::TophatterTurret;
		Turret.DisplayName = TEXT("Tophatter Defense Turret");
		Turret.CodeSequence = {
			EStratagemInputDirection::Down,
			EStratagemInputDirection::Up,
			EStratagemInputDirection::Right,
			EStratagemInputDirection::Up
		};
		Turret.NetPowerCost = 35.0f;
		Turret.CooldownDuration = 25.0f;
		RegisteredRecipes.Add(Turret);
	}

	// 3. Instant Hovercraft Airdrop: ↑ ↑ ← → ↓
	{
		FStratagemRecipe Hovercraft;
		Hovercraft.AirdropType = EStratagemAirdropType::HovercraftDrop;
		Hovercraft.DisplayName = TEXT("Instant Hovercraft Airdrop");
		Hovercraft.CodeSequence = {
			EStratagemInputDirection::Up,
			EStratagemInputDirection::Up,
			EStratagemInputDirection::Left,
			EStratagemInputDirection::Right,
			EStratagemInputDirection::Down
		};
		Hovercraft.NetPowerCost = 25.0f;
		Hovercraft.CooldownDuration = 15.0f;
		RegisteredRecipes.Add(Hovercraft);
	}

	// 4. Heavy Crane Smash Drop: → ↓ ← ↑ ↓
	{
		FStratagemRecipe Crane;
		Crane.AirdropType = EStratagemAirdropType::HeavyCraneSmash;
		Crane.DisplayName = TEXT("Heavy Crane Smash Drop");
		Crane.CodeSequence = {
			EStratagemInputDirection::Right,
			EStratagemInputDirection::Down,
			EStratagemInputDirection::Left,
			EStratagemInputDirection::Up,
			EStratagemInputDirection::Down
		};
		Crane.NetPowerCost = 75.0f;
		Crane.CooldownDuration = 40.0f;
		RegisteredRecipes.Add(Crane);
	}
}

bool UWonderStratagemComponent::InputDirection(EStratagemInputDirection Direction, FVector TargetDropLocation)
{
	float CurrentTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;

	// Reset if input timed out
	if (InputBuffer.Num() > 0 && (CurrentTime - LastInputTimestamp) > MaxSequenceTimeoutSeconds)
	{
		ResetInputBuffer();
	}

	LastInputTimestamp = CurrentTime;
	InputBuffer.Add(Direction);
	OnStratagemBufferChanged.Broadcast(InputBuffer);

	// Check if this input matches any recipe prefix
	bool bMatchesAnyPrefix = false;
	for (const FStratagemRecipe& Recipe : RegisteredRecipes)
	{
		if (InputBuffer.Num() <= Recipe.CodeSequence.Num())
		{
			bool bPrefixMatch = true;
			for (int32 i = 0; i < InputBuffer.Num(); ++i)
			{
				if (InputBuffer[i] != Recipe.CodeSequence[i])
				{
					bPrefixMatch = false;
					break;
				}
			}

			if (bPrefixMatch)
			{
				bMatchesAnyPrefix = true;
				break;
			}
		}
	}

	if (!bMatchesAnyPrefix)
	{
		ResetInputBuffer();
		OnStratagemCodeFailed.Broadcast(TEXT("Sequence mis-input. Buffer reset."));
		return false;
	}

	return EvaluateBufferMatch(TargetDropLocation);
}

void UWonderStratagemComponent::ResetInputBuffer()
{
	InputBuffer.Empty();
	OnStratagemBufferChanged.Broadcast(InputBuffer);
}

bool UWonderStratagemComponent::EvaluateBufferMatch(FVector TargetLocation)
{
	for (const FStratagemRecipe& Recipe : RegisteredRecipes)
	{
		if (InputBuffer == Recipe.CodeSequence)
		{
			if (!IsStratagemReady(Recipe.AirdropType))
			{
				double CurrentTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0;
				float Remaining = CooldownTimestamps[Recipe.AirdropType] - CurrentTime;
				ResetInputBuffer();
				OnStratagemCodeFailed.Broadcast(FString::Printf(TEXT("Stratagem '%s' on cooldown (%.1fs remaining)"), *Recipe.DisplayName, Remaining));
				return false;
			}

			// Deploy the airdrop!
			ResetInputBuffer();
			return SpawnOrbitalAirdrop(Recipe.AirdropType, TargetLocation);
		}
	}

	return false;
}

bool UWonderStratagemComponent::SpawnOrbitalAirdrop(EStratagemAirdropType Type, FVector DropLocation)
{
	if (Type == EStratagemAirdropType::None)
	{
		return false;
	}

	double CurrentTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0;

	// Find recipe duration
	float Cooldown = 15.0f;
	for (const FStratagemRecipe& Recipe : RegisteredRecipes)
	{
		if (Recipe.AirdropType == Type)
		{
			Cooldown = Recipe.CooldownDuration;
			break;
		}
	}

	CooldownTimestamps.Add(Type, CurrentTime + Cooldown);
	OnOrbitalAirdropSpawned.Broadcast(Type, DropLocation);

	UE_LOG(LogTemp, Warning, TEXT("[WonderStratagem] ORBITAL AIRDROP INBOUND: Type %d delivered to %s"),
		static_cast<int32>(Type), *DropLocation.ToString());

	return true;
}

bool UWonderStratagemComponent::IsStratagemReady(EStratagemAirdropType Type) const
{
	if (!CooldownTimestamps.Contains(Type))
	{
		return true;
	}

	double CurrentTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0;
	return CurrentTime >= CooldownTimestamps[Type];
}
