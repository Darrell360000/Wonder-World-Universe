#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WonderLandAuctionComponent.generated.h"

USTRUCT(BlueprintType)
struct WONDERTOWERTYCOON_API FLandAuctionLot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Auction")
	FName LotID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Auction")
	FString LotName = TEXT("Commercial Lot 4-G");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Auction")
	int64 BasePrice = 50000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Auction")
	int64 CurrentBid = 50000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Auction")
	FString HighestBidder = TEXT("None");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Auction")
	float TimeRemainingSeconds = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Auction")
	bool bIsActive = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Auction")
	bool bPlayerWon = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Auction")
	TArray<FString> RivalCorporations;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAuctionStarted, const FLandAuctionLot&, Lot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnBidPlaced, FName, LotID, int64, NewBid, const FString&, BidderName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAuctionFinalized, const FLandAuctionLot&, Lot, bool, bPlayerWon);

UCLASS(ClassGroup=(WonderWorld), meta=(BlueprintSpawnableComponent))
class WONDERTOWERTYCOON_API UWonderLandAuctionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWonderLandAuctionComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/* --- AUCTION BOURSE --- */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wonder|Auction")
	TMap<FName, FLandAuctionLot> ActiveAuctions;

	UFUNCTION(BlueprintCallable, Category = "Wonder|Auction - Actions")
	bool StartNewAuction(FName LotID, FString LotName, int64 BasePrice, float DurationSeconds = 30.0f);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Auction - Actions")
	bool PlacePlayerBid(FName LotID, int64 BidAmount);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Auction - Actions")
	bool PlaceBid(FName LotID, int64 BidAmount, const FString& BidderName);

	UFUNCTION(BlueprintPure, Category = "Wonder|Auction - Queries")
	bool GetAuctionLot(FName LotID, FLandAuctionLot& OutLot) const;

	/* --- AI RIVAL TUNING --- */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Auction - Tuning")
	float RivalBidIntervalSeconds = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder|Auction - Tuning")
	float RivalBidProbability = 0.65f;

	/* --- EVENTS --- */
	UPROPERTY(BlueprintAssignable, Category = "Wonder|Auction|Events")
	FOnAuctionStarted OnAuctionStarted;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|Auction|Events")
	FOnBidPlaced OnBidPlaced;

	UPROPERTY(BlueprintAssignable, Category = "Wonder|Auction|Events")
	FOnAuctionFinalized OnAuctionFinalized;

private:
	float RivalBidTimer = 0.0f;
	void SimulateRivalBids(float DeltaTime);
	void FinalizeAuction(FName LotID);
};
