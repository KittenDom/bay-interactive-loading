// Copyright @subajat1 2026.

#pragma once

#include "Structs/LoadingTipEntry.h"
#include "Structs/TipHistoryData.h"
#include "UObject/Object.h"

#include "TipHistoryService.generated.h"

USTRUCT()
struct FTipHistoryEntry
{
	GENERATED_BODY()

public:
	UPROPERTY()
	int32 TimesShown = 0;
	UPROPERTY()
	int32 LastShownLoadIndex = 0;
};


UCLASS()
class UTipHistoryService : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Called by UTipSelectionService after a tip is shown
	 */
	void RecordTipShown(const FGuid& tipID);


	/**
	 * Called by UTipFilterService to prune already-recent tips
	 * @return 
	 */
	bool IsTipOnCooldown(const FLoadingTipEntry& tip, const FGuid& tipID) const;

	/**
	 * Serialize this to your save game — the plugin exposes a raw struct
	 * your game can embed in its own USaveGame subclass
	 * @return 
	 */
	FTipHistoryData GetSerializableData() const;
	void LoadFromData(const FTipHistoryData& data);

	void IncrementLoadIndex() { globalLoadIdx++; }

private:
	TMap<FGuid, FTipHistoryEntry> history;
	int32 globalLoadIdx = 0;
};
