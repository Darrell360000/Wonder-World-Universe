#include "WonderGadgetArsenalComponent.h"
#include "GameFramework/Character.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

UWonderGadgetArsenalComponent::UWonderGadgetArsenalComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UWonderGadgetArsenalComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UWonderGadgetArsenalComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Gravity Gun Hold: pull held physics object toward God Hand
	if (HeldPhysicsObject && GetOwner())
	{
		UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(HeldPhysicsObject->GetRootComponent());
		if (Prim && Prim->IsSimulatingPhysics())
		{
			FVector TargetLoc = GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * 250.f + FVector(0.f, 0.f, 50.f);
			FVector Direction = (TargetLoc - HeldPhysicsObject->GetActorLocation());
			Prim->SetPhysicsLinearVelocity(Direction * 10.0f);
		}
	}
}

void UWonderGadgetArsenalComponent::CycleGadgetMode()
{
	uint8 Current = static_cast<uint8>(ActiveMode);
	ActiveMode = static_cast<EGadgetMode>((Current + 1) % 4);
}

void UWonderGadgetArsenalComponent::TriggerPrimaryGadget()
{
	ACharacter* Char = Cast<ACharacter>(GetOwner());
	if (!Char) return;

	switch (ActiveMode)
	{
	case EGadgetMode::KineticHand:
	{
		// Gravity Gun: Line trace to grab physics prop
		if (GetWorld())
		{
			FHitResult Hit;
			FVector Start = Char->GetActorLocation();
			FVector End = Start + Char->GetActorForwardVector() * 1200.f;
			if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_PhysicsBody))
			{
				if (Hit.GetActor())
				{
					HeldPhysicsObject = Hit.GetActor();
				}
			}
		}
		break;
	}
	case EGadgetMode::StealthCamo:
		bIsCloaked = !bIsCloaked;
		UpdateCloakMaterial(bIsCloaked);
		break;

	case EGadgetMode::ARIVisor:
		bVisorActive = !bVisorActive;
		// Enables post-process outline and conduit highlights
		break;

	case EGadgetMode::LaserCutter:
		// Emits high-intensity laser trace to breach locks/turrets
		break;
	}
}

void UWonderGadgetArsenalComponent::ReleasePrimaryGadget()
{
	if (ActiveMode == EGadgetMode::KineticHand && HeldPhysicsObject)
	{
		// Launch held physics object like a missile
		UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(HeldPhysicsObject->GetRootComponent());
		if (Prim && GetOwner())
		{
			Prim->AddImpulse(GetOwner()->GetActorForwardVector() * 3500.f, NAME_None, true);
		}
		HeldPhysicsObject = nullptr;
	}
}

void UWonderGadgetArsenalComponent::UpdateCloakMaterial(bool bEnable)
{
	ACharacter* Char = Cast<ACharacter>(GetOwner());
	if (Char && Char->GetMesh())
	{
		// Modulates material opacity / refraction for light-bending stealth
		Char->GetMesh()->SetScalarParameterValueOnMaterials(FName("CloakAlpha"), bEnable ? 0.08f : 1.0f);
	}
}
