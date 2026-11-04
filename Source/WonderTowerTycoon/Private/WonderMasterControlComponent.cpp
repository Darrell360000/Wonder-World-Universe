#include "WonderMasterControlComponent.h"
#include "WonderVehicleInteractComponent.h"
#include "WonderHoverPhysicsComponent.h"
#include "WonderOpenWorldSandbox.h"
#include "WonderAdvancedCombatComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PrimitiveComponent.h"
#include "Components/InputComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"

UWonderMasterControlComponent::UWonderMasterControlComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	ControlMode = EWonderControlMode::OnFoot;
	DefaultWalkSpeed = 600.0f;
	SprintSpeed = 1150.0f;
	ComboStrikeIndex = 0;
	bParryWindowActive = false;
	LastParryTime = -10.0f;
	bSlingshotEjectActive = false;
	bCymaticFuseHolding = false;
	TetheredPropActor = nullptr;
	BioRallyRecoverableHealth = 0.0f;
	BioRallyWindowRemaining = 0.0f;
	bBulletTimeActive = false;
	bLedgerHUDVisible = false;
}

void UWonderMasterControlComponent::BeginPlay()
{
	Super::BeginPlay();

	PawnOwner = Cast<APawn>(GetOwner());
	CharacterOwner = Cast<ACharacter>(GetOwner());

	if (CharacterOwner)
	{
		ControlMode = EWonderControlMode::OnFoot;
		if (UCharacterMovementComponent* MoveComp = CharacterOwner->GetCharacterMovement())
		{
			// Free 360 degree 3D locomotion
			MoveComp->bConstrainToPlane = false;
			MoveComp->MaxWalkSpeed = DefaultWalkSpeed;
		}
	}
	else if (PawnOwner && PawnOwner->IsA<AWonderHoverVehiclePawn>())
	{
		ControlMode = EWonderControlMode::InVehicle;
	}
}

void UWonderMasterControlComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Tick Bio-Rally recoverable health window (3.0s window)
	if (BioRallyWindowRemaining > 0.0f)
	{
		BioRallyWindowRemaining -= DeltaTime;
		if (BioRallyWindowRemaining <= 0.0f)
		{
			BioRallyRecoverableHealth = 0.0f;
			BioRallyWindowRemaining = 0.0f;
		}
	}

	// Update Cymatic Fuse magnetic tether
	if (bCymaticFuseHolding && TetheredPropActor && PawnOwner)
	{
		if (UPrimitiveComponent* PropPrim = Cast<UPrimitiveComponent>(TetheredPropActor->GetRootComponent()))
		{
			const FVector TargetLoc = PawnOwner->GetActorLocation() + PawnOwner->GetActorForwardVector() * 320.0f + FVector(0.0f, 0.0f, 120.0f);
			const FVector CurrentLoc = TetheredPropActor->GetActorLocation();
			const FVector PullForce = (TargetLoc - CurrentLoc) * 12.0f;
			PropPrim->SetPhysicsLinearVelocity(PullForce);
		}
	}
}

void UWonderMasterControlComponent::SetupControlBindings(UInputComponent* PlayerInputComponent)
{
	if (!PlayerInputComponent)
	{
		return;
	}

	// --- On-Foot Axes ---
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &UWonderMasterControlComponent::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &UWonderMasterControlComponent::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &UWonderMasterControlComponent::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &UWonderMasterControlComponent::LookUp);

	// --- On-Foot Actions ---
	PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Pressed, this, &UWonderMasterControlComponent::StartSprint);
	PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Released, this, &UWonderMasterControlComponent::StopSprint);

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &UWonderMasterControlComponent::HandleJump);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Released, this, &UWonderMasterControlComponent::StopJump);

	PlayerInputComponent->BindAction(TEXT("DodgeRoll"), IE_Pressed, this, &UWonderMasterControlComponent::HandleDodgeRoll);
	PlayerInputComponent->BindAction(TEXT("ComboStrike"), IE_Pressed, this, &UWonderMasterControlComponent::HandleComboStrike);
	PlayerInputComponent->BindAction(TEXT("ResonanceParry"), IE_Pressed, this, &UWonderMasterControlComponent::HandleResonanceParry);

	PlayerInputComponent->BindAction(TEXT("CymaticFuse"), IE_Pressed, this, &UWonderMasterControlComponent::StartCymaticFuse);
	PlayerInputComponent->BindAction(TEXT("CymaticFuse"), IE_Released, this, &UWonderMasterControlComponent::StopCymaticFuse);

	PlayerInputComponent->BindAction(TEXT("SubBassAscend"), IE_Pressed, this, &UWonderMasterControlComponent::HandleSubBassAscend);
	PlayerInputComponent->BindAction(TEXT("BulletTime"), IE_Pressed, this, &UWonderMasterControlComponent::HandleBulletTime);
	PlayerInputComponent->BindAction(TEXT("SonicDiscRecall"), IE_Pressed, this, &UWonderMasterControlComponent::HandleSonicDiscRecall);
	PlayerInputComponent->BindAction(TEXT("VehicleHijack"), IE_Pressed, this, &UWonderMasterControlComponent::HandleVehicleHijack);
	PlayerInputComponent->BindAction(TEXT("ToggleLedgerHUD"), IE_Pressed, this, &UWonderMasterControlComponent::ToggleLedgerHUD);

	// --- In-Vehicle Axes ---
	PlayerInputComponent->BindAxis(TEXT("VehicleSteer"), this, &UWonderMasterControlComponent::VehicleSteer);
	PlayerInputComponent->BindAxis(TEXT("VehicleThrottle"), this, &UWonderMasterControlComponent::VehicleThrottle);
	PlayerInputComponent->BindAxis(TEXT("VehicleBrake"), this, &UWonderMasterControlComponent::VehicleBrake);

	// --- In-Vehicle Actions ---
	PlayerInputComponent->BindAction(TEXT("VehicleToggleHover"), IE_Pressed, this, &UWonderMasterControlComponent::VehicleToggleHover);
	PlayerInputComponent->BindAction(TEXT("VehicleAscend"), IE_Pressed, this, &UWonderMasterControlComponent::VehicleAscend);
	PlayerInputComponent->BindAction(TEXT("VehicleDescend"), IE_Pressed, this, &UWonderMasterControlComponent::VehicleDescend);
	PlayerInputComponent->BindAction(TEXT("VehicleAfterburner"), IE_Pressed, this, &UWonderMasterControlComponent::VehicleAfterburner);
	PlayerInputComponent->BindAction(TEXT("VehicleEjectionVault"), IE_Pressed, this, &UWonderMasterControlComponent::VehicleEjectionVault);
	PlayerInputComponent->BindAction(TEXT("VehicleHandbrake"), IE_Pressed, this, &UWonderMasterControlComponent::VehicleHandbrake);
}

/* ==============================================================================
   ON-FOOT IMPLEMENTATIONS
   ============================================================================== */

void UWonderMasterControlComponent::MoveForward(float Value)
{
	if (ControlMode != EWonderControlMode::OnFoot || FMath::IsNearlyZero(Value) || !PawnOwner)
	{
		return;
	}

	const FRotator ControlRot = PawnOwner->GetControlRotation();
	const FRotator YawRot(0.0f, ControlRot.Yaw, 0.0f);
	const FVector Direction = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
	PawnOwner->AddMovementInput(Direction, Value);
}

void UWonderMasterControlComponent::MoveRight(float Value)
{
	if (ControlMode != EWonderControlMode::OnFoot || FMath::IsNearlyZero(Value) || !PawnOwner)
	{
		return;
	}

	const FRotator ControlRot = PawnOwner->GetControlRotation();
	const FRotator YawRot(0.0f, ControlRot.Yaw, 0.0f);
	const FVector Direction = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
	PawnOwner->AddMovementInput(Direction, Value);
}

void UWonderMasterControlComponent::Turn(float Value)
{
	if (FMath::IsNearlyZero(Value) || !PawnOwner)
	{
		return;
	}
	PawnOwner->AddControllerYawInput(Value);
}

void UWonderMasterControlComponent::LookUp(float Value)
{
	if (FMath::IsNearlyZero(Value) || !PawnOwner)
	{
		return;
	}
	PawnOwner->AddControllerPitchInput(Value);
}

void UWonderMasterControlComponent::StartSprint()
{
	if (CharacterOwner)
	{
		if (UCharacterMovementComponent* MoveComp = CharacterOwner->GetCharacterMovement())
		{
			MoveComp->MaxWalkSpeed = SprintSpeed;
		}
	}
}

void UWonderMasterControlComponent::StopSprint()
{
	if (CharacterOwner)
	{
		if (UCharacterMovementComponent* MoveComp = CharacterOwner->GetCharacterMovement())
		{
			MoveComp->MaxWalkSpeed = DefaultWalkSpeed;
		}
	}
}

void UWonderMasterControlComponent::HandleJump()
{
	if (!CharacterOwner)
	{
		return;
	}

	// FLOW COMBO 1: THE SLINGSHOT EJECT
	// If in mid-air and Slingshot Eject is primed, shoot kinetic grapple pendulum line
	if (bSlingshotEjectActive || CharacterOwner->GetCharacterMovement()->IsFalling())
	{
		bSlingshotEjectActive = false;

		// Convert forward speed into aerial pendulum launch
		const FVector SwingImpulse = CharacterOwner->GetActorForwardVector() * 1600.0f + FVector(0.0f, 0.0f, 750.0f);
		CharacterOwner->LaunchCharacter(SwingImpulse, true, true);
		OnSynergyComboExecuted.Broadcast(TEXT("TheSlingshotEject"));
		return;
	}

	CharacterOwner->Jump();
}

void UWonderMasterControlComponent::StopJump()
{
	if (CharacterOwner)
	{
		CharacterOwner->StopJumping();
	}
}

void UWonderMasterControlComponent::HandleDodgeRoll()
{
	if (!CharacterOwner)
	{
		return;
	}

	FVector DodgeDirection = CharacterOwner->GetLastMovementInputVector();
	if (DodgeDirection.IsNearlyZero())
	{
		DodgeDirection = CharacterOwner->GetActorForwardVector();
	}

	const FVector DodgeVelocity = DodgeDirection.GetSafeNormal() * 1100.0f + FVector(0.0f, 0.0f, 150.0f);
	CharacterOwner->LaunchCharacter(DodgeVelocity, true, true);
}

void UWonderMasterControlComponent::HandleComboStrike()
{
	const float CurrentTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;

	// FLOW COMBO 2: THE COUNTER-FINISHER
	// Pressing LMB within 0.4s of a parry triggers a cinematic takedown finisher
	if (CurrentTime - LastParryTime <= 0.4f)
	{
		LastParryTime = -10.0f;
		if (UWorld* World = GetWorld())
		{
			UGameplayStatics::SetGlobalTimeDilation(World, 0.15f);
			FTimerHandle UnusedHandle;
			World->GetTimerManager().SetTimer(UnusedHandle, [World]()
			{
				if (World)
				{
					UGameplayStatics::SetGlobalTimeDilation(World, 1.0f);
				}
			}, 0.25f, false);
		}
		OnSynergyComboExecuted.Broadcast(TEXT("CounterFinisher"));
		return;
	}

	// 3-Hit Punch / Kick combo string
	ComboStrikeIndex = (ComboStrikeIndex + 1) % 3;

	// FLOW COMBO 4: RALLY SIPHON
	// Landing strikes during the 3s bio-rally window refills depleted health
	if (BioRallyWindowRemaining > 0.0f && BioRallyRecoverableHealth > 0.0f)
	{
		const float HealAmount = FMath::Min(BioRallyRecoverableHealth, 25.0f);
		BioRallyRecoverableHealth -= HealAmount;
		OnBioRallyHealthRecovered.Broadcast(HealAmount);
	}
}

void UWonderMasterControlComponent::HandleResonanceParry()
{
	bParryWindowActive = true;
	LastParryTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;

	if (UWorld* World = GetWorld())
	{
		FTimerHandle ParryTimer;
		World->GetTimerManager().SetTimer(ParryTimer, [this]()
		{
			bParryWindowActive = false;
		}, 0.35f, false);
	}
}

void UWonderMasterControlComponent::StartCymaticFuse()
{
	bCymaticFuseHolding = true;

	if (!PawnOwner || !GetWorld())
	{
		return;
	}

	// Magnetically tether nearby movable physics prop
	FHitResult Hit;
	const FVector Start = PawnOwner->GetActorLocation();
	const FVector End = Start + PawnOwner->GetActorForwardVector() * 1200.0f;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(PawnOwner);

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldDynamic, QueryParams))
	{
		if (Hit.GetActor() && Hit.GetComponent() && Hit.GetComponent()->IsSimulatingPhysics())
		{
			TetheredPropActor = Hit.GetActor();
		}
	}
}

void UWonderMasterControlComponent::StopCymaticFuse()
{
	if (bCymaticFuseHolding && TetheredPropActor)
	{
		// FLOW COMBO 3: THE ORBITAL WRECKING BALL RELEASE
		if (UPrimitiveComponent* PropPrim = Cast<UPrimitiveComponent>(TetheredPropActor->GetRootComponent()))
		{
			const FVector HurlVelocity = PawnOwner ? PawnOwner->GetActorForwardVector() * 2200.0f : FVector(0.0f, 0.0f, -1200.0f);
			PropPrim->SetPhysicsLinearVelocity(HurlVelocity);
		}
		OnSynergyComboExecuted.Broadcast(TEXT("OrbitalWreckingBallReleased"));
	}

	bCymaticFuseHolding = false;
	TetheredPropActor = nullptr;
}

void UWonderMasterControlComponent::HandleSubBassAscend()
{
	if (!CharacterOwner || !GetWorld())
	{
		return;
	}

	// Trace upwards through solid ceilings, bridges, and roofs
	const FVector Start = CharacterOwner->GetActorLocation() + FVector(0.0f, 0.0f, 250.0f);
	const FVector End = Start + FVector(0.0f, 0.0f, 1500.0f);
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(CharacterOwner);

	if (GetWorld()->LineTraceSingleByChannel(Hit, End, Start, ECC_WorldStatic, Params))
	{
		const FVector TeleportLoc = Hit.Location + FVector(0.0f, 0.0f, 90.0f);
		CharacterOwner->SetActorLocation(TeleportLoc, false, nullptr, ETeleportType::TeleportPhysics);
	}
	else
	{
		CharacterOwner->LaunchCharacter(FVector(0.0f, 0.0f, 900.0f), false, true);
	}
}

void UWonderMasterControlComponent::HandleBulletTime()
{
	if (!GetWorld())
	{
		return;
	}

	if (PawnOwner)
	{
		if (UWonderAdvancedCombatComponent* CombatComp = PawnOwner->FindComponentByClass<UWonderAdvancedCombatComponent>())
		{
			CombatComp->ToggleHarmonicDilation();
			bBulletTimeActive = CombatComp->bInDilationMode;
			return;
		}
	}

	bBulletTimeActive = !bBulletTimeActive;
	const float TimeScale = bBulletTimeActive ? 0.25f : 1.0f;
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), TimeScale);
}

void UWonderMasterControlComponent::HandleSonicDiscRecall()
{
	if (PawnOwner)
	{
		if (UWonderAdvancedCombatComponent* CombatComp = PawnOwner->FindComponentByClass<UWonderAdvancedCombatComponent>())
		{
			if (CombatComp->bIsDiscThrown)
			{
				CombatComp->RecallSonicDisc();
			}
			else
			{
				CombatComp->ThrowSonicDisc(PawnOwner->GetActorForwardVector());
			}
		}
	}

	// Broadcast recall stinger
	OnSynergyComboExecuted.Broadcast(TEXT("SonicDiscRecall"));
}

void UWonderMasterControlComponent::HandleVehicleHijack()
{
	if (!PawnOwner)
	{
		return;
	}

	// Look for UWonderVehicleInteractComponent on the pawn or nearby
	UWonderVehicleInteractComponent* InteractComp = PawnOwner->FindComponentByClass<UWonderVehicleInteractComponent>();
	if (!InteractComp)
	{
		for (TActorIterator<AActor> It(GetWorld()); It; ++It)
		{
			if (UWonderVehicleInteractComponent* Found = It->FindComponentByClass<UWonderVehicleInteractComponent>())
			{
				InteractComp = Found;
				break;
			}
		}
	}

	if (InteractComp)
	{
		InteractComp->TryEnterNearestVehicle(PawnOwner);
	}
}

void UWonderMasterControlComponent::ToggleLedgerHUD()
{
	bLedgerHUDVisible = !bLedgerHUDVisible;
}

/* ==============================================================================
   IN-VEHICLE IMPLEMENTATIONS
   ============================================================================== */

void UWonderMasterControlComponent::VehicleSteer(float Value)
{
	if (ControlMode != EWonderControlMode::InVehicle || FMath::IsNearlyZero(Value))
	{
		return;
	}

	if (AWonderHoverVehiclePawn* HoverPawn = Cast<AWonderHoverVehiclePawn>(PawnOwner))
	{
		const float DeltaTime = GetWorld() ? GetWorld()->GetDeltaSeconds() : 0.016f;
		HoverPawn->ApplySteeringInput(Value, DeltaTime);
	}
}

void UWonderMasterControlComponent::VehicleThrottle(float Value)
{
	if (ControlMode != EWonderControlMode::InVehicle || FMath::IsNearlyZero(Value))
	{
		return;
	}

	if (AWonderHoverVehiclePawn* HoverPawn = Cast<AWonderHoverVehiclePawn>(PawnOwner))
	{
		const float DeltaTime = GetWorld() ? GetWorld()->GetDeltaSeconds() : 0.016f;
		HoverPawn->ApplyThrottleInput(Value, DeltaTime);
	}
}

void UWonderMasterControlComponent::VehicleBrake(float Value)
{
	if (ControlMode != EWonderControlMode::InVehicle || FMath::IsNearlyZero(Value))
	{
		return;
	}

	if (AWonderHoverVehiclePawn* HoverPawn = Cast<AWonderHoverVehiclePawn>(PawnOwner))
	{
		const float DeltaTime = GetWorld() ? GetWorld()->GetDeltaSeconds() : 0.016f;
		HoverPawn->ApplyThrottleInput(-Value, DeltaTime);
	}
}

void UWonderMasterControlComponent::VehicleToggleHover()
{
	if (AWonderHoverVehiclePawn* HoverPawn = Cast<AWonderHoverVehiclePawn>(PawnOwner))
	{
		HoverPawn->ToggleFlyingCarMode();
	}
	else if (PawnOwner)
	{
		if (UWonderHoverPhysicsComponent* HoverComp = PawnOwner->FindComponentByClass<UWonderHoverPhysicsComponent>())
		{
			HoverComp->ToggleHover();
		}
	}

	// FLOW COMBO 3: THE ORBITAL WRECKING BALL ENGAGEMENT
	if (bCymaticFuseHolding && TetheredPropActor)
	{
		if (UPrimitiveComponent* PropPrim = Cast<UPrimitiveComponent>(TetheredPropActor->GetRootComponent()))
		{
			PropPrim->AddImpulse(FVector(0.0f, 0.0f, 1800.0f), NAME_None, true);
		}
		OnSynergyComboExecuted.Broadcast(TEXT("OrbitalWreckingBallAscend"));
	}
}

void UWonderMasterControlComponent::VehicleAscend()
{
	if (AWonderHoverVehiclePawn* HoverPawn = Cast<AWonderHoverVehiclePawn>(PawnOwner))
	{
		const float DeltaTime = GetWorld() ? GetWorld()->GetDeltaSeconds() : 0.016f;
		HoverPawn->ApplyVerticalFlightInput(1.0f, DeltaTime);
	}
	else if (PawnOwner)
	{
		if (UWonderHoverPhysicsComponent* HoverComp = PawnOwner->FindComponentByClass<UWonderHoverPhysicsComponent>())
		{
			HoverComp->Ascend(1.0f);
		}
	}
}

void UWonderMasterControlComponent::VehicleDescend()
{
	if (AWonderHoverVehiclePawn* HoverPawn = Cast<AWonderHoverVehiclePawn>(PawnOwner))
	{
		const float DeltaTime = GetWorld() ? GetWorld()->GetDeltaSeconds() : 0.016f;
		HoverPawn->ApplyVerticalFlightInput(-1.0f, DeltaTime);
	}
	else if (PawnOwner)
	{
		if (UWonderHoverPhysicsComponent* HoverComp = PawnOwner->FindComponentByClass<UWonderHoverPhysicsComponent>())
		{
			HoverComp->Descend(1.0f);
		}
	}
}

void UWonderMasterControlComponent::VehicleAfterburner()
{
	if (AWonderHoverVehiclePawn* HoverPawn = Cast<AWonderHoverVehiclePawn>(PawnOwner))
	{
		const bool bNewState = !HoverPawn->bSonicAfterburnerActive;
		HoverPawn->SetSonicAfterburner(bNewState);
		OnSynergyComboExecuted.Broadcast(bNewState ? TEXT("AfterburnerEngaged") : TEXT("AfterburnerDisengaged"));
	}
}

void UWonderMasterControlComponent::VehicleEjectionVault()
{
	if (!PawnOwner)
	{
		return;
	}

	bSlingshotEjectActive = true;

	// Exit vehicle with high speed ejection
	UWonderVehicleInteractComponent* InteractComp = PawnOwner->FindComponentByClass<UWonderVehicleInteractComponent>();
	if (!InteractComp)
	{
		for (TActorIterator<AActor> It(GetWorld()); It; ++It)
		{
			if (UWonderVehicleInteractComponent* Found = It->FindComponentByClass<UWonderVehicleInteractComponent>())
			{
				InteractComp = Found;
				break;
			}
		}
	}

	if (InteractComp)
	{
		InteractComp->ExitCurrentVehicle(PawnOwner, true);
	}
}

void UWonderMasterControlComponent::VehicleHandbrake()
{
	if (AWonderHoverVehiclePawn* HoverPawn = Cast<AWonderHoverVehiclePawn>(PawnOwner))
	{
		HoverPawn->CurrentForwardSpeed *= 0.4f;
	}
}

void UWonderMasterControlComponent::NotifyDamageTaken(float DamageAmount)
{
	BioRallyRecoverableHealth = FMath::Clamp(BioRallyRecoverableHealth + DamageAmount, 0.0f, 100.0f);
	BioRallyWindowRemaining = 3.0f; // 3 second orange rally window
}
