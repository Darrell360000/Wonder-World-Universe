#include "WonderPhoneComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

UWonderPhoneComponent::UWonderPhoneComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UWonderPhoneComponent::BeginPlay()
{
	Super::BeginPlay();

	if (PhoneWidgetClass)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PC)
		{
			ActivePhoneWidget = CreateWidget<UUserWidget>(PC, PhoneWidgetClass);
		}
	}
}

void UWonderPhoneComponent::TogglePhone()
{
	if (!ActivePhoneWidget) return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC) return;

	bPhoneOpen = !bPhoneOpen;

	if (bPhoneOpen)
	{
		ActivePhoneWidget->AddToViewport(50);
		PC->SetInputMode(FInputModeGameAndUI().SetWidgetToFocus(ActivePhoneWidget->TakeWidget()));
		PC->bShowMouseCursor = true;
	}
	else
	{
		ActivePhoneWidget->RemoveFromParent();
		PC->SetInputMode(FInputModeGameOnly());
		PC->bShowMouseCursor = false;
	}
}

void UWonderPhoneComponent::RemoteSummonHoverVehicle()
{
	TArray<AActor*> Vehicles;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("DrivableVehicle"), Vehicles);

	if (Vehicles.Num() > 0 && GetOwner())
	{
		FVector SpawnLoc = GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * 400.f + FVector(0.f, 0.f, 80.f);
		Vehicles[0]->SetActorLocation(SpawnLoc);
	}
}

void UWonderPhoneComponent::TriggerGridOvercharge()
{
	if (NetPowerMegaWatts >= 50.0f)
	{
		NetPowerMegaWatts -= 50.0f;
		// Triggers global time dilation or localized blackout
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.35f);
	}
}
