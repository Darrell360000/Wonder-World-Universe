// DarrellXGenCharacter.h - Unreal Engine 5.8
// Slot 1 Playable Top-Hatter Hero: Darrell X-Gen ("The Master of Sound Magic" / "The Sonic Tactician")
// Accompanied by Minime Tophatter on clavicle_l, wielding the Equalizer X-Hat and Conductor's Baton Wand.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DarrellXGenCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSoundMagicCast, FString, AbilityName, float, NewRhythmHeat);

UCLASS()
class WONDERTOWERTYCOON_API ADarrellXGenCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ADarrellXGenCharacter();

protected:
    virtual void BeginPlay() override;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Magic")
    float RhythmHeat = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Magic")
    float TargetBPM = 85.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Magic")
    float GodHandParryWindowSeconds = 0.35f;

    UPROPERTY(BlueprintAssignable, Category = "Sound Magic|Events")
    FOnSoundMagicCast OnSoundMagicCast;

    // Casts Frequency Shockwave (+15.0f Rhythm Heat per beat hit), shattering Phase 1 Syndicate Overlord sonic barriers
    UFUNCTION(BlueprintCallable, Category = "Sound Magic|Combat")
    void CastSonicShockwave(FVector TargetLocation);

    // Slows time (Matrix-style) around foes on the 85 BPM beat
    UFUNCTION(BlueprintCallable, Category = "Sound Magic|Combat")
    void ActivateTempoLock(float DurationSeconds = 3.0f);

    // Ultimate 360-degree screen-clearing audio finisher at 100.0f Rhythm Heat
    UFUNCTION(BlueprintCallable, Category = "Sound Magic|Combat")
    bool UnleashWonderHatBoomerang();
};
