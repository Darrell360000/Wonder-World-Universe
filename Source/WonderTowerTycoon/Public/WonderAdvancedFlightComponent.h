#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WonderAdvancedFlightComponent.generated.h"

class UPrimitiveComponent;

UENUM(BlueprintType)
enum class EFlightVehicleType : uint8
{
	HoverCar     UMETA(DisplayName = "Street Hovercraft"),
	JetFighter   UMETA(DisplayName = "Supersonic Interceptor"),
	HeavyGunship UMETA(DisplayName = "B-11 Combat Gunship"),
	Amphibious   UMETA(DisplayName = "Water/Street Amphibious")
};

UCLASS(ClassGroup=(WonderWorld), meta=(BlueprintSpawnableComponent))
class WONDERTOWERTYCOON_API UWonderAdvancedFlightComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWonderAdvancedFlightComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Flight")
	EFlightVehicleType VehicleType = EFlightVehicleType::HoverCar;

	/* --- ANALOG FLIGHT INPUTS (HOTAS / GAMEPAD READY) --- */
	UFUNCTION(BlueprintCallable, Category = "Wonder|Flight - Inputs")
	void SetThrottleInput(float Value); // -1.0 to 1.0 (Reverse to Afterburner)

	UFUNCTION(BlueprintCallable, Category = "Wonder|Flight - Inputs")
	void SetFlightSteering(float Pitch, float Roll, float Yaw);

	/* --- PERFORMANCE LIMITS --- */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Flight - Stats")
	float MaxFlightSpeed = 7500.0f; // ~165 mph

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Flight - Stats")
	float AfterburnerMultiplier = 1.6f;

private:
	UPROPERTY()
	UPrimitiveComponent* RootPrim;

	float TargetThrottle = 0.0f;
	FVector CurrentFlightTorques = FVector::ZeroVector;

	void CalculateAerodynamics(float DeltaTime);
};
