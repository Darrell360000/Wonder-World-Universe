// Copyright Darrell R. Lewis / Wonder World Trilogy. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "WonderStratagemSubsystem.generated.h"

UENUM(BlueprintType)
enum class EStratagemDirection : uint8
{
	Up     UMETA(DisplayName = "Up (↑)"),
	Down   UMETA(DisplayName = "Down (↓)"),
	Left   UMETA(DisplayName = "Left (←)"),
	Right  UMETA(DisplayName = "Right (→)")
};

UENUM(BlueprintType)
enum class EStratagemType : uint8
{
	None                   UMETA(DisplayName = "None"),
	OrbitalLightning       UMETA(DisplayName = "Net-Power Orbital Lightning Strike"),
	TophatterTurret        UMETA(DisplayName = "Autonomous Tophatter Defense Turret"),
	HeavyCraneSmash        UMETA(DisplayName = "Heavy Crane Smash Drop"),
	InstantHovercraft      UMETA(DisplayName = "Instant Hovercraft Airdrop")
};

USTRUCT(BlueprintType)
struct FStratagemDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Stratagem")
	EStratagemType Type = EStratagemType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Stratagem")
	FString DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Stratagem")
	TArray<EStratagemDirection> CodeSequence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Stratagem")
	float NetPowerCost = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Stratagem")
	float CooldownSeconds = 15.0f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStratagemSequenceUpdated, const TArray<EStratagemDirection>&, CurrentSequence);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStratagemExecuted, EStratagemType, ActivatedType, FVector, TargetLocation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStratagemFailed, FString, Reason);

/**
 * UWonderStratagemSubsystem
 * Implements Helldivers 2 inspired Municipal Stratagem Call-Ins via the Wonder Phone.
 * Players enter D-pad directional sequences to call down orbital lightning, defense turrets,
 * crane wrecking balls, and instant vehicle airdrops onto the open-world streets.
 */
UCLASS()
class WONDERTOWERTYCOON_API UWonderStratagemSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/* --- D-PAD INPUT INTERFACE --- */
	UFUNCTION(BlueprintCallable, Category = "Wonder|Stratagem")
	bool InputDirection(EStratagemDirection Direction, FVector TargetAimLocation);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Stratagem")
	void ClearInputSequence();

	UFUNCTION(BlueprintPure, Category = "Wonder|Stratagem")
	const TArray<EStratagemDirection>& GetCurrentSequence() const { return CurrentSequence; }

	UFUNCTION(BlueprintPure, Category = "Wonder|Stratagem")
	TArray<FStratagemDefinition> GetAvailableStratagems() const { return RegisteredStratagems; }

	/* --- EVENTS --- */
	UPROPERTY(BlueprintAssignable, Category = "Wonder|Stratagem|Events")
	FOnStratagemSequenceUpdated OnStratagemSequenceUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|Stratagem|Events")
	FOnStratagemExecuted OnStratagemExecuted;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|Stratagem|Events")
	FOnStratagemFailed OnStratagemFailed;

private:
	void RegisterDefaultStratagems();
	bool EvaluateCurrentSequence(FVector TargetLocation);

	TArray<EStratagemDirection> CurrentSequence;
	TArray<FStratagemDefinition> RegisteredStratagems;
	TMap<EStratagemType, double> CooldownTimers;
};
