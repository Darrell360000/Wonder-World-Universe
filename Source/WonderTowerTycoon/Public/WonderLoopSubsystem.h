#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "WonderLoopSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoopReset, int32, NewLoopCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnClueDiscovered, FName, ClueID, const FString&, ClueDescription);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoopTimeWarning, float, SecondsRemaining);

/**
 * Manages the 22-minute Blackout time-loop mystery in Act II.
 * Preserves discovered clues and Rumor Map state across city resets.
 */
UCLASS()
class WONDERTOWERTYCOON_API UWonderLoopSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	UWonderLoopSubsystem();

	// FTickableGameObject interface
	virtual void Tick(float DeltaTime) override;
	virtual ETickableTickType GetTickableTickType() const override { return ETickableTickType::Always; }
	virtual TStatId GetStatId() const override;
	virtual bool IsTickable() const override { return bLoopActive; }
	virtual bool IsTickableWhenPaused() const override { return false; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wonder|Loop")
	float LoopDurationTotal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wonder|Loop")
	float CurrentLoopTimeRemaining;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wonder|Loop")
	int32 LoopCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wonder|Loop")
	bool bLoopActive;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wonder|Loop")
	TSet<FName> DiscoveredClueIDs;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wonder|Loop")
	TMap<FName, FString> RumorMap;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|Loop")
	FOnLoopReset OnLoopReset;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|Loop")
	FOnClueDiscovered OnClueDiscovered;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|Loop")
	FOnLoopTimeWarning OnLoopTimeWarning;

	UFUNCTION(BlueprintCallable, Category = "Wonder|Loop")
	void StartLoop(float DurationSeconds = 1320.0f);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Loop")
	void StopLoop();

	UFUNCTION(BlueprintCallable, Category = "Wonder|Loop")
	void ResetLoop(bool bTriggerBlackoutVFX = true);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Loop")
	bool DiscoverClue(FName ClueID, const FString& ClueDescription);

	UFUNCTION(BlueprintPure, Category = "Wonder|Loop")
	bool HasClue(FName ClueID) const;

	UFUNCTION(BlueprintCallable, Category = "Wonder|Loop")
	void AddTimeBonus(float Seconds);

	UFUNCTION(BlueprintPure, Category = "Wonder|Loop")
	float GetTimeRemainingMinutes() const;

	UFUNCTION(BlueprintPure, Category = "Wonder|Loop")
	TArray<FName> GetAllDiscoveredClueIDs() const;

private:
	bool bWarning5MinFired;
	bool bWarning1MinFired;
	bool bWarning30SecFired;
};
