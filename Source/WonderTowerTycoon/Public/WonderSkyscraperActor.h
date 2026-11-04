#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WonderSkyscraperActor.generated.h"

UENUM(BlueprintType)
enum class ETowerAlignment : uint8
{
	UnknownRoll        UMETA(DisplayName = "Under Construction / Unknown"),
	WonderBuilding     UMETA(DisplayName = "Wonder Building (Friendly Civic Hub)"),
	SyndicateHostile   UMETA(DisplayName = "Syndicate Controlled (Raidable Enemy Tower)"),
	LiberatedCEOTower  UMETA(DisplayName = "Liberated Wonder Tower (Player is CEO)")
};

class UWonderFloorDraftingComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTowerTakeoverCompleted, AWonderSkyscraperActor*, Tower, APawn*, LiberatingPlayer);

UCLASS()
class WONDERTOWERTYCOON_API AWonderSkyscraperActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AWonderSkyscraperActor();

protected:
	virtual void BeginPlay() override;

public:	
	/* --- TOWER PROPERTIES --- */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Tower")
	ETowerAlignment TowerAlignment = ETowerAlignment::SyndicateHostile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Tower - Stats")
	int32 TotalFloors = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Tower - Stats")
	int64 DailyRevenueYield = 15000;

	/* --- TAKEOVER / CEO LOGIC --- */
	UFUNCTION(BlueprintCallable, Category = "Wonder|Tower - Actions")
	void TriggerHostileTakeover(APawn* PlayerPawn);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Tower - Actions")
	void SpawnFloorEnemies(int32 FloorNumber);

	/* --- VISUALS --- */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Tower - Visuals")
	FLinearColor HostileNeonColor = FLinearColor(1.0f, 0.05f, 0.1f, 1.0f); // Red

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Tower - Visuals")
	FLinearColor LiberatedNeonColor = FLinearColor(0.1f, 0.8f, 1.0f, 1.0f); // Wonder Cyan/Gold

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wonder|Tower - Visuals")
	UStaticMeshComponent* ExteriorMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wonder|Tower - Visuals")
	UStaticMeshComponent* HelipadMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wonder|Tower - Drafting")
	UWonderFloorDraftingComponent* FloorDraftingComponent;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|Tower|Events")
	FOnTowerTakeoverCompleted OnTowerTakeoverCompleted;

private:
	void UpdateTowerVisuals();
};
