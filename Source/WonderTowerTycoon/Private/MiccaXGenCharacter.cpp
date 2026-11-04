#include "MiccaXGenCharacter.h"
#include "WonderMermaidMagicComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AMiccaXGenCharacter::AMiccaXGenCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->bOrientRotationToMovement = true;
		MoveComp->RotationRate = FRotator(0.0f, 600.0f, 0.0f);
		MoveComp->JumpZVelocity = 700.0f;
		MoveComp->AirControl = 0.45f;
		MoveComp->MaxWalkSpeed = 620.0f;
		MoveComp->MaxSwimSpeed = 1600.0f; // 80 mph Hydro-Glide in water/canals
	}

	MermaidMagicComponent = CreateDefaultSubobject<UWonderMermaidMagicComponent>(TEXT("MermaidMagicComponent"));
}
