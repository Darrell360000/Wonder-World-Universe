#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WonderVehicleInteractComponent.generated.h"

UCLASS(ClassGroup=(WonderWorld), meta=(BlueprintSpawnableComponent))
class WONDERTOWERTYCOON_API UWonderVehicleInteractComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWonderVehicleInteractComponent();

	/* --- INTERACTION RADIUS --- */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Vehicle Interaction")
	float InteractionRadius = 350.0f;

	/* --- ENTER & EXIT LOGIC --- */
	UFUNCTION(BlueprintCallable, Category = "Wonder|Vehicle Interaction")
	bool TryEnterNearestVehicle(APawn* PlayerPawn);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Vehicle Interaction")
	void ExitCurrentVehicle(APawn* VehiclePawn, bool bHighSpeedEject = false);

	UPROPERTY(BlueprintReadOnly, Category = "Wonder|Vehicle Interaction")
	APawn* CurrentOccupiedVehicle = nullptr;

private:
	APawn* FindBestNearbyVehicle(APawn* PlayerPawn);
};
