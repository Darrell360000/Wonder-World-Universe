// Copyright Darrell R. Lewis / Wonder World Trilogy. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WonderStratagemComponent.generated.h"

UENUM(BlueprintType)
enum class EStratagemInputDirection : uint8
{
	Up     UMETA(DisplayName = "Up (↑)"),
	Down   UMETA(DisplayName = "Down (↓)"),
	Left   UMETA(DisplayName = "Left (←)"),
	Right  UMETA(DisplayName = "Right (→)")
};

UENUM(BlueprintType)
enum class EStratagemAirdropType : uint8
{
	None                  UMETA(DisplayName = "None"),
	OrbitalLightningBeam  UMETA(DisplayName = "Net-Power Lightning Beam"),
	TophatterTurret       UMETA(DisplayName = "Tophatter Defense Turret"),
	HovercraftDrop        UMETA(DisplayName = "Instant Hovercraft Airdrop"),
	HeavyCraneSmash       UMETA(DisplayName = "Heavy Crane Smash Drop")
};

USTRUCT(BlueprintType)
struct FStratagemRecipe
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Stratagem")
	EStratagemAirdropType AirdropType = EStratagemAirdropType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Stratagem")
	FString DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Stratagem")
	TArray<EStratagemInputDirection> CodeSequence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Stratagem")
	float NetPowerCost = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Stratagem")
	float CooldownDuration = 15.0f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStratagemBufferChanged, const TArray<EStratagemInputDirection>&, CurrentBuffer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnOrbitalAirdropSpawned, EStratagemAirdropType, AirdropType, FVector, DropLocation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStratagemCodeFailed, FString, FailReason);

/**
 * UWonderStratagemComponent
 * Implements Helldivers 2 inspired D-pad directional sequence parsing (e.g. Up-Right-Down-Down-Up)
 * and spawns orbital airdrops from Mama Tower (Net-Power Lightning Beam, Tophatter Turret, Hovercraft Drop).
 */
UCLASS(ClassGroup=(WonderWorld), meta=(BlueprintSpawnableComponent))
class WONDERTOWERTYCOON_API UWonderStratagemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWonderStratagemComponent();

	/* --- D-PAD INPUT PARSING --- */
	UFUNCTION(BlueprintCallable, Category = "Wonder|Stratagem")
	bool InputDirection(EStratagemInputDirection Direction, FVector TargetDropLocation);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Stratagem")
	void ResetInputBuffer();

	UFUNCTION(BlueprintCallable, Category = "Wonder|Stratagem")
	bool SpawnOrbitalAirdrop(EStratagemAirdropType Type, FVector DropLocation);

	/* --- QUERIES --- */
	UFUNCTION(BlueprintPure, Category = "Wonder|Stratagem")
	const TArray<EStratagemInputDirection>& GetCurrentBuffer() const { return InputBuffer; }

	UFUNCTION(BlueprintPure, Category = "Wonder|Stratagem")
	TArray<FStratagemRecipe> GetRegisteredRecipes() const { return RegisteredRecipes; }

	UFUNCTION(BlueprintPure, Category = "Wonder|Stratagem")
	bool IsStratagemReady(EStratagemAirdropType Type) const;

	/* --- TUNING PARAMETERS --- */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Stratagem|Tuning")
	float MaxSequenceTimeoutSeconds = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Stratagem|Tuning")
	float OrbitalBeamDamage = 1200.0f;

	/* --- EVENTS --- */
	UPROPERTY(BlueprintAssignable, Category = "Wonder|Stratagem|Events")
	FOnStratagemBufferChanged OnStratagemBufferChanged;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|Stratagem|Events")
	FOnOrbitalAirdropSpawned OnOrbitalAirdropSpawned;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|Stratagem|Events")
	FOnStratagemCodeFailed OnStratagemCodeFailed;

protected:
	virtual void BeginPlay() override;

private:
	void InitializeDefaultRecipes();
	bool EvaluateBufferMatch(FVector TargetLocation);

	TArray<EStratagemInputDirection> InputBuffer;
	TArray<FStratagemRecipe> RegisteredRecipes;
	TMap<EStratagemAirdropType, double> CooldownTimestamps;
	float LastInputTimestamp = 0.0f;
};
