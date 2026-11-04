#include "WonderLoopSubsystem.h"

UWonderLoopSubsystem::UWonderLoopSubsystem()
	: LoopDurationTotal(1320.0f) // 22 minutes
	, CurrentLoopTimeRemaining(1320.0f)
	, LoopCount(1)
	, bLoopActive(false)
	, bWarning5MinFired(false)
	, bWarning1MinFired(false)
	, bWarning30SecFired(false)
{
}

void UWonderLoopSubsystem::Tick(float DeltaTime)
{
	if (!bLoopActive)
	{
		return;
	}

	CurrentLoopTimeRemaining = FMath::Max(0.0f, CurrentLoopTimeRemaining - DeltaTime);

	// Warnings
	if (!bWarning5MinFired && CurrentLoopTimeRemaining <= 300.0f)
	{
		bWarning5MinFired = true;
		OnLoopTimeWarning.Broadcast(CurrentLoopTimeRemaining);
	}
	if (!bWarning1MinFired && CurrentLoopTimeRemaining <= 60.0f)
	{
		bWarning1MinFired = true;
		OnLoopTimeWarning.Broadcast(CurrentLoopTimeRemaining);
	}
	if (!bWarning30SecFired && CurrentLoopTimeRemaining <= 30.0f)
	{
		bWarning30SecFired = true;
		OnLoopTimeWarning.Broadcast(CurrentLoopTimeRemaining);
	}

	if (CurrentLoopTimeRemaining <= 0.0f)
	{
		ResetLoop(true);
	}
}

TStatId UWonderLoopSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UWonderLoopSubsystem, STATGROUP_Tickables);
}

void UWonderLoopSubsystem::StartLoop(float DurationSeconds)
{
	LoopDurationTotal = FMath::Max(10.0f, DurationSeconds);
	CurrentLoopTimeRemaining = LoopDurationTotal;
	bLoopActive = true;
	bWarning5MinFired = false;
	bWarning1MinFired = false;
	bWarning30SecFired = false;
}

void UWonderLoopSubsystem::StopLoop()
{
	bLoopActive = false;
}

void UWonderLoopSubsystem::ResetLoop(bool bTriggerBlackoutVFX)
{
	LoopCount++;
	CurrentLoopTimeRemaining = LoopDurationTotal;
	bWarning5MinFired = false;
	bWarning1MinFired = false;
	bWarning30SecFired = false;

	OnLoopReset.Broadcast(LoopCount);
}

bool UWonderLoopSubsystem::DiscoverClue(FName ClueID, const FString& ClueDescription)
{
	if (ClueID.IsNone() || DiscoveredClueIDs.Contains(ClueID))
	{
		return false;
	}

	DiscoveredClueIDs.Add(ClueID);
	RumorMap.Add(ClueID, ClueDescription);
	OnClueDiscovered.Broadcast(ClueID, ClueDescription);
	return true;
}

bool UWonderLoopSubsystem::HasClue(FName ClueID) const
{
	return DiscoveredClueIDs.Contains(ClueID);
}

void UWonderLoopSubsystem::AddTimeBonus(float Seconds)
{
	CurrentLoopTimeRemaining = FMath::Clamp(CurrentLoopTimeRemaining + Seconds, 0.0f, LoopDurationTotal);
}

float UWonderLoopSubsystem::GetTimeRemainingMinutes() const
{
	return CurrentLoopTimeRemaining / 60.0f;
}

TArray<FName> UWonderLoopSubsystem::GetAllDiscoveredClueIDs() const
{
	return DiscoveredClueIDs.Array();
}
