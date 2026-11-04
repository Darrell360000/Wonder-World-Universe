#include "WonderCameraDirectorComponent.h"
#include "GameFramework/Character.h"

UWonderCameraDirectorComponent::UWonderCameraDirectorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UWonderCameraDirectorComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (Owner)
	{
		SpringArmComp = Owner->FindComponentByClass<USpringArmComponent>();
		CameraComp = Owner->FindComponentByClass<UCameraComponent>();
	}

	TargetArmLength = ThirdPersonArmLength;
	TargetSocketOffset = ThirdPersonSocketOffset;
}

void UWonderCameraDirectorComponent::TogglePerspective()
{
	if (CurrentPerspective == ECameraPerspective::ThirdPerson)
	{
		SetPerspective(ECameraPerspective::FirstPerson);
	}
	else
	{
		SetPerspective(ECameraPerspective::ThirdPerson);
	}
}

void UWonderCameraDirectorComponent::SetPerspective(ECameraPerspective NewPerspective)
{
	CurrentPerspective = NewPerspective;

	if (CurrentPerspective == ECameraPerspective::FirstPerson)
	{
		TargetArmLength = 0.0f;
		TargetSocketOffset = FirstPersonSocketOffset;
	}
	else
	{
		TargetArmLength = ThirdPersonArmLength;
		TargetSocketOffset = ThirdPersonSocketOffset;
	}

	UpdateMeshClipping();
}

void UWonderCameraDirectorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (SpringArmComp)
	{
		// Smooth interpolation between 3rd and 1st person
		SpringArmComp->TargetArmLength = FMath::FInterpTo(SpringArmComp->TargetArmLength, TargetArmLength, DeltaTime, CameraTransitionSpeed);
		SpringArmComp->SocketOffset = FMath::VInterpTo(SpringArmComp->SocketOffset, TargetSocketOffset, DeltaTime, CameraTransitionSpeed);
	}
}

void UWonderCameraDirectorComponent::UpdateMeshClipping()
{
	ACharacter* Char = Cast<ACharacter>(GetOwner());
	if (Char && Char->GetMesh())
	{
		// Hide head/hair in 1st person to prevent camera clipping; keep arms visible
		if (CurrentPerspective == ECameraPerspective::FirstPerson)
		{
			Char->GetMesh()->SetOwnerNoSee(false);
			Char->bUseControllerRotationYaw = true; // Tight FPS mouse look
		}
		else
		{
			Char->bUseControllerRotationYaw = false; // Free-look 3rd person
		}
	}
}
