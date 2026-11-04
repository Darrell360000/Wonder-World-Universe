#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WonderPartyTypes.generated.h"

/**
 * Audience dial adjusting party mini-game difficulty, timers, and elimination rules.
 */
UENUM(BlueprintType)
enum class EPartyAudienceDial : uint8
{
	Kids            UMETA(DisplayName = "Kids (No Elimination)"),
	CasualFriends   UMETA(DisplayName = "Casual Friends (Social Rules)"),
	Coworkers       UMETA(DisplayName = "Coworkers (Team Trials)"),
	SoloWithAI      UMETA(DisplayName = "Solo (AI Tophatters)")
};

/**
 * Category of Wonder World party activity.
 */
UENUM(BlueprintType)
enum class EPartyGameCategory : uint8
{
	IndoorSocial    UMETA(DisplayName = "Indoor Social"),
	TeamTrial       UMETA(DisplayName = "Team Trial"),
	SkylineLawn     UMETA(DisplayName = "Skyline Lawn"),
	MindBending     UMETA(DisplayName = "Mind-Bending Mechanic")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPartyGameEnded, int32, WinningPlayerIndex, int32, FinalScore);

/**
 * Base class for all Wonder World party mini-games running in the Block Party Hub.
 */
UCLASS(Blueprintable, BlueprintType, Abstract)
class WONDERTOWERTYCOON_API UWonderMiniGame : public UObject
{
	GENERATED_BODY()

public:
	UWonderMiniGame();

	UPROPERTY(BlueprintReadOnly, Category = "Wonder|Party")
	int32 ActivePlayerCount;

	UPROPERTY(BlueprintReadOnly, Category = "Wonder|Party")
	EPartyAudienceDial CurrentAudience;

	UPROPERTY(BlueprintReadOnly, Category = "Wonder|Party")
	float TimeRemaining;

	UPROPERTY(BlueprintReadOnly, Category = "Wonder|Party")
	bool bIsRunning;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|Party")
	FOnPartyGameEnded OnGameEnded;

	UFUNCTION(BlueprintCallable, Category = "Wonder|Party")
	virtual void StartGame(int32 PlayerCount, EPartyAudienceDial AudienceDial, float DurationSeconds = 180.0f);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Party")
	virtual void EndGame(int32 WinningPlayerIndex, int32 FinalScore);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Party")
	virtual void TickGame(float DeltaTime);

	UFUNCTION(BlueprintImplementableEvent, Category = "Wonder|Party", meta = (DisplayName = "OnGameStarted"))
	void K2_OnGameStarted();

	UFUNCTION(BlueprintImplementableEvent, Category = "Wonder|Party", meta = (DisplayName = "OnGameEnded"))
	void K2_OnGameEnded(int32 WinningPlayerIndex, int32 FinalScore);
};

/**
 * Primary Data Asset defining a mini-game inside the Block Party Hub.
 */
UCLASS(BlueprintType)
class WONDERTOWERTYCOON_API UWonderPartyGameDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Party Game Details")
	FName GameID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Party Game Details")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Party Game Details", meta = (MultiLine = true))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Party Game Details")
	EPartyGameCategory Category = EPartyGameCategory::IndoorSocial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Party Game Rules", meta = (ClampMin = "1", ClampMax = "16"))
	int32 MinPlayers = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Party Game Rules", meta = (ClampMin = "1", ClampMax = "16"))
	int32 MaxPlayers = 8;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Party Game Rules", meta = (ClampMin = "10.0", ClampMax = "600.0"))
	float DefaultDurationSeconds = 180.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Party Game Rules")
	bool bSupportsSoloAI = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Party Game Rules")
	TSubclassOf<UWonderMiniGame> MiniGameClass;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(TEXT("WonderPartyGame"), GameID.IsNone() ? GetFName() : GameID);
	}
};
