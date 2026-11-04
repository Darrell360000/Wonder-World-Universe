#include "WonderVehicleInteractComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

UWonderVehicleInteractComponent::UWonderVehicleInteractComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

APawn* UWonderVehicleInteractComponent::FindBestNearbyVehicle(APawn* PlayerPawn)
{
	if (!PlayerPawn) return nullptr;

	TArray<AActor*> FoundVehicles;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("DrivableVehicle"), FoundVehicles);

	APawn* ClosestVehicle = nullptr;
	float ClosestDistanceSq = InteractionRadius * InteractionRadius;

	for (AActor* Actor : FoundVehicles)
	{
		APawn* PotentialPawn = Cast<APawn>(Actor);
		if (PotentialPawn && PotentialPawn != PlayerPawn)
		{
			float DistSq = FVector::DistSquared(PlayerPawn->GetActorLocation(), PotentialPawn->GetActorLocation());
			if (DistSq < ClosestDistanceSq)
			{
				ClosestDistanceSq = DistSq;
				ClosestVehicle = PotentialPawn;
			}
		}
	}
	return ClosestVehicle;
}

bool UWonderVehicleInteractComponent::TryEnterNearestVehicle(APawn* PlayerPawn)
{
	APawn* Vehicle = FindBestNearbyVehicle(PlayerPawn);
	if (!Vehicle) return false;

	APlayerController* PC = Cast<APlayerController>(PlayerPawn->GetController());
	if (!PC) return false;

	// Hide character and attach to vehicle
	PlayerPawn->SetActorHiddenInGame(true);
	PlayerPawn->SetActorEnableCollision(false);
	PlayerPawn->AttachToActor(Vehicle, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	// Possess vehicle
	PC->UnPossess();
	PC->Possess(Vehicle);
	CurrentOccupiedVehicle = Vehicle;
	return true;
}

void UWonderVehicleInteractComponent::ExitCurrentVehicle(APawn* VehiclePawn, bool bHighSpeedEject)
{
	if (!VehiclePawn) return;

	APlayerController* PC = Cast<APlayerController>(VehiclePawn->GetController());
	if (!PC) return;

	// Unpossess vehicle and restore character to driver side
	TArray<AActor*> AttachedActors;
	VehiclePawn->GetAttachedActors(AttachedActors);

	for (AActor* Attached : AttachedActors)
	{
		APawn* CharacterPawn = Cast<APawn>(Attached);
		if (CharacterPawn)
		{
			CharacterPawn->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			CharacterPawn->SetActorHiddenInGame(false);
			CharacterPawn->SetActorEnableCollision(true);

			FVector ExitOffset = VehiclePawn->GetActorRightVector() * -180.f + FVector(0.f, 0.f, 50.f);
			CharacterPawn->SetActorLocation(VehiclePawn->GetActorLocation() + ExitOffset);

			PC->UnPossess();
			PC->Possess(CharacterPawn);
			CurrentOccupiedVehicle = nullptr;
			break;
		}
	}
}
