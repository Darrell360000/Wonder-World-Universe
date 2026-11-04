#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WonderWardrobeComponent.generated.h"

UENUM(BlueprintType)
enum class EWardrobeSlot : uint8
{
	Hat      UMETA(DisplayName = "Hat (Tophatter Gear)"),
	Jacket   UMETA(DisplayName = "Jacket (Kinetic Coat)"),
	Gloves   UMETA(DisplayName = "Gloves (God Hand Gauntlets)"),
	Pants    UMETA(DisplayName = "Pants (Parkour Slacks)"),
	Shoes    UMETA(DisplayName = "Shoes (Rail-Grind Mag-Boots)"),
	Backpack UMETA(DisplayName = "Backpack (Thruster Jet-Rig)")
};

USTRUCT(BlueprintType)
struct WONDERTOWERTYCOON_API FWardrobeItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Wardrobe")
	FName ItemID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Wardrobe")
	FString ItemName = TEXT("Standard Streetwear");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Wardrobe")
	EWardrobeSlot Slot = EWardrobeSlot::Hat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Wardrobe")
	int32 RarityTier = 1; // 1: Common, 2: Rare, 3: Epic, 4: Legendary, 5: Prestige

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Wardrobe")
	float StatModifier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Wardrobe")
	FString AbilityPerk = TEXT("None");
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWardrobeSlotEquipped, EWardrobeSlot, Slot, const FWardrobeItem&, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDressbotCraftSuccess, const FWardrobeItem&, CraftedItem);

UCLASS(ClassGroup=(WonderWorld), meta=(BlueprintSpawnableComponent))
class WONDERTOWERTYCOON_API UWonderWardrobeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWonderWardrobeComponent();

protected:
	virtual void BeginPlay() override;

public:	
	/* --- 6-SLOT EQUIPPED WARDROBE --- */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wonder|Wardrobe")
	TMap<EWardrobeSlot, FWardrobeItem> EquippedOutfit;

	UFUNCTION(BlueprintCallable, Category = "Wonder|Wardrobe")
	bool EquipItem(const FWardrobeItem& NewItem);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Wardrobe")
	bool UnequipSlot(EWardrobeSlot Slot);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Wardrobe")
	bool GetEquippedItem(EWardrobeSlot Slot, FWardrobeItem& OutItem) const;

	/* --- WONDER DRESSBOT FABRICATOR --- */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Dressbot")
	int32 AvailableGarmentBoxes = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Dressbot")
	int32 SalvagedScrap = 350;

	UFUNCTION(BlueprintCallable, Category = "Wonder|Dressbot")
	bool CraftWithDressbot(FWardrobeItem& OutCraftedItem);

	/* --- COMBAT & STAT AGGREGATION --- */
	UFUNCTION(BlueprintPure, Category = "Wonder|Wardrobe - Stats")
	float GetTotalStatMultiplier(EWardrobeSlot Slot) const;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|Wardrobe|Events")
	FOnWardrobeSlotEquipped OnWardrobeSlotEquipped;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|Wardrobe|Events")
	FOnDressbotCraftSuccess OnDressbotCraftSuccess;

private:
	void InitializeDefaultWardrobe();
};
