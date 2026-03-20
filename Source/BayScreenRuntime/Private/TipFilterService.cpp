// Copyright @subajat1 2026.


#include "TipFilterService.h"

#include "TipHistoryService.h"
#include "Structs/LoadingTipEntry.h"

TArray<FLoadingTipEntry> UTipFilterService::Filter(const TArray<FLoadingTipEntry>& allTips, const FGameplayTagContainer& inContext, const UTipHistoryService* historyService) const
{
	TArray<FLoadingTipEntry> _result;
	_result.Reserve(allTips.Num());

	for (const FLoadingTipEntry& tip : allTips)
	{
		// Guard: skip tips with no text
		if (tip.TipText.IsEmpty())
		{
			continue;
		}

		// Guard: skip tips with invalid GUID — history tracking would be broken
		if (!tip.TipID.IsValid())
		{
			continue;
		}

		// Tag predicate check — RequiredTags (AND) and BlockedByTags (OR)
		if (!tip.MatchesContext(inContext))
		{
			continue;
		}

		// Cooldown check — only applied when a history service is provided
		if (historyService && historyService->IsTipOnCooldown(tip, tip.TipID))
			continue;

		_result.Add(tip);
	}

	return _result;
}

TArray<FLoadingTipEntry> UTipFilterService::FilterUniversal(const TArray<FLoadingTipEntry>& allTips) const
{
	TArray<FLoadingTipEntry> _result;

	for (const FLoadingTipEntry& tip : allTips)
	{
		if (tip.TipText.IsEmpty())
		{
			continue;
		}

		if (!tip.TipID.IsValid())
		{
			continue;
		}

		if (tip.RequiredTags.IsEmpty() && tip.BlockedByTags.IsEmpty())
		{
			_result.Add(tip);
		}
	}

	return _result;
}
