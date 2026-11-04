#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WonderMermaidMagicComponent.h"
#include "MiccaXGenCharacter.generated.h"

UCLASS()
class WONDERTOWERTYCOON_API AMiccaXGenCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMiccaXGenCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Micca|Magic")
	UWonderMermaidMagicComponent* MermaidMagicComponent;
};
