// Copyright @subajat1 2026.

#pragma once

#include "UObject/Object.h"
#include "GameplayTagContainer.h"

#include "TipFilterService.generated.h"

/**
 * 
 */
UCLASS()
class BAYSCREENRUNTIME_API UTipFilterService : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Returns all tips from AllTips whose tag predicates match Context
	 * and are not currently on cooldown according to HistoryService.
	 *
	 * @param allTips        The full tip library from developer settings.
	 * @param inContext        The merged tag container from ULoadingContextAggregator.
	 * @param historyService Optional. If null, cooldown checks are skipped.
	 *
	 * @return Filtered list. Order matches the input order — no sorting applied.
	 */
	TArray<struct FLoadingTipEntry> Filter(const TArray<FLoadingTipEntry>& allTips, const FGameplayTagContainer& inContext, const class UTipHistoryService* historyService = nullptr) const;

	/**
	 * Convenience overload: returns only universal fallback tips —
	 * tips with empty RequiredTags and empty BlockedByTags.
	 * Used by UTipSelectionService when context filtering yields nothing.
	 */
	TArray<FLoadingTipEntry> FilterUniversal(const TArray<FLoadingTipEntry>& allTips) const;
};
