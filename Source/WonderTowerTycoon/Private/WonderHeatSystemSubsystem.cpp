#include "WonderHeatSystemSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UWonderHeatSystemSubsystem::AddCrimeHeat(float Amount)
{
	HeatScore = FMath::Clamp(HeatScore + Amount, 0.0f, 500.0f);
	UpdateDispatchLevel();
}

void UWonderHeatSystemSubsystem::CoolDownHeat(float DeltaTime)
{
	if (HeatScore > 0.0f)
	{
		HeatScore = FMath::Max(0.0f, HeatScore - (DeltaTime * 5.0f));
		UpdateDispatchLevel();
	}
}

void UWonderHeatSystemSubsystem::UpdateDispatchLevel()
{
	int32 OldLevel = CurrentHeatLevel;
	CurrentHeatLevel = FMath::Clamp(FMath::FloorToInt(HeatScore / 100.0f), 0, 5);

	if (OldLevel != CurrentHeatLevel)
	{
		OnHeatLevelChanged.Broadcast(CurrentHeatLevel);
	}
}
