#include "WonderAdvancedFlightComponent.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"

UWonderAdvancedFlightComponent::UWonderAdvancedFlightComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_PrePhysics;
}

void UWonderAdvancedFlightComponent::SetThrottleInput(float Value)
{
	TargetThrottle = FMath::Clamp(Value, -0.5f, 1.0f);
}

void UWonderAdvancedFlightComponent::SetFlightSteering(float Pitch, float Roll, float Yaw)
{
	CurrentFlightTorques = FVector(Roll, Pitch, Yaw);
}

void UWonderAdvancedFlightComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!RootPrim && GetOwner())
	{
		RootPrim = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent());
	}
	if (!RootPrim || !GetOwner()) return;

	CalculateAerodynamics(DeltaTime);
}

void UWonderAdvancedFlightComponent::CalculateAerodynamics(float DeltaTime)
{
	if (!GetOwner() || !RootPrim) return;

	// Forward Thrust
	float CurrentMaxSpeed = (TargetThrottle > 0.8f) ? (MaxFlightSpeed * AfterburnerMultiplier) : MaxFlightSpeed;
	FVector ThrustForce = GetOwner()->GetActorForwardVector() * (TargetThrottle * CurrentMaxSpeed * 150.0f);
	RootPrim->AddForce(ThrustForce);

	// 3-Axis Angular Steering (Pitch, Roll, Yaw)
	FVector AngularForce = CurrentFlightTorques * 85000.0f;
	RootPrim->AddTorqueInRadians(GetOwner()->GetActorRotation().RotateVector(AngularForce));

	// Natural Aerodynamic Lift & Drag
	FVector Velocity = RootPrim->GetComponentVelocity();
	float ForwardSpeed = FVector::DotProduct(Velocity, GetOwner()->GetActorForwardVector());
	FVector LiftForce = GetOwner()->GetActorUpVector() * (ForwardSpeed * 12.0f);
	RootPrim->AddForce(LiftForce);
}
