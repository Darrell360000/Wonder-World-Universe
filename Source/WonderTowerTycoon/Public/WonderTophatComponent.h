#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WonderTophatComponent.generated.h"

class UStaticMeshComponent;

UENUM(BlueprintType)
enum class ETophatType : uint8
{
	None           UMETA(DisplayName = "No Hat"),
	Aerodynamic    UMETA(DisplayName = "Aerodynamic Stovepipe (Mobility)"),
	RazorBrim      UMETA(DisplayName = "Razor-Brim Crimson Derby (Combat)"),
	Syndicate      UMETA(DisplayName = "Syndicate Ledger Hat (Utility)"),
	CursedOverlord UMETA(DisplayName = "Overlord Gilded Crown (Cursed)")
};

UCLASS(ClassGroup=(WonderWorld), meta=(BlueprintSpawnableComponent))
class WONDERTOWERTYCOON_API UWonderTophatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWonderTophatComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/* --- CURRENT EQUIPPED HAT --- */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Tophat")
	ETophatType EquippedHat = ETophatType::RazorBrim;

	UFUNCTION(BlueprintCallable, Category = "Wonder|Tophat")
	void EquipHat(ETophatType NewHat);

	/* --- ABILITY TRIGGERS --- */
	UFUNCTION(BlueprintCallable, Category = "Wonder|Tophat - Actions")
	void UsePrimaryHatAbility();

	UFUNCTION(BlueprintCallable, Category = "Wonder|Tophat - Actions")
	void UseSecondaryHatAbility();

	/* --- COMBAT STAT MULTIPLIERS --- */
	UPROPERTY(BlueprintReadOnly, Category = "Wonder|Tophat - Stats")
	float DamageMultiplier = 1.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Wonder|Tophat - Stats")
	float CashSiphonRadius = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Wonder|Tophat - Stats")
	bool bCanGlide = false;

private:
	UPROPERTY()
	UStaticMeshComponent* HatMeshComponent;

	void UpdateHatModifiers();
};
