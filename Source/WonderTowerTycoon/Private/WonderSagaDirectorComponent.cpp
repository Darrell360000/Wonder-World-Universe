// Copyright Darrell R. Lewis / Wonder World Trilogy. All Rights Reserved.

#include "WonderSagaDirectorComponent.h"
#include "Engine/Engine.h"

UWonderSagaDirectorComponent::UWonderSagaDirectorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UWonderSagaDirectorComponent::BeginPlay()
{
	Super::BeginPlay();
	ApplySagaRestrictions();
}

void UWonderSagaDirectorComponent::AdvanceAct()
{
	switch (CurrentAct)
	{
	case EWonderAct::ActI_SovereignStreets:
		CurrentAct = EWonderAct::ActII_ShadowGrid;
		break;
	case EWonderAct::ActII_ShadowGrid:
		CurrentAct = EWonderAct::ActIII_ApexCitadel;
		break;
	case EWonderAct::ActIII_ApexCitadel:
		// Already at apex
		return;
	}

	ApplySagaRestrictions();
	OnActChanged.Broadcast(CurrentAct);

	if (GEngine)
	{
		const TCHAR* ActName = TEXT("Act I: Sovereign Streets");
		if (CurrentAct == EWonderAct::ActII_ShadowGrid) ActName = TEXT("Act II: The Shadow Grid & Atlantis City");
		else if (CurrentAct == EWonderAct::ActIII_ApexCitadel) ActName = TEXT("Act III: The Apex Citadel");

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Emerald,
			FString::Printf(TEXT("★ SAGA ADVANCED: %s Unlocked! ★"), ActName));
	}
}

void UWonderSagaDirectorComponent::ApplySagaRestrictions()
{
	// Gate features per part; content stripping is done by GFP/chunking at cook time.
	switch (ActiveSagaMode)
	{
	case ESagaChapterMode::Part1_SovereignStreets: break; // streets, hover drive, ledger
	case ESagaChapterMode::Part2_ShadowGrid:       break; // stealth, ARI, Atlantis
	case ESagaChapterMode::Part3_ApexCitadel:      break; // jets, Titan
	case ESagaChapterMode::Part4_LunarFrontier:    break; // shipyards, warp drive, Europa
	default: break; // Full omnibus: everything on
	}
}

void UWonderSagaDirectorComponent::SetSagaMode(ESagaChapterMode NewMode)
{
	if (ActiveSagaMode != NewMode)
	{
		ActiveSagaMode = NewMode;
		ApplySagaRestrictions();
		OnSagaModeChanged.Broadcast(ActiveSagaMode);
	}
}

bool UWonderSagaDirectorComponent::IsSubwayGridAccessible() const
{
	return CurrentAct >= EWonderAct::ActII_ShadowGrid ||
	       ActiveSagaMode == ESagaChapterMode::Part2_ShadowGrid ||
	       ActiveSagaMode == ESagaChapterMode::Part3_ApexCitadel ||
	       ActiveSagaMode == ESagaChapterMode::Full_Trilogy_Omnibus;
}

bool UWonderSagaDirectorComponent::IsCitadelApexAccessible() const
{
	return CurrentAct >= EWonderAct::ActIII_ApexCitadel ||
	       ActiveSagaMode == ESagaChapterMode::Part3_ApexCitadel ||
	       ActiveSagaMode == ESagaChapterMode::Full_Trilogy_Omnibus;
}

bool UWonderSagaDirectorComponent::IsStratosphericFlightAllowed() const
{
	return CurrentAct >= EWonderAct::ActIII_ApexCitadel ||
	       ActiveSagaMode == ESagaChapterMode::Part3_ApexCitadel ||
	       ActiveSagaMode == ESagaChapterMode::Part4_LunarFrontier ||
	       ActiveSagaMode == ESagaChapterMode::Full_Trilogy_Omnibus;
}

bool UWonderSagaDirectorComponent::IsLunarShipyardsAccessible() const
{
	return ActiveSagaMode == ESagaChapterMode::Part4_LunarFrontier ||
	       ActiveSagaMode == ESagaChapterMode::Full_Trilogy_Omnibus;
}

bool UWonderSagaDirectorComponent::IsWarpTravelAccessible() const
{
	return ActiveSagaMode == ESagaChapterMode::Part4_LunarFrontier ||
	       ActiveSagaMode == ESagaChapterMode::Full_Trilogy_Omnibus;
}
