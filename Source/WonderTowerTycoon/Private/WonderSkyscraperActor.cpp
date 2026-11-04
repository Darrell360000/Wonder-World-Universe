#include "WonderSkyscraperActor.h"
#include "WonderFloorDraftingComponent.h"
#include "WonderLedgerSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

AWonderSkyscraperActor::AWonderSkyscraperActor()
{
	PrimaryActorTick.bCanEverTick = false;

	ExteriorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ExteriorMesh"));
	RootComponent = ExteriorMesh;

	HelipadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HelipadMesh"));
	HelipadMesh->SetupAttachment(RootComponent);
	HelipadMesh->ComponentTags.Add(FName("DrivableVehicleLandingPad"));
	Tags.Add(FName("DrivableVehicleLandingPad"));

	FloorDraftingComponent = CreateDefaultSubobject<UWonderFloorDraftingComponent>(TEXT("FloorDraftingComponent"));
}

void AWonderSkyscraperActor::BeginPlay()
{
	Super::BeginPlay();

	// If unrolled, 60% chance of spawning as a Syndicate Hostile tower to raid
	if (TowerAlignment == ETowerAlignment::UnknownRoll)
	{
		TowerAlignment = (FMath::FRand() > 0.4f) ? ETowerAlignment::SyndicateHostile : ETowerAlignment::WonderBuilding;
	}

	UpdateTowerVisuals();
}

void AWonderSkyscraperActor::TriggerHostileTakeover(APawn* PlayerPawn)
{
	if (TowerAlignment != ETowerAlignment::SyndicateHostile) return;

	// Convert enemy building into a Wonder Tower and make player the CEO
	TowerAlignment = ETowerAlignment::LiberatedCEOTower;
	UpdateTowerVisuals();

	// Award CEO bonus funds directly into the 3-Column Ledger
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UWonderLedgerSubsystem* Ledger = GI->GetSubsystem<UWonderLedgerSubsystem>())
		{
			Ledger->RecordGrindIncome(DailyRevenueYield, 500, TEXT("Skyscraper Hostile Takeover: CEO Coronation"));
			Ledger->Column1Grind.CommercialLeaseIncome += DailyRevenueYield;
			Ledger->Col1GrindIncomePerSec += 25.0; // Passive per-second Net-Power empire tick
			Ledger->SaveSharedLedgerJson();

			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 6.0f, FColor::Yellow,
					FString::Printf(TEXT("★ HOSTILE TAKEOVER COMPLETE: You are now the CEO! ★\n+Cash Deposited: $%.0f\n+Vitality Credit: +500\n+Empire Yield: +$25.0/s Net-Power Siphon (Press F8 to inspect Ledger)"), (float)DailyRevenueYield));
			}
		}
	}

	// Award CEO bonus funds and trigger celebration audio
	UGameplayStatics::PlaySound2D(GetWorld(), nullptr); // Harmonic victory sting
	OnTowerTakeoverCompleted.Broadcast(this, PlayerPawn);
}

void AWonderSkyscraperActor::SpawnFloorEnemies(int32 FloorNumber)
{
	// Spawns syndicate guards on lower/mid floors and executive boss on top floor
}

void AWonderSkyscraperActor::UpdateTowerVisuals()
{
	if (!ExteriorMesh) return;

	if (TowerAlignment == ETowerAlignment::SyndicateHostile)
	{
		ExteriorMesh->SetVectorParameterValueOnMaterials(FName("NeonTrimColor"), FVector(HostileNeonColor.R, HostileNeonColor.G, HostileNeonColor.B));
	}
	else if (TowerAlignment == ETowerAlignment::LiberatedCEOTower || TowerAlignment == ETowerAlignment::WonderBuilding)
	{
		ExteriorMesh->SetVectorParameterValueOnMaterials(FName("NeonTrimColor"), FVector(LiberatedNeonColor.R, LiberatedNeonColor.G, LiberatedNeonColor.B));
	}
}
