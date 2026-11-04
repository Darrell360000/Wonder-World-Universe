// Copyright Darrell R. Lewis / Wonder World Trilogy. All Rights Reserved.

#include "WonderMermaidMagicComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Engine/OverlapResult.h"
#include "Components/PrimitiveComponent.h"

UWonderMermaidMagicComponent::UWonderMermaidMagicComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UWonderMermaidMagicComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UWonderMermaidMagicComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// If Maelstrom Singularity is active, apply rotational vortex and radial inward pull forces
	if (bMaelstromActive && GetWorld())
	{
		TArray<FOverlapResult> Overlaps;
		FCollisionShape Sphere = FCollisionShape::MakeSphere(MaelstromRadius);
		FCollisionQueryParams Params(SCENE_QUERY_STAT(MaelstromPull), false, GetOwner());

		if (GetWorld()->OverlapMultiByChannel(Overlaps, MaelstromLocation, FQuat::Identity, ECC_WorldDynamic, Sphere, Params))
		{
			for (const FOverlapResult& Overlap : Overlaps)
			{
				if (UPrimitiveComponent* Prim = Overlap.GetComponent())
				{
					if (Prim->IsSimulatingPhysics())
					{
						FVector ToCenter = MaelstromLocation - Prim->GetComponentLocation();
						float Dist = ToCenter.Size();
						if (Dist > 50.0f)
						{
							FVector PullDir = ToCenter.GetSafeNormal();
							// Tangential vortex swirl vector
							FVector Tangent = FVector::CrossProduct(PullDir, FVector::UpVector).GetSafeNormal();
							FVector VortexForce = (PullDir * MaelstromPullStrength + Tangent * (MaelstromPullStrength * 0.7f)) * (1.0f - (Dist / MaelstromRadius));
							Prim->AddForce(VortexForce * Prim->GetMass());
						}
					}
				}

				if (AActor* OverlapActor = Overlap.GetActor())
				{
					if (OverlapActor != GetOwner())
					{
						if (ACharacter* Char = Cast<ACharacter>(OverlapActor))
						{
							FVector PullVector = (MaelstromLocation - Char->GetActorLocation()).GetSafeNormal() * 800.0f;
							Char->LaunchCharacter(PullVector * DeltaTime * 30.0f, false, false);
						}
					}
				}
			}
		}
	}
}

void UWonderMermaidMagicComponent::TriggerHydroGeyser()
{
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor) return;

	FVector Origin = OwnerActor->GetActorLocation();
	ACharacter* OwnerChar = Cast<ACharacter>(OwnerActor);

	// 1. Launch owner character 80ft into the air
	if (OwnerChar && OwnerChar->GetCharacterMovement())
	{
		OwnerChar->LaunchCharacter(FVector(0.0f, 0.0f, GeyserLaunchForce), true, true);
	}

	// 2. Launch any surrounding syndicate enemies caught in the geyser plume
	if (UWorld* World = GetWorld())
	{
		TArray<FOverlapResult> Overlaps;
		FCollisionShape Cylinder = FCollisionShape::MakeSphere(400.0f);
		FCollisionQueryParams Params(SCENE_QUERY_STAT(HydroGeyserLaunch), false, OwnerActor);

		if (World->OverlapMultiByChannel(Overlaps, Origin, FQuat::Identity, ECC_Pawn, Cylinder, Params))
		{
			for (const FOverlapResult& Overlap : Overlaps)
			{
				if (ACharacter* SurroundingChar = Cast<ACharacter>(Overlap.GetActor()))
				{
					if (SurroundingChar != OwnerChar)
					{
						SurroundingChar->LaunchCharacter(FVector(0.0f, 0.0f, GeyserLaunchForce * 0.85f), false, true);
					}
				}
			}
		}
	}

	UGameplayStatics::PlaySound2D(GetWorld(), nullptr);
	OnHydroGeyserLaunched.Broadcast(GeyserLaunchForce);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.5f, FColor::Cyan,
			FString::Printf(TEXT("🌊 MERMAID MAGIC: High-Pressure Hydro-Geyser Erupted! (+%.0f Launch Force)"), GeyserLaunchForce));
	}
}

void UWonderMermaidMagicComponent::CastSirensResonanceSong()
{
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor || !GetWorld()) return;

	FVector Origin = OwnerActor->GetActorLocation();
	int32 StunnedCount = 0;

	TArray<FOverlapResult> Overlaps;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(SirensSongRadius);
	FCollisionQueryParams Params(SCENE_QUERY_STAT(SirensSong), false, OwnerActor);

	if (GetWorld()->OverlapMultiByChannel(Overlaps, Origin, FQuat::Identity, ECC_Pawn, Sphere, Params))
	{
		for (const FOverlapResult& Overlap : Overlaps)
		{
			if (ACharacter* TargetChar = Cast<ACharacter>(Overlap.GetActor()))
			{
				if (TargetChar != OwnerActor)
				{
					// Disorient syndicate divers and enforcers on the 85 BPM rhythm clock
					FVector PushBack = (TargetChar->GetActorLocation() - Origin).GetSafeNormal() * 1200.0f + FVector(0.f, 0.f, 350.f);
					TargetChar->LaunchCharacter(PushBack, true, true);
					StunnedCount++;
				}
			}
		}
	}

	UGameplayStatics::PlaySound2D(GetWorld(), nullptr);
	OnSirensSongCymaticHit.Broadcast(Origin, StunnedCount);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Turquoise,
			FString::Printf(TEXT("🎵 SIREN'S RESONANCE SONG (85 BPM): Sonar Cymatics Disoriented %d Enemies!"), StunnedCount));
	}
}

void UWonderMermaidMagicComponent::ActivateOceanicBarrier(bool bEnable)
{
	bOceanicBarrierActive = bEnable;
	OnOceanicBarrierStateChanged.Broadcast(bEnable);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, bEnable ? FColor::Cyan : FColor::Silver,
			bEnable ? TEXT("🛡️ OCEANIC BARRIER ACTIVE: Swirling Saltwater Vortex Deflecting Ballistics!")
			        : TEXT("🛡️ Oceanic Barrier Lowered."));
	}
}

void UWonderMermaidMagicComponent::SummonMaelstromSingularity()
{
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor || !GetWorld()) return;

	// Spawn whirlpool 800 units in front of player
	MaelstromLocation = OwnerActor->GetActorLocation() + OwnerActor->GetActorForwardVector() * 800.0f;
	bMaelstromActive = true;

	GetWorld()->GetTimerManager().SetTimer(MaelstromDurationTimer, [this]()
	{
		bMaelstromActive = false;
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor::Blue, TEXT("🌀 Maelstrom Singularity Dissipated."));
		}
	}, 6.0f, false);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue,
			TEXT("🌀 MAELSTROM SINGULARITY: Crushing Aquatic Whirlpool Collapsing Syndicate Vessels!"));
	}
}

void UWonderMermaidMagicComponent::ToggleSubmergedSwimming(bool bSubmerged)
{
	bIsSwimmingSubmerged = bSubmerged;

	if (ACharacter* OwnerChar = Cast<ACharacter>(GetOwner()))
	{
		if (UCharacterMovementComponent* MoveComp = OwnerChar->GetCharacterMovement())
		{
			if (bSubmerged)
			{
				MoveComp->SetMovementMode(MOVE_Swimming);
				MoveComp->MaxSwimSpeed = 1600.0f; // 80 mph Hydro-Glide
			}
			else
			{
				MoveComp->SetMovementMode(MOVE_Walking);
				MoveComp->MaxWalkSpeed = 600.0f;
			}
		}
	}
}
