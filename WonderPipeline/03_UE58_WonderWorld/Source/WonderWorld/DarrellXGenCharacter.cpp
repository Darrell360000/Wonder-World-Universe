// DarrellXGenCharacter.cpp - Unreal Engine 5.8
#include "DarrellXGenCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

ADarrellXGenCharacter::ADarrellXGenCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        MoveComp->bConstrainToPlane = true;
        MoveComp->SetPlaneConstraintNormal(FVector(0.0f, 1.0f, 0.0f));
    }
}

void ADarrellXGenCharacter::BeginPlay()
{
    Super::BeginPlay();
}

void ADarrellXGenCharacter::CastSonicShockwave(FVector TargetLocation)
{
    RhythmHeat = FMath::Clamp(RhythmHeat + 15.0f, 0.0f, 100.0f);
    OnSoundMagicCast.Broadcast(TEXT("FrequencyShockwave"), RhythmHeat);
}

void ADarrellXGenCharacter::ActivateTempoLock(float DurationSeconds)
{
    RhythmHeat = FMath::Clamp(RhythmHeat + 10.0f, 0.0f, 100.0f);
    OnSoundMagicCast.Broadcast(TEXT("TempoLock"), RhythmHeat);
}

bool ADarrellXGenCharacter::UnleashWonderHatBoomerang()
{
    if (RhythmHeat < 100.0f)
    {
        return false;
    }

    RhythmHeat = 0.0f;
    OnSoundMagicCast.Broadcast(TEXT("WonderHatBoomerang"), RhythmHeat);
    return true;
}
