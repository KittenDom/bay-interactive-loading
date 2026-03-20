// Copyright @subajat1 2026.

#pragma once

#include "TipHistoryService.h"

void UTipHistoryService::RecordTipShown(const FGuid& tipID)
{
	FTipHistoryEntry& Entry = history.FindOrAdd(tipID);
	Entry.TimesShown++;
	Entry.LastShownLoadIndex = globalLoadIdx;
}

bool UTipHistoryService::IsTipOnCooldown(const FLoadingTipEntry& tip, const FGuid& tipID) const
{
	if (tip.CooldownLoadCount <= 0)
	{
		return false;
	}
	const FTipHistoryEntry* Entry = history.Find(tipID);
	if (!Entry)
	{
		return false;
	}
	return (globalLoadIdx - Entry->LastShownLoadIndex) < tip.CooldownLoadCount;
}

FTipHistoryData UTipHistoryService::GetSerializableData() const
{
	// TODO
	return FTipHistoryData();
}

void UTipHistoryService::LoadFromData(const FTipHistoryData& data)
{
	// TODO
}
