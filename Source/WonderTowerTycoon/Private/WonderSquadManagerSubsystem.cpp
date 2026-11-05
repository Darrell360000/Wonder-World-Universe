// Copyright Epic Games, Inc. All Rights Reserved.

#include "WonderSquadManagerSubsystem.h"

void UWonderSquadManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	HeroRoster.Empty();

	// 1. Darrell X-Gen
	FWonderHeroInfo Darrell;
	Darrell.Role = EWonderHeroRole::DarrellXGen;
	Darrell.HeroName = TEXT("Darrell X-Gen");
	Darrell.SignatureHat = TEXT("Equalizer X-Hat (SOUND MAGIC LED)");
	Darrell.SignatureWeapon = TEXT("Dual Conductor Baton Wands");
	Darrell.bIsUnlocked = true;
	Darrell.bIsInActiveSquad = true;
	HeroRoster.Add(Darrell);

	// 2. Minime Tophatter
	FWonderHeroInfo Minime;
	Minime.Role = EWonderHeroRole::MinimeTactical;
	Minime.HeroName = TEXT("Minime Tophatter");
	Minime.SignatureHat = TEXT("Micro Digital Equalizer Topper");
	Minime.SignatureWeapon = TEXT("Geometric Light Scanner");
	Minime.bIsUnlocked = true;
	Minime.bIsInActiveSquad = true;
	HeroRoster.Add(Minime);

	// 3. Spider-Pulse
	FWonderHeroInfo SpiderPulse;
	SpiderPulse.Role = EWonderHeroRole::SpiderPulse;
	SpiderPulse.HeroName = TEXT("The Spider-Pulse Icon");
	SpiderPulse.SignatureHat = TEXT("Theatrical Pink-Band Top Hat");
	SpiderPulse.SignatureWeapon = TEXT("Cybernetic Forearm Gauntlets & Web-Lines");
	SpiderPulse.bIsUnlocked = true;
	SpiderPulse.bIsInActiveSquad = true;
	HeroRoster.Add(SpiderPulse);

	// 4. Divine Hands
	FWonderHeroInfo DivineHands;
	DivineHands.Role = EWonderHeroRole::DivineHands;
	DivineHands.HeroName = TEXT("The Divine Hands Top-Hatter");
	DivineHands.SignatureHat = TEXT("God's Hands Gold Crown Hat");
	DivineHands.SignatureWeapon = TEXT("Ethereal Translucent Shaping Hands");
	DivineHands.bIsUnlocked = true;
	DivineHands.bIsInActiveSquad = true;
	HeroRoster.Add(DivineHands);

	// 5. Jaxson Thorne
	FWonderHeroInfo Jaxson;
	Jaxson.Role = EWonderHeroRole::JaxsonThorne;
	Jaxson.HeroName = TEXT("Jaxson 'Iron Riff' Thorne");
	Jaxson.SignatureHat = TEXT("Anvil Crown (Hydraulic Shock Absorbers & Hazard Lights)");
	Jaxson.SignatureWeapon = TEXT("Pneumatic Sonic-Gauntlet Fists");
	Jaxson.bIsUnlocked = true;
	Jaxson.bIsInActiveSquad = true;
	HeroRoster.Add(Jaxson);

	// 6. Tao X Zen
	FWonderHeroInfo Tao;
	Tao.Role = EWonderHeroRole::TaoXZen;
	Tao.HeroName = TEXT("Tao X Zen");
	Tao.SignatureHat = TEXT("The Wonder Hat (Pink-Gold Taoist Sigils & Purple Portal)");
	Tao.SignatureWeapon = TEXT("Cosmic Chrono-Tears & Warp Duster");
	Tao.bIsUnlocked = true;
	Tao.bIsInActiveSquad = true;
	HeroRoster.Add(Tao);

	// 7. Mama Wonder
	FWonderHeroInfo Mama;
	Mama.Role = EWonderHeroRole::MamaWonder;
	Mama.HeroName = TEXT("Mama Wonder");
	Mama.SignatureHat = TEXT("Empress Equalizer Crown Top Hat");
	Mama.SignatureWeapon = TEXT("Evon Hub Scepter & Conductor Baton");
	Mama.bIsUnlocked = true;
	Mama.bIsInActiveSquad = true;
	HeroRoster.Add(Mama);

	CurrentActiveHero = EWonderHeroRole::DarrellXGen;
	OnSquadFormationUpdated.Broadcast(HeroRoster.Num());
}

bool UWonderSquadManagerSubsystem::SwitchActiveHero(EWonderHeroRole TargetRole)
{
	for (const FWonderHeroInfo& Info : HeroRoster)
	{
		if (Info.Role == TargetRole && Info.bIsUnlocked)
		{
			CurrentActiveHero = TargetRole;
			OnActiveHeroChanged.Broadcast(CurrentActiveHero, Info.HeroName);
			return true;
		}
	}
	return false;
}
