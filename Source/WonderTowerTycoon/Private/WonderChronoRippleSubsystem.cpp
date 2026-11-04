// Copyright Darrell R. Lewis / Wonder World Trilogy. All Rights Reserved.

#include "WonderChronoRippleSubsystem.h"
#include "Engine/World.h"

void UWonderChronoRippleSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	InitializeDefaultDistricts();

	UE_LOG(LogTemp, Log, TEXT("[WonderChrono] UWonderChronoRippleSubsystem Initialized with %d Districts."), DistrictLayouts.Num());
}

void UWonderChronoRippleSubsystem::InitializeDefaultDistricts()
{
	DistrictLayouts.Empty();

	// District 1: Downtown Central
	{
		FDistrictTemporalLayout D1;
		D1.DistrictId = TEXT("DowntownCentral");
		D1.DistrictName = TEXT("Downtown Central Core");
		D1.ActiveArchitecturalStyle = TEXT("Classic Art Deco Retro-Futurism");
		D1.AssignedCEO = TEXT("Darrell R. Lewis");
		D1.bSubwayVaultRouteOpen = false;
		D1.EconomicSynergyBonus = 1.0f;
		DistrictLayouts.Add(D1.DistrictId, D1);
	}

	// District 2: The Neon Works / Industrial Strip
	{
		FDistrictTemporalLayout D2;
		D2.DistrictId = TEXT("NeonWorks");
		D2.DistrictName = TEXT("The Neon Industrial Strip");
		D2.ActiveArchitecturalStyle = TEXT("Heavy Industrial Cyberpunk");
		D2.AssignedCEO = TEXT("Syndicate Enforcer Warlord");
		D2.bSubwayVaultRouteOpen = false;
		D2.EconomicSynergyBonus = 0.85f;
		DistrictLayouts.Add(D2.DistrictId, D2);
	}

	// District 3: Financial Spire Heights
	{
		FDistrictTemporalLayout D3;
		D3.DistrictId = TEXT("FinancialHeights");
		D3.DistrictName = TEXT("Financial Spire Heights");
		D3.ActiveArchitecturalStyle = TEXT("Brutalist Corporate Monolith");
		D3.AssignedCEO = TEXT("Corrupt Chief Justice");
		D3.bSubwayVaultRouteOpen = false;
		D3.EconomicSynergyBonus = 1.2f;
		DistrictLayouts.Add(D3.DistrictId, D3);
	}

	// District 4: Waterfront Transit Marina
	{
		FDistrictTemporalLayout D4;
		D4.DistrictId = TEXT("WaterfrontMarina");
		D4.DistrictName = TEXT("Waterfront Transit Marina");
		D4.ActiveArchitecturalStyle = TEXT("Bioluminescent Solarpunk");
		D4.AssignedCEO = TEXT("Mama Tower Municipal Council");
		D4.bSubwayVaultRouteOpen = true;
		D4.EconomicSynergyBonus = 1.15f;
		DistrictLayouts.Add(D4.DistrictId, D4);
	}

	// District 5: Apex Stratosphere Ascent
	{
		FDistrictTemporalLayout D5;
		D5.DistrictId = TEXT("ApexAscent");
		D5.DistrictName = TEXT("Apex Stratosphere Ascent");
		D5.ActiveArchitecturalStyle = TEXT("Golden Vanguard Solarpunk");
		D5.AssignedCEO = TEXT("Syndicate Overlord Titan");
		D5.bSubwayVaultRouteOpen = false;
		D5.EconomicSynergyBonus = 1.5f;
		DistrictLayouts.Add(D5.DistrictId, D5);
	}
}

void UWonderChronoRippleSubsystem::RecordHistoricalDecision(FString DecisionKey, FString MemoryTitle, EDromeMemoryEra Era, int32 ChoiceIndex)
{
	FChronoMemoryDecision Decision;
	Decision.DecisionKey = DecisionKey;
	Decision.MemoryTitle = MemoryTitle;
	Decision.Era = Era;
	Decision.ChosenPath = ChoiceIndex;
	Decision.TimeStamp = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0;

	DecisionHistory.Add(Decision);

	UE_LOG(LogTemp, Warning, TEXT("[WonderChrono] HISTORICAL MEMORY DECISION LOGGED: '%s' | Era: %d | Choice: %d"),
		*MemoryTitle, static_cast<int32>(Era), ChoiceIndex);
}

void UWonderChronoRippleSubsystem::TriggerChronoRipple(FString DecisionKey, FString AffectedDistrictId, FString NewArchitecturalStyle, FString NewCEO, bool bUnlockSubwayRoute)
{
	if (!DistrictLayouts.Contains(AffectedDistrictId))
	{
		return;
	}

	FDistrictTemporalLayout& Layout = DistrictLayouts[AffectedDistrictId];

	FString OldStyle = Layout.ActiveArchitecturalStyle;
	FString OldCEO = Layout.AssignedCEO;

	if (!NewArchitecturalStyle.IsEmpty() && NewArchitecturalStyle != OldStyle)
	{
		Layout.ActiveArchitecturalStyle = NewArchitecturalStyle;
		OnArchitecturalStyleMorphed.Broadcast(AffectedDistrictId, OldStyle, NewArchitecturalStyle);
	}

	if (!NewCEO.IsEmpty() && NewCEO != OldCEO)
	{
		Layout.AssignedCEO = NewCEO;
		OnCEOLeadershipOverturned.Broadcast(AffectedDistrictId, OldCEO, NewCEO);
	}

	if (bUnlockSubwayRoute && !Layout.bSubwayVaultRouteOpen)
	{
		Layout.bSubwayVaultRouteOpen = true;
		OnTemporalSubwayRouteUnlocked.Broadcast(AffectedDistrictId);
	}

	OnChronoRipplePropagated.Broadcast(DecisionKey, AffectedDistrictId, Layout.ActiveArchitecturalStyle, Layout.AssignedCEO);

	UE_LOG(LogTemp, Warning, TEXT("[WonderChrono] CHRONO-RIPPLE PROPAGATED to District '%s': Style -> '%s' | CEO -> '%s' | Subway Route: %s"),
		*AffectedDistrictId, *Layout.ActiveArchitecturalStyle, *Layout.AssignedCEO, Layout.bSubwayVaultRouteOpen ? TEXT("OPEN") : TEXT("LOCKED"));
}

void UWonderChronoRippleSubsystem::ApplyArchitecturalShift(FString DistrictId, FString NewStyle)
{
	if (DistrictLayouts.Contains(DistrictId))
	{
		FDistrictTemporalLayout& Layout = DistrictLayouts[DistrictId];
		FString OldStyle = Layout.ActiveArchitecturalStyle;
		Layout.ActiveArchitecturalStyle = NewStyle;
		OnArchitecturalStyleMorphed.Broadcast(DistrictId, OldStyle, NewStyle);
	}
}

void UWonderChronoRippleSubsystem::TransferDistrictCEO(FString DistrictId, FString NewCEO)
{
	if (DistrictLayouts.Contains(DistrictId))
	{
		FDistrictTemporalLayout& Layout = DistrictLayouts[DistrictId];
		FString OldCEO = Layout.AssignedCEO;
		Layout.AssignedCEO = NewCEO;
		OnCEOLeadershipOverturned.Broadcast(DistrictId, OldCEO, NewCEO);
	}
}

void UWonderChronoRippleSubsystem::UnlockSubwayRoute(FString DistrictId)
{
	if (DistrictLayouts.Contains(DistrictId))
	{
		FDistrictTemporalLayout& Layout = DistrictLayouts[DistrictId];
		Layout.bSubwayVaultRouteOpen = true;
		OnTemporalSubwayRouteUnlocked.Broadcast(DistrictId);
	}
}

bool UWonderChronoRippleSubsystem::GetDistrictTemporalLayout(FString DistrictId, FDistrictTemporalLayout& OutLayout) const
{
	if (const FDistrictTemporalLayout* Found = DistrictLayouts.Find(DistrictId))
	{
		OutLayout = *Found;
		return true;
	}
	return false;
}

TArray<FDistrictTemporalLayout> UWonderChronoRippleSubsystem::GetAllDistrictStates() const
{
	TArray<FDistrictTemporalLayout> Results;
	DistrictLayouts.GenerateValueArray(Results);
	return Results;
}
