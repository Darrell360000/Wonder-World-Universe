#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "WonderCameraDirectorComponent.generated.h"

UENUM(BlueprintType)
enum class ECameraPerspective : uint8
{
	ThirdPerson UMETA(DisplayName = "Third-Person (Over-the-Shoulder)"),
	FirstPerson UMETA(DisplayName = "First-Person (Immersive Eye-Level)")
};

UCLASS(ClassGroup=(WonderWorld), meta=(BlueprintSpawnableComponent))
class WONDERTOWERTYCOON_API UWonderCameraDirectorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWonderCameraDirectorComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/* --- CAMERA STATE & TOGGLE --- */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Camera")
	ECameraPerspective CurrentPerspective = ECameraPerspective::ThirdPerson;

	UFUNCTION(BlueprintCallable, Category = "Wonder|Camera")
	void TogglePerspective();

	UFUNCTION(BlueprintCallable, Category = "Wonder|Camera")
	void SetPerspective(ECameraPerspective NewPerspective);

	/* --- TUNING PARAMETERS --- */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Camera - Tuning")
	float ThirdPersonArmLength = 360.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Camera - Tuning")
	FVector ThirdPersonSocketOffset = FVector(0.f, 55.f, 25.f); // Right-shoulder brawler view

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Camera - Tuning")
	FVector FirstPersonSocketOffset = FVector(15.f, 0.f, 65.f); // Eye-level view

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Camera - Tuning")
	float CameraTransitionSpeed = 14.0f; // Smooth zoom interpolation

private:
	UPROPERTY()
	USpringArmComponent* SpringArmComp;

	UPROPERTY()
	UCameraComponent* CameraComp;

	float TargetArmLength;
	FVector TargetSocketOffset;

	void UpdateMeshClipping();
};
