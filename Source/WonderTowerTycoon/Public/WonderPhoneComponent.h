#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Blueprint/UserWidget.h"
#include "WonderPhoneComponent.generated.h"

UCLASS(ClassGroup=(WonderWorld), meta=(BlueprintSpawnableComponent))
class WONDERTOWERTYCOON_API UWonderPhoneComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWonderPhoneComponent();

protected:
	virtual void BeginPlay() override;

public:	
	/* --- PHONE STATE & TOGGLE --- */
	UFUNCTION(BlueprintCallable, Category = "Wonder|Phone")
	void TogglePhone();

	UPROPERTY(BlueprintReadOnly, Category = "Wonder|Phone")
	bool bPhoneOpen = false;

	/* --- UMG WIDGET REFERENCE --- */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Phone - UI")
	TSubclassOf<UUserWidget> PhoneWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category = "Wonder|Phone - UI")
	UUserWidget* ActivePhoneWidget;

	/* --- CORE APP HOOKS --- */
	UFUNCTION(BlueprintCallable, Category = "Wonder|Phone - Apps")
	void RemoteSummonHoverVehicle();

	UFUNCTION(BlueprintCallable, Category = "Wonder|Phone - Apps")
	void TriggerGridOvercharge();

	/* --- 3-TIER CURRENCY DATA --- */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Economy")
	int64 PhysicalCash = 25000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Economy")
	int64 CorporateCredit = 1200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Economy")
	float NetPowerMegaWatts = 450.5f;
};
