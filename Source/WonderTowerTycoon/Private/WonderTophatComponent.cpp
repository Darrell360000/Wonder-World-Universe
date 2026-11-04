#include "WonderTophatComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

UWonderTophatComponent::UWonderTophatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UWonderTophatComponent::BeginPlay()
{
	Super::BeginPlay();
	UpdateHatModifiers();
}

void UWonderTophatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Aerodynamic Glide Logic
	if (bCanGlide)
	{
		ACharacter* Char = Cast<ACharacter>(GetOwner());
		if (Char && Char->GetCharacterMovement() && Char->GetCharacterMovement()->IsFalling())
		{
			FVector Vel = Char->GetCharacterMovement()->Velocity;
			if (Vel.Z < -200.f)
			{
				Char->GetCharacterMovement()->Velocity.Z = -200.f; // Terminal glide velocity
			}
		}
	}
}

void UWonderTophatComponent::EquipHat(ETophatType NewHat)
{
	EquippedHat = NewHat;
	UpdateHatModifiers();
}

void UWonderTophatComponent::UpdateHatModifiers()
{
	// Reset base stats
	DamageMultiplier = 1.0f;
	CashSiphonRadius = 0.0f;
	bCanGlide = false;

	switch (EquippedHat)
	{
	case ETophatType::Aerodynamic:
		bCanGlide = true;
		break;

	case ETophatType::RazorBrim:
		DamageMultiplier = 1.35f;
		break;

	case ETophatType::Syndicate:
		CashSiphonRadius = 1500.0f; // 15-meter magnet
		break;

	case ETophatType::CursedOverlord:
		DamageMultiplier = 3.0f; // 3x Glass Cannon
		break;

	default:
		break;
	}
}

void UWonderTophatComponent::UsePrimaryHatAbility()
{
	ACharacter* Char = Cast<ACharacter>(GetOwner());
	if (!Char) return;

	if (EquippedHat == ETophatType::RazorBrim)
	{
		// Throw hat discus projectile
		FVector SpawnLoc = Char->GetActorLocation() + Char->GetActorForwardVector() * 100.f;
		FRotator SpawnRot = Char->GetActorRotation();
		if (GetWorld())
		{
			GetWorld()->SpawnActor<AActor>(AActor::StaticClass(), SpawnLoc, SpawnRot);
		}
	}
	else if (EquippedHat == ETophatType::Aerodynamic)
	{
		// Downward Anvil Stomp
		if (Char->GetCharacterMovement())
		{
			Char->GetCharacterMovement()->Velocity = FVector(0.f, 0.f, -2500.f);
		}
	}
}

void UWonderTophatComponent::UseSecondaryHatAbility()
{
	// Recall hat back to head
}
