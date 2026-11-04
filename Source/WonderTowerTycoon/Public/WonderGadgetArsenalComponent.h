#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WonderGadgetArsenalComponent.generated.h"

UENUM(BlueprintType)
enum class EGadgetMode : uint8
{
	KineticHand UMETA(DisplayName = "Gravity Gun & Web Tether"),
	LaserCutter UMETA(DisplayName = "Omni-Tool & Watch Laser"),
	StealthCamo UMETA(DisplayName = "Optical Camouflage"),
	ARIVisor    UMETA(DisplayName = "ARI Detective Analysis")
};

UCLASS(ClassGroup=(WonderWorld), meta=(BlueprintSpawnableComponent))
class WONDERTOWERTYCOON_API UWonderGadgetArsenalComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWonderGadgetArsenalComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/* --- GADGET MODE SELECTION --- */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Gadgets")
	EGadgetMode ActiveMode = EGadgetMode::KineticHand;

	UFUNCTION(BlueprintCallable, Category = "Wonder|Gadgets")
	void CycleGadgetMode();

	/* --- GADGET ACTIONS --- */
	UFUNCTION(BlueprintCallable, Category = "Wonder|Gadgets - Actions")
	void TriggerPrimaryGadget();

	UFUNCTION(BlueprintCallable, Category = "Wonder|Gadgets - Actions")
	void ReleasePrimaryGadget();

	/* --- STEALTH CAMOUFLAGE STATE --- */
	UPROPERTY(BlueprintReadOnly, Category = "Wonder|Gadgets - Camo")
	bool bIsCloaked = false;

	/* --- ARI VISOR STATE --- */
	UPROPERTY(BlueprintReadOnly, Category = "Wonder|Gadgets - Visor")
	bool bVisorActive = false;

private:
	UPROPERTY()
	AActor* HeldPhysicsObject = nullptr;

	void UpdateCloakMaterial(bool bEnable);
};
