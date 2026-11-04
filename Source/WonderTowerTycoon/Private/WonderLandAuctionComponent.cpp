#include "WonderLandAuctionComponent.h"

UWonderLandAuctionComponent::UWonderLandAuctionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UWonderLandAuctionComponent::BeginPlay()
{
	Super::BeginPlay();

	// Seed sample initial property auctions for live Wonder Phone bourse
	StartNewAuction(FName("Lot_NeonStrip_4G"), TEXT("Neon Boulevard Commercial Lot 4-G"), 75000, 45.0f);
	StartNewAuction(FName("Lot_IronWorks_Substation"), TEXT("Iron Works Industrial Parcel 12"), 120000, 60.0f);
}

void UWonderLandAuctionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TArray<FName> ExpiredLots;

	for (auto& Pair : ActiveAuctions)
	{
		FLandAuctionLot& Lot = Pair.Value;
		if (Lot.bIsActive)
		{
			Lot.TimeRemainingSeconds -= DeltaTime;
			if (Lot.TimeRemainingSeconds <= 0.0f)
			{
				Lot.TimeRemainingSeconds = 0.0f;
				ExpiredLots.Add(Pair.Key);
			}
		}
	}

	SimulateRivalBids(DeltaTime);

	for (const FName& ExpiredID : ExpiredLots)
	{
		FinalizeAuction(ExpiredID);
	}
}

bool UWonderLandAuctionComponent::StartNewAuction(FName LotID, FString LotName, int64 BasePrice, float DurationSeconds)
{
	if (LotID.IsNone()) return false;

	FLandAuctionLot NewLot;
	NewLot.LotID = LotID;
	NewLot.LotName = LotName;
	NewLot.BasePrice = BasePrice;
	NewLot.CurrentBid = BasePrice;
	NewLot.HighestBidder = TEXT("Syndicate Opening Bid");
	NewLot.TimeRemainingSeconds = DurationSeconds;
	NewLot.bIsActive = true;
	NewLot.bPlayerWon = false;

	NewLot.RivalCorporations.Add(TEXT("Kuroda Syndicate Corp"));
	NewLot.RivalCorporations.Add(TEXT("Apex Horizon Conglomerate"));
	NewLot.RivalCorporations.Add(TEXT("Vectra Capital Syndicate"));

	ActiveAuctions.Add(LotID, NewLot);
	OnAuctionStarted.Broadcast(NewLot);

	UE_LOG(LogTemp, Log, TEXT("[WonderAuction] Auction opened for '%s' (Base: $%lld, Duration: %.1fs)"),
		*LotName, BasePrice, DurationSeconds);

	return true;
}

bool UWonderLandAuctionComponent::PlacePlayerBid(FName LotID, int64 BidAmount)
{
	return PlaceBid(LotID, BidAmount, TEXT("Darrell (Player)"));
}

bool UWonderLandAuctionComponent::PlaceBid(FName LotID, int64 BidAmount, const FString& BidderName)
{
	FLandAuctionLot* Lot = ActiveAuctions.Find(LotID);
	if (!Lot || !Lot->bIsActive) return false;

	if (BidAmount <= Lot->CurrentBid)
	{
		UE_LOG(LogTemp, Warning, TEXT("[WonderAuction] Rejected bid of $%lld on '%s' (Must exceed current bid $%lld)"),
			BidAmount, *Lot->LotName, Lot->CurrentBid);
		return false;
	}

	Lot->CurrentBid = BidAmount;
	Lot->HighestBidder = BidderName;

	// Anti-sniping: extend timer if bid placed in the final 5 seconds
	if (Lot->TimeRemainingSeconds < 5.0f)
	{
		Lot->TimeRemainingSeconds = 5.0f;
	}

	UE_LOG(LogTemp, Log, TEXT("[WonderAuction] New Highest Bid on '%s': $%lld by '%s' (Time Left: %.1fs)"),
		*Lot->LotName, BidAmount, *BidderName, Lot->TimeRemainingSeconds);

	OnBidPlaced.Broadcast(LotID, BidAmount, BidderName);
	return true;
}

bool UWonderLandAuctionComponent::GetAuctionLot(FName LotID, FLandAuctionLot& OutLot) const
{
	if (const FLandAuctionLot* Found = ActiveAuctions.Find(LotID))
	{
		OutLot = *Found;
		return true;
	}
	return false;
}

void UWonderLandAuctionComponent::SimulateRivalBids(float DeltaTime)
{
	RivalBidTimer += DeltaTime;
	if (RivalBidTimer < RivalBidIntervalSeconds) return;

	RivalBidTimer = 0.0f;

	for (auto& Pair : ActiveAuctions)
	{
		FLandAuctionLot& Lot = Pair.Value;
		if (Lot.bIsActive && Lot.HighestBidder == TEXT("Darrell (Player)"))
		{
			if (FMath::FRand() < RivalBidProbability && Lot.RivalCorporations.Num() > 0)
			{
				const int32 RivalIndex = FMath::RandRange(0, Lot.RivalCorporations.Num() - 1);
				const FString& RivalName = Lot.RivalCorporations[RivalIndex];

				const float EscalationPercent = FMath::FRandRange(0.08f, 0.22f);
				const int64 Raise = FMath::Max(5000LL, static_cast<int64>(Lot.CurrentBid * EscalationPercent));
				const int64 NewRivalBid = Lot.CurrentBid + Raise;

				PlaceBid(Lot.LotID, NewRivalBid, RivalName);
			}
		}
	}
}

void UWonderLandAuctionComponent::FinalizeAuction(FName LotID)
{
	FLandAuctionLot* Lot = ActiveAuctions.Find(LotID);
	if (!Lot || !Lot->bIsActive) return;

	Lot->bIsActive = false;
	Lot->bPlayerWon = (Lot->HighestBidder == TEXT("Darrell (Player)"));

	if (Lot->bPlayerWon)
	{
		UE_LOG(LogTemp, Log, TEXT("[WonderAuction] AUCTION WON! Darrell secured '%s' for $%lld. Lot is now available for Wonder Tower construction!"),
			*Lot->LotName, Lot->CurrentBid);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[WonderAuction] AUCTION LOST! '%s' won by '%s' for $%lld. Property converted into a raidable Syndicate Fortress!"),
			*Lot->LotName, *Lot->HighestBidder, Lot->CurrentBid);
	}

	OnAuctionFinalized.Broadcast(*Lot, Lot->bPlayerWon);
}
