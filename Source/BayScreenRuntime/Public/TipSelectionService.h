// Copyright @subajat1 2026.

#pragma once

#include "UObject/Object.h"

#include "TipSelectionService.generated.h"

/**
 * 
 */
UCLASS()
class UTipSelectionService : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(class UTipHistoryService* inHistory) { HistoryService = inHistory; }
	TArray<struct FLoadingTipEntry> Select(const TArray<FLoadingTipEntry>& filteredTips) const;

private:
	/**
	 * @role: Shuffle within each priority band
	 * Ensures that when multiple situational tips share the same priority,
	 * the player sees them in a different order each load rather than
	 * always seeing the first one authored in developer settings
	 * @param tips ya tips lah bray
	 */
	static void ShuffleWithinPriorityBands(TArray<FLoadingTipEntry>& tips);

private:
	UPROPERTY()
	UTipHistoryService* HistoryService = nullptr;

	UPROPERTY()
	TArray<FGuid> TipGUIDs;
};
