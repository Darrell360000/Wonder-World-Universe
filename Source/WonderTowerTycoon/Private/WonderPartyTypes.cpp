#include "WonderPartyTypes.h"

UWonderMiniGame::UWonderMiniGame()
	: ActivePlayerCount(1)
	, CurrentAudience(EPartyAudienceDial::CasualFriends)
	, TimeRemaining(0.0f)
	, bIsRunning(false)
{
}

void UWonderMiniGame::StartGame(int32 PlayerCount, EPartyAudienceDial AudienceDial, float DurationSeconds)
{
	ActivePlayerCount = FMath::Max(1, PlayerCount);
	CurrentAudience = AudienceDial;
	TimeRemaining = DurationSeconds;
	bIsRunning = true;

	K2_OnGameStarted();
}

void UWonderMiniGame::EndGame(int32 WinningPlayerIndex, int32 FinalScore)
{
	if (!bIsRunning)
	{
		return;
	}

	bIsRunning = false;
	K2_OnGameEnded(WinningPlayerIndex, FinalScore);
	OnGameEnded.Broadcast(WinningPlayerIndex, FinalScore);
}

void UWonderMiniGame::TickGame(float DeltaTime)
{
	if (!bIsRunning)
	{
		return;
	}

	TimeRemaining = FMath::Max(0.0f, TimeRemaining - DeltaTime);
	if (TimeRemaining <= 0.0f)
	{
		EndGame(0, 0);
	}
}
