#include "WonderWardrobeComponent.h"
#include "Kismet/KismetMathLibrary.h"

UWonderWardrobeComponent::UWonderWardrobeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	AvailableGarmentBoxes = 5;
	SalvagedScrap = 350;
}

void UWonderWardrobeComponent::BeginPlay()
{
	Super::BeginPlay();
	InitializeDefaultWardrobe();
}

void UWonderWardrobeComponent::InitializeDefaultWardrobe()
{
	// 1. Hat
	FWardrobeItem DefaultHat;
	DefaultHat.ItemID = FName("Hat_RazorBrim");
	DefaultHat.ItemName = TEXT("Razor-Brim Crimson Derby");
	DefaultHat.Slot = EWardrobeSlot::Hat;
	DefaultHat.RarityTier = 3;
	DefaultHat.StatModifier = 1.35f;
	DefaultHat.AbilityPerk = TEXT("Discus Throw & Ricochet Guard Break");
	EquippedOutfit.Add(EWardrobeSlot::Hat, DefaultHat);

	// 2. Jacket
	FWardrobeItem DefaultJacket;
	DefaultJacket.ItemID = FName("Jacket_KineticCoat");
	DefaultJacket.ItemName = TEXT("Kinetic Weaver Coat");
	DefaultJacket.Slot = EWardrobeSlot::Jacket;
	DefaultJacket.RarityTier = 2;
	DefaultJacket.StatModifier = 1.20f;
	DefaultJacket.AbilityPerk = TEXT("+20% Armor Rating & +1.0s Bio-Rally Window");
	EquippedOutfit.Add(EWardrobeSlot::Jacket, DefaultJacket);

	// 3. Gloves
	FWardrobeItem DefaultGloves;
	DefaultGloves.ItemID = FName("Gloves_GodHand");
	DefaultGloves.ItemName = TEXT("God Hand Gauntlets");
	DefaultGloves.Slot = EWardrobeSlot::Gloves;
	DefaultGloves.RarityTier = 3;
	DefaultGloves.StatModifier = 1.40f;
	DefaultGloves.AbilityPerk = TEXT("Zero-Point Magnetic Pull & +40% Punch Impact");
	EquippedOutfit.Add(EWardrobeSlot::Gloves, DefaultGloves);

	// 4. Pants
	FWardrobeItem DefaultPants;
	DefaultPants.ItemID = FName("Pants_ParkourSlacks");
	DefaultPants.ItemName = TEXT("Parkour Velocity Slacks");
	DefaultPants.Slot = EWardrobeSlot::Pants;
	DefaultPants.RarityTier = 2;
	DefaultPants.StatModifier = 1.15f;
	DefaultPants.AbilityPerk = TEXT("+15% Sprint Speed & Dodge-Roll Frame Reduction");
	EquippedOutfit.Add(EWardrobeSlot::Pants, DefaultPants);

	// 5. Shoes
	FWardrobeItem DefaultShoes;
	DefaultShoes.ItemID = FName("Shoes_MagBoots");
	DefaultShoes.ItemName = TEXT("Rail-Grind Mag-Boots");
	DefaultShoes.Slot = EWardrobeSlot::Shoes;
	DefaultShoes.RarityTier = 3;
	DefaultShoes.StatModifier = 1.50f;
	DefaultShoes.AbilityPerk = TEXT("Locks onto light-rail tracks at 65 m/s");
	EquippedOutfit.Add(EWardrobeSlot::Shoes, DefaultShoes);

	// 6. Backpack
	FWardrobeItem DefaultBackpack;
	DefaultBackpack.ItemID = FName("Backpack_ThrusterRig");
	DefaultBackpack.ItemName = TEXT("Thruster Jet-Rig");
	DefaultBackpack.Slot = EWardrobeSlot::Backpack;
	DefaultBackpack.RarityTier = 3;
	DefaultBackpack.StatModifier = 1.30f;
	DefaultBackpack.AbilityPerk = TEXT("Vertical hover turbine boost & altitude reserve");
	EquippedOutfit.Add(EWardrobeSlot::Backpack, DefaultBackpack);
}

bool UWonderWardrobeComponent::EquipItem(const FWardrobeItem& NewItem)
{
	EquippedOutfit.Add(NewItem.Slot, NewItem);
	OnWardrobeSlotEquipped.Broadcast(NewItem.Slot, NewItem);
	return true;
}

bool UWonderWardrobeComponent::UnequipSlot(EWardrobeSlot Slot)
{
	if (EquippedOutfit.Contains(Slot))
	{
		EquippedOutfit.Remove(Slot);
		return true;
	}
	return false;
}

bool UWonderWardrobeComponent::GetEquippedItem(EWardrobeSlot Slot, FWardrobeItem& OutItem) const
{
	if (const FWardrobeItem* Found = EquippedOutfit.Find(Slot))
	{
		OutItem = *Found;
		return true;
	}
	return false;
}

float UWonderWardrobeComponent::GetTotalStatMultiplier(EWardrobeSlot Slot) const
{
	if (const FWardrobeItem* Found = EquippedOutfit.Find(Slot))
	{
		return Found->StatModifier;
	}
	return 1.0f;
}

bool UWonderWardrobeComponent::CraftWithDressbot(FWardrobeItem& OutCraftedItem)
{
	const int32 ScrapCost = 50;
	if (AvailableGarmentBoxes < 1 || SalvagedScrap < ScrapCost)
	{
		return false;
	}

	AvailableGarmentBoxes -= 1;
	SalvagedScrap -= ScrapCost;

	// Curated Dressbot Recipe Pool (Including Scrap Mechanic Prestige Hats)
	static const TArray<FWardrobeItem> DressbotItemPool = {
		// Prestige Hats
		{ FName("Hat_GoldenArchitect"), TEXT("Crown of the Master Architect"), EWardrobeSlot::Hat, 5, 2.00f, TEXT("24k Golden Aura & 2x District Tax Yields") },
		{ FName("Hat_DuckieDampener"), TEXT("Rubber Duckie Sound Dampener"), EWardrobeSlot::Hat, 3, 1.25f, TEXT("Negates fall damage & dampens engine noise on 85 BPM beat") },
		{ FName("Hat_SubterraneanCommando"), TEXT("Subterranean Commando Cap"), EWardrobeSlot::Hat, 3, 1.20f, TEXT("+20% ballistic protection in underground subways") },
		{ FName("Hat_WelderVisor"), TEXT("Industrial Plasma Visor"), EWardrobeSlot::Hat, 4, 1.35f, TEXT("Flashbang & laser immunity") },
		{ FName("Hat_NitroHelmet"), TEXT("Nitro Crash Helmet"), EWardrobeSlot::Hat, 4, 1.40f, TEXT("Triggers explosive shockwave on hover vehicle bails") },
		{ FName("Hat_SpacehikerCowl"), TEXT("Orbital Vacuum Cowl"), EWardrobeSlot::Hat, 5, 1.60f, TEXT("Immunity to toxic gas blooms and low-pressure altitude zones") },
		
		// Body Pieces
		{ FName("Jacket_ResonanceTrench"), TEXT("Resonance Trench Coat"), EWardrobeSlot::Jacket, 4, 1.45f, TEXT("+45% Armor & Siphons 10% Kinetic Damage to Net-Power") },
		{ FName("Gloves_ZeroPointGauntlets"), TEXT("Zero-Point God Hand Gauntlets"), EWardrobeSlot::Gloves, 4, 1.60f, TEXT("+60% Kinetic Launch Speed for Cars and Dumpsters") },
		{ FName("Pants_AcousticCargo"), TEXT("Acoustic Weave Parkour Pants"), EWardrobeSlot::Pants, 3, 1.30f, TEXT("+30% Slide Speed & Zero Fall Fatigue") },
		{ FName("Shoes_OverclockedMagGreaves"), TEXT("Overclocked Mag-Greaves"), EWardrobeSlot::Shoes, 4, 1.70f, TEXT("Light-rail grinding speeds up to 90 m/s with spark discharge") },
		{ FName("Backpack_StratosphereTurbine"), TEXT("Stratosphere Thruster Jet-Rig"), EWardrobeSlot::Backpack, 5, 1.85f, TEXT("Doubles vertical altitude reserve and enables mid-air afterburner") }
	};

	const int32 RandomIndex = FMath::RandRange(0, DressbotItemPool.Num() - 1);
	OutCraftedItem = DressbotItemPool[RandomIndex];

	// Auto-equip crafted piece
	EquipItem(OutCraftedItem);

	OnDressbotCraftSuccess.Broadcast(OutCraftedItem);
	return true;
}
