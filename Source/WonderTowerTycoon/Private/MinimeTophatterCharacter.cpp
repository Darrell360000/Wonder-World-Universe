// Copyright Epic Games, Inc. All Rights Reserved.

#include "MinimeTophatterCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

AMinimeTophatterCharacter::AMinimeTophatterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(20.0f, 40.0f);
	SetActorScale3D(FVector(0.35f, 0.35f, 0.35f));

	MiniTopHatMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MiniTopHatMesh"));
	MiniTopHatMesh->SetupAttachment(GetMesh(), FName("head"));

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;

	bIsAttached = true;
	HoverFlightSpeed = 1200.0f;
}

void AMinimeTophatterCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AMinimeTophatterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMinimeTophatterCharacter::AttachToHost(ACharacter* HostCharacter, FName SocketName)
{
	if (!HostCharacter) return;

	CurrentHost = HostCharacter;
	bIsAttached = true;

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->DisableMovement();

	AttachToComponent(HostCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
	SetActorRelativeScale3D(FVector(0.35f, 0.35f, 0.35f));

	OnMinimeScoutingStateChanged.Broadcast(false);
}

void AMinimeTophatterCharacter::DetachToScout(FVector TargetLocation)
{
	bIsAttached = false;
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	GetCharacterMovement()->MaxFlySpeed = HoverFlightSpeed;

	OnMinimeScoutingStateChanged.Broadcast(true);
}

void AMinimeTophatterCharacter::ScanAndTagCopycats(float ScanRadius)
{
	TArray<AActor*> OverlappingActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	UKismetSystemLibrary::SphereOverlapActors(
		this,
		GetActorLocation(),
		ScanRadius,
		ObjectTypes,
		APawn::StaticClass(),
		TArray<AActor*>(),
		OverlappingActors
	);

	for (AActor* Actor : OverlappingActors)
	{
		if (Actor && Actor != this && Actor != CurrentHost.Get())
		{
			OnCopycatTargetTagged.Broadcast(Actor, Actor->GetActorLocation());
		}
	}
}
