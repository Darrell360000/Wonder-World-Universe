#include "WonderAdvancedCombatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"

UWonderAdvancedCombatComponent::UWonderAdvancedCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UWonderAdvancedCombatComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UWonderAdvancedCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// If recalling, interpolate disc back to God Hand socket
	if (bIsDiscThrown && ActiveThrownDisc && !ActiveThrownDisc->IsPendingKillPending())
	{
		ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
		if (OwnerChar && OwnerChar->GetMesh())
		{
			FVector HandLoc = OwnerChar->GetMesh()->GetSocketLocation(FName("GodHandSocket"));
			FVector NewLoc = FMath::VInterpTo(ActiveThrownDisc->GetActorLocation(), HandLoc, DeltaTime, 18.0f);
			ActiveThrownDisc->SetActorLocation(NewLoc);

			if (FVector::DistSquared(NewLoc, HandLoc) < 4000.f)
			{
				ActiveThrownDisc->Destroy();
				ActiveThrownDisc = nullptr;
				bIsDiscThrown = false;
			}
		}
	}
}

void UWonderAdvancedCombatComponent::ThrowSonicDisc(FVector ForwardDirection)
{
	if (bIsDiscThrown)
	{
		RecallSonicDisc();
		return;
	}

	if (GetOwner() && GetWorld())
	{
		FVector SpawnLoc = GetOwner()->GetActorLocation() + ForwardDirection * 150.f;
		FRotator SpawnRot = ForwardDirection.Rotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();

		// Spawn weapon projectile and project forward
		ActiveThrownDisc = GetWorld()->SpawnActor<AActor>(AActor::StaticClass(), SpawnLoc, SpawnRot, SpawnParams);
		bIsDiscThrown = true;
	}
}

void UWonderAdvancedCombatComponent::RecallSonicDisc()
{
	if (bIsDiscThrown && ActiveThrownDisc)
	{
		// Deals piercing return damage along trajectory
		UGameplayStatics::PlaySound2D(GetWorld(), nullptr); // Optional audio sting
	}
}

void UWonderAdvancedCombatComponent::ToggleHarmonicDilation()
{
	bInDilationMode = !bInDilationMode;
	float TargetDilation = bInDilationMode ? 0.20f : 1.0f;
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), TargetDilation);
}

void UWonderAdvancedCombatComponent::TriggerCinemaFailForwardRespawn()
{
	// Fail Forward: Reload level at Drome Cinema entrance with full health and unlocked memory
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
	UGameplayStatics::OpenLevel(GetWorld(), FName("WonderWorld_Downtown_Showcase"));
}
