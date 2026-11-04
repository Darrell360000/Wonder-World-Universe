// SyndicateOverlordBoss.h - Unreal Engine 5.8
// Flagship 3-Phase Tactical Tag-Team Boss Encounter ("THE SYNDICATE OVERLORD")
// Phase 1: Sonic Barrier Breach (Requires Darrell X-Gen's CastSonicShockwave)
// Phase 2: Laser Grid Sabotage (Requires Vesper Vance / Spider-Pulse CodeSliceExecution)
// Phase 3: Armored Juggernaut Showdown (Requires Jaxson "Iron Riff" Thorne's BassCannonSmash)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SyndicateOverlordBoss.generated.h"

UENUM(BlueprintType)
enum class ESyndicateBossPhase : uint8
{
    Phase1_SonicBarrierBreach UMETA(DisplayName = "Phase 1: Sonic Barrier Breach (Darrell X-Gen)"),
    Phase2_LaserGridSabotage  UMETA(DisplayName = "Phase 2: Laser Grid Sabotage (Vesper Vance / Spider-Pulse)"),
    Phase3_ArmoredJuggernaut  UMETA(DisplayName = "Phase 3: Armored Juggernaut Showdown (Jaxson Thorne)"),
    Defeated                  UMETA(DisplayName = "Boss Defeated & Purified")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBossPhaseChanged, ESyndicateBossPhase, NewPhase, FString, TacticalPrompt);

UCLASS()
class WONDERTOWERTYCOON_API ASyndicateOverlordBoss : public AActor
{
    GENERATED_BODY()

public:
    ASyndicateOverlordBoss();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Syndicate Overlord")
    ESyndicateBossPhase CurrentPhase = ESyndicateBossPhase::Phase1_SonicBarrierBreach;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Syndicate Overlord")
    float SonicBarrierIntegrity = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Syndicate Overlord")
    bool bLaserGridActive = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Syndicate Overlord")
    float MechArmorHealth = 100.0f;

    UPROPERTY(BlueprintAssignable, Category = "Syndicate Overlord|Events")
    FOnBossPhaseChanged OnBossPhaseChanged;

    // Phase 1: Darrell X-Gen overloads the high-frequency sound barrier at 85 BPM
    UFUNCTION(BlueprintCallable, Category = "Syndicate Overlord|Phase 1")
    bool ApplyDarrellSonicShockwave(float ShockwavePower);

    // Phase 2: Vesper Vance / Spider-Pulse infiltrates the laser grid and executes Code-Slice on the mainframe
    UFUNCTION(BlueprintCallable, Category = "Syndicate Overlord|Phase 2")
    bool ExecuteVesperLaserGridSabotage();

    // Phase 3: Jaxson "Iron Riff" Thorne shatters the Overlord's mech armor with Bass-Cannon Smash
    UFUNCTION(BlueprintCallable, Category = "Syndicate Overlord|Phase 3")
    bool ApplyJaxsonBassCannonSmash(float SubBassDamage);
};
