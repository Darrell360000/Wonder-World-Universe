// Copyright Darrell R. Lewis / Wonder World Trilogy. All Rights Reserved.

#include "WonderTerminal.h"
#include "WonderSkyscraperActor.h"
#include "WonderLedgerSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

AWonderTerminal::AWonderTerminal()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	TerminalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TerminalMesh"));
	RootComponent = TerminalMesh;
}

void AWonderTerminal::TriggerHostileTakeover()
{
	if (!HasAuthority() || bConverted) return;
	bConverted = true;

	// 1. Swap building material set to golden/cyan Wonder Tower
	if (ParentSkyscraper)
	{
		ParentSkyscraper->TriggerHostileTakeover(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	}
	else
	{
		TArray<AActor*> FoundTowers;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWonderSkyscraperActor::StaticClass(), FoundTowers);
		for (AActor* Actor : FoundTowers)
		{
			if (AWonderSkyscraperActor* Tower = Cast<AWonderSkyscraperActor>(Actor))
			{
				Tower->TriggerHostileTakeover(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
				break;
			}
		}
	}

	// 2. Flip enforcer team to Tophatter guards
	// Notified via OnTakeoverComplete and Tower Takeover delegates

	// 3. Reroute revenue to the F8 ledger
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UWonderLedgerSubsystem* Ledger = GI->GetSubsystem<UWonderLedgerSubsystem>())
		{
			Ledger->RecordGrindIncome(TakeoverRevenueBonus, 500, TEXT("Terminal Hand-Slam Hostile Takeover"));
			Ledger->Col1GrindIncomePerSec += 50.0;
			Ledger->SaveSharedLedgerJson();

			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 6.0f, FColor::Yellow,
					FString::Printf(TEXT("★ TERMINAL HOSTILE TAKEOVER COMPLETE! ★\n+$%.0f Direct Cash Deposit | Revenue rerouted to F8 Ledger!"), TakeoverRevenueBonus));
			}
		}
	}

	UGameplayStatics::PlaySound2D(GetWorld(), nullptr);
	OnTakeoverComplete.Broadcast(this); // Blueprint hook for the victory VFX
}
