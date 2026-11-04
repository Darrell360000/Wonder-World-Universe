// Copyright Darrell R. Lewis / Wonder World Trilogy. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WonderAntiGravityLabComponent.generated.h"

UENUM(BlueprintType)
enum class EGravityDriveMode : uint8
{
	StandardEarth     UMETA(DisplayName = "Standard Earth (1.0G Down)"),
	ZeroGChamber      UMETA(DisplayName = "Zero-G Floating Chamber"),
	InvertedSkyscraper UMETA(DisplayName = "360° Glass Curtain Wall Inversion"),
	SingularityVortex UMETA(DisplayName = "Graviton Singularity Vortex")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGravityModeChanged, EGravityDriveMode, NewMode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGravitonSingularitySpawned, FVector, VortexLocation, float, Radius);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPersonalGravityInverted, FVector, NewGravityDirection);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnZeroGVolumeStateChanged, bool, bIsInsideZeroG);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHovercraftInversionTuned, float, Efficiency, float, AlignmentRate);

/**
 * UWonderAntiGravityLabComponent
 * Implements the 2027 Vanguard Anti-Gravity R&D Lab mechanics:
 * Zero-G physics volume triggers, Graviton Singularity pull forces,
 * and gravity-inversion drive tuning for hovercraft and wall-running traversal.
 */
UCLASS(ClassGroup=(WonderWorld), meta=(BlueprintSpawnableComponent))
class WONDERTOWERTYCOON_API UWonderAntiGravityLabComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWonderAntiGravityLabComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/* --- ZERO-G VOLUME TRIGGERS --- */
	UFUNCTION(BlueprintCallable, Category = "Wonder|AntiGravity")
	void EnterZeroGVolume();

	UFUNCTION(BlueprintCallable, Category = "Wonder|AntiGravity")
	void ExitZeroGVolume();

	/* --- GRAVITY DRIVE CONTROLS --- */
	UFUNCTION(BlueprintCallable, Category = "Wonder|AntiGravity")
	void SetGravityMode(EGravityDriveMode NewMode);

	UFUNCTION(BlueprintCallable, Category = "Wonder|AntiGravity")
	void ShiftPersonalGravityVector(FVector NewGravityDirection);

	UFUNCTION(BlueprintCallable, Category = "Wonder|AntiGravity")
	void SpawnGravitonSingularity(FVector VortexLocation, float DurationSeconds = 6.0f);

	UFUNCTION(BlueprintCallable, Category = "Wonder|AntiGravity")
	void DismissSingularity();

	/* --- HOVERCRAFT DRIVE TUNING --- */
	UFUNCTION(BlueprintCallable, Category = "Wonder|AntiGravity|Hovercraft")
	void TuneHovercraftGravityInversion(float NewEfficiency, float NewAlignmentRate);

	/* --- QUERIES --- */
	UFUNCTION(BlueprintPure, Category = "Wonder|AntiGravity")
	EGravityDriveMode GetCurrentGravityMode() const { return CurrentMode; }

	UFUNCTION(BlueprintPure, Category = "Wonder|AntiGravity")
	FVector GetActiveGravityVector() const { return CurrentGravityDirection; }

	UFUNCTION(BlueprintPure, Category = "Wonder|AntiGravity")
	bool IsInZeroGVolume() const { return bInsideZeroGVolume; }

	UFUNCTION(BlueprintPure, Category = "Wonder|AntiGravity")
	bool IsSingularityActive() const { return bSingularityActive; }

	UFUNCTION(BlueprintPure, Category = "Wonder|AntiGravity|Hovercraft")
	float GetInversionEfficiency() const { return InversionDriveEfficiency; }

	/* --- TUNING PARAMETERS --- */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|AntiGravity|Tuning")
	float SingularityRadius = 1800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|AntiGravity|Tuning")
	float SingularityPullForce = 6500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|AntiGravity|Tuning")
	float ZeroGFrictionDamping = 0.85f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|AntiGravity|Tuning")
	float GravityShiftInterpolationSpeed = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|AntiGravity|Hovercraft")
	float InversionDriveEfficiency = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|AntiGravity|Hovercraft")
	float HovercraftInversionAlignmentRate = 12.0f;

	/* --- EVENTS --- */
	UPROPERTY(BlueprintAssignable, Category = "Wonder|AntiGravity|Events")
	FOnGravityModeChanged OnGravityModeChanged;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|AntiGravity|Events")
	FOnGravitonSingularitySpawned OnGravitonSingularitySpawned;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|AntiGravity|Events")
	FOnPersonalGravityInverted OnPersonalGravityInverted;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|AntiGravity|Events")
	FOnZeroGVolumeStateChanged OnZeroGVolumeStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|AntiGravity|Events")
	FOnHovercraftInversionTuned OnHovercraftInversionTuned;

protected:
	virtual void BeginPlay() override;

private:
	void UpdateSingularityPull(float DeltaTime);
	void UpdatePersonalGravityOrientation(float DeltaTime);

	EGravityDriveMode CurrentMode = EGravityDriveMode::StandardEarth;
	FVector CurrentGravityDirection = FVector::DownVector;
	FVector TargetGravityDirection = FVector::DownVector;

	bool bInsideZeroGVolume = false;
	bool bSingularityActive = false;
	FVector ActiveSingularityLocation = FVector::ZeroVector;
	float SingularityTimeRemaining = 0.0f;
};
