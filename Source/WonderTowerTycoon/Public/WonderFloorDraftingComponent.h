#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WonderFloorDraftingComponent.generated.h"

UENUM(BlueprintType)
enum class EFloorRoomType : uint8
{
	ServerVault       UMETA(DisplayName = "High-Voltage Server Vault (Net-Power Siphon)"),
	SecurityBarracks  UMETA(DisplayName = "Syndicate Armory (Heavy Guard Combat)"),
	ExecutiveOffice   UMETA(DisplayName = "Corporate Records (Cipher Intel & Cash)"),
	PenthouseBoss     UMETA(DisplayName = "Executive Penthouse Suite (CEO Boss Fight)"),
	RooftopHelipad    UMETA(DisplayName = "Helipad Landing Pad (Wonder Conversion)")
};

USTRUCT(BlueprintType)
struct FDraftableFloorNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 FloorLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EFloorRoomType RoomType = EFloorRoomType::SecurityBarracks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsCleared = false;
};

UCLASS(ClassGroup=(WonderWorld), meta=(BlueprintSpawnableComponent))
class WONDERTOWERTYCOON_API UWonderFloorDraftingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWonderFloorDraftingComponent();

	/* --- FLOOR DRAFTING API --- */
	UFUNCTION(BlueprintCallable, Category = "Wonder|Tower Draft")
	void GenerateDraftChoices(int32 CurrentFloor, TArray<EFloorRoomType>& OutChoices);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Tower Draft")
	void SelectAndBuildFloor(int32 FloorNumber, EFloorRoomType SelectedType);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Tower Draft")
	void MarkFloorCleared(int32 FloorNumber);

	UPROPERTY(BlueprintReadOnly, Category = "Wonder|Tower Draft")
	TArray<FDraftableFloorNode> ActiveTowerFloors;

	UPROPERTY(BlueprintReadOnly, Category = "Wonder|Tower Draft")
	int32 CurrentAscentFloor = 1;
};
