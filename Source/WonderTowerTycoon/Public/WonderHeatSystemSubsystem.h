#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "WonderHeatSystemSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHeatLevelChanged, int32, NewHeatLevel);

UCLASS()
class WONDERTOWERTYCOON_API UWonderHeatSystemSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	/* --- WANTED HEAT RATING (0 TO 5 STARS) --- */
	UPROPERTY(BlueprintReadOnly, Category = "Wonder|Heat")
	int32 CurrentHeatLevel = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Wonder|Heat")
	float HeatScore = 0.0f;

	UFUNCTION(BlueprintCallable, Category = "Wonder|Heat")
	void AddCrimeHeat(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Heat")
	void CoolDownHeat(float DeltaTime);

	UPROPERTY(BlueprintAssignable, Category = "Wonder|Heat")
	FOnHeatLevelChanged OnHeatLevelChanged;

private:
	void UpdateDispatchLevel();
};
