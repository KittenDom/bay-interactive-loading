// Copyright @subajat1 2026.

#pragma once

#include "LoadingTipEntry.h"

struct FThreadSafeSharedState
{
	TAtomic<float> LoadProgress{0.f};
	TAtomic<int32> TipIndex{0};
	TAtomic<int32> TipCount{0};
	TArray<FLoadingTipEntry> FilteredTips;

	FCriticalSection AssetNameLock;
	FString CurrentAssetName;

	TAtomic<bool> bTipDirty{false};
	TAtomic<bool> bWaitingForPressToContinue{false};
	TAtomic<bool> bPressToContinueActivated{false};

	void SetAssetName(const FString& nameStr)
	{
		FScopeLock Lock(&AssetNameLock);
		CurrentAssetName = nameStr;
	}

	FString GetAssetName() const
	{
		const UE::FWindowsRecursiveMutex* Lock(&AssetNameLock);
		return CurrentAssetName;
	}

	void AdvanceTip()
	{
		const int32 count = TipCount.Load();
		if (count > 0)
		{
			TipIndex.Store((TipIndex.Load() + 1) % count);
		}
		bTipDirty.Store(true);
	}
};
