// WonderHoverPhysicsComponent.cpp
// Implements 4‑corner raycast thrusters, PID spring‑damper suspension,
// Niagara visual effects, and Blueprint‑exposed controls.

#include "WonderHoverPhysicsComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Components/PrimitiveComponent.h"
#include "Components/InputComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

UWonderHoverPhysicsComponent::UWonderHoverPhysicsComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickGroup = TG_PrePhysics;
}

void UWonderHoverPhysicsComponent::BeginPlay()
{
    Super::BeginPlay();

    // Cache the root primitive for force application.
    RootPrim = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent());

    // Bind input actions (expects the owning actor to have an InputComponent).
    if (AActor* Owner = GetOwner())
    {
        if (UInputComponent* Input = Owner->FindComponentByClass<UInputComponent>())
        {
            SetupHoverInputComponent(Input);
        }
    }

    // Initialise Niagara thruster FX if a system is assigned.
    if (ThrusterNiagaraSystem)
    {
        const FVector Offsets[4] = {
            FVector( 80.f, -40.f, 0.f),
            FVector( 80.f,  40.f, 0.f),
            FVector(-80.f, -40.f, 0.f),
            FVector(-80.f,  40.f, 0.f)
        };
        for (int32 i = 0; i < 4; ++i)
        {
            UNiagaraComponent* NC = UNiagaraFunctionLibrary::SpawnSystemAttached(
                ThrusterNiagaraSystem,
                GetOwner()->GetRootComponent(),
                NAME_None,
                GetOwner()->GetActorLocation() + GetOwner()->GetActorRotation().RotateVector(Offsets[i]),
                GetOwner()->GetActorRotation(),
                EAttachLocation::KeepWorldPosition,
                true, // Auto destroy
                true, // Auto activate
                ENCPoolMethod::None,
                true
            );
            if (NC)
            {
                NC->SetVariableLinearColor(FName("User.GlowColor"), ThrusterGlowColor);
                SpawnedThrusterFX.Add(NC);
            }
        }
    }
}

void UWonderHoverPhysicsComponent::SetupHoverInputComponent(UInputComponent* Input)
{
    if (!Input)
    {
        return;
    }

    // Hover Toggle bindings (V key, Gamepad FaceButton Top)
    Input->BindAction("HoverToggle", IE_Pressed, this, &UWonderHoverPhysicsComponent::ToggleHover);
    Input->BindAction("VehicleToggleHover", IE_Pressed, this, &UWonderHoverPhysicsComponent::ToggleHover);
    Input->BindKey(EKeys::V, IE_Pressed, this, &UWonderHoverPhysicsComponent::ToggleHover);

    // Hover Ascend bindings (Spacebar, Gamepad Right Shoulder / RB / R1)
    Input->BindAction("HoverAscend", IE_Pressed, this, &UWonderHoverPhysicsComponent::HandleAscend);
    Input->BindAction("VehicleAscend", IE_Pressed, this, &UWonderHoverPhysicsComponent::HandleAscend);
    Input->BindAction("Ascend", IE_Pressed, this, &UWonderHoverPhysicsComponent::HandleAscend);

    // Hover Descend bindings (Left Ctrl, Gamepad Left Shoulder / LB / L1)
    Input->BindAction("HoverDescend", IE_Pressed, this, &UWonderHoverPhysicsComponent::HandleDescend);
    Input->BindAction("VehicleDescend", IE_Pressed, this, &UWonderHoverPhysicsComponent::HandleDescend);
    Input->BindAction("Descend", IE_Pressed, this, &UWonderHoverPhysicsComponent::HandleDescend);
}

void UWonderHoverPhysicsComponent::ToggleHover()
{
    bHoverActive = !bHoverActive;
    if (bHoverActive)
    {
        // Reset to a safe default height inside allowed range.
        DesiredHoverHeight = FMath::Clamp(DesiredHoverHeight, MinHoverHeight, MaxHoverHeight);
    }
    // Immediately update visual state.
    UpdateThrusterVisuals();
}

void UWonderHoverPhysicsComponent::Ascend(float Rate)
{
	if (bHoverActive)
	{
		DesiredHoverHeight = FMath::Clamp(DesiredHoverHeight + (Rate * 4.0f), MinHoverHeight, MaxHoverHeight);
	}
}

void UWonderHoverPhysicsComponent::Descend(float Rate)
{
	if (bHoverActive)
	{
		DesiredHoverHeight = FMath::Clamp(DesiredHoverHeight - (Rate * 4.0f), MinHoverHeight, MaxHoverHeight);
	}
}

void UWonderHoverPhysicsComponent::HandleAscend()
{
	Ascend(1.0f);
}

void UWonderHoverPhysicsComponent::HandleDescend()
{
	Descend(1.0f);
}

void UWonderHoverPhysicsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!bHoverActive || !RootPrim || !RootPrim->IsSimulatingPhysics())
    {
        return;
    }

    ApplyHoverForces(DeltaTime);
}

void UWonderHoverPhysicsComponent::ApplyHoverForces(float DeltaTime)
{
    AActor* Owner = GetOwner();
    if (!Owner) return;

    const FVector ActorLoc = Owner->GetActorLocation();
    const FRotator ActorRot = Owner->GetActorRotation();
    const FVector UpVec = Owner->GetActorUpVector();

    // 4 Corner offsets (X: Forward/Back, Y: Left/Right, Z: Downward offset)
    const FVector CornerOffsets[4] = {
        FVector( 120.f, -70.f, -10.f), // Front-Left
        FVector( 120.f,  70.f, -10.f), // Front-Right
        FVector(-120.f, -70.f, -10.f), // Rear-Left
        FVector(-120.f,  70.f, -10.f)  // Rear-Right
    };

    FCollisionQueryParams TraceParams;
    TraceParams.AddIgnoredActor(Owner);

    const float TraceLength = MaxHoverHeight * 1.5f;

    for (int32 i = 0; i < 4; ++i)
    {
        const FVector WorldOffset = ActorRot.RotateVector(CornerOffsets[i]);
        const FVector RayStart = ActorLoc + WorldOffset;
        const FVector RayEnd = RayStart - (UpVec * TraceLength);

        FHitResult Hit;
        const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, RayStart, RayEnd, ECC_Visibility, TraceParams);

        if (bHit && Hit.Distance > 0.0f)
        {
            // Compression distance: DesiredHeight minus current hit distance
            const float Compression = DesiredHoverHeight - Hit.Distance;

            if (Compression > 0.0f)
            {
                // Hooke's Law Spring Force: F = -k * x
                const float SpringForce = Compression * SpringStiffness;

                // Damping Force based on velocity at this specific corner point
                const FVector PointVelocity = RootPrim->GetPhysicsLinearVelocityAtPoint(RayStart);
                const float DampingForce = -FVector::DotProduct(PointVelocity, UpVec) * Damping;

                // Total upward thrust for this thruster corner
                const float TotalThrust = FMath::Max(0.0f, SpringForce + DampingForce);
                const FVector ForceVector = UpVec * TotalThrust;

                RootPrim->AddForceAtLocation(ForceVector, RayStart);
            }
        }
    }

    // Angular Upright Stabilization: Prevent hover vehicle from rolling or flipping over
    const FVector CurrentUp = Owner->GetActorUpVector();
    const FVector DesiredUp = FVector::UpVector;
    const FVector AngularCorrectionAxis = FVector::CrossProduct(CurrentUp, DesiredUp);
    const float AlignmentAngle = FMath::Acos(FMath::Clamp(FVector::DotProduct(CurrentUp, DesiredUp), -1.0f, 1.0f));

    if (AlignmentAngle > 0.01f)
    {
        const FVector CorrectiveTorque = AngularCorrectionAxis * (AlignmentAngle * StabilizationStrength * 1000.0f);
        RootPrim->AddTorqueInRadians(CorrectiveTorque, NAME_None, true);
    }
}

void UWonderHoverPhysicsComponent::UpdateThrusterVisuals()
{
    for (UNiagaraComponent* NC : SpawnedThrusterFX)
    {
        if (NC)
        {
            NC->SetActive(bHoverActive);
        }
    }
}
