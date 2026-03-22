// Copyright @subajat1 2026.

#include "TipSelectionService.h"

#include "Structs/LoadingTipEntry.h"

TArray<FLoadingTipEntry> UTipSelectionService::Select(const TArray<FLoadingTipEntry>& filteredTips) const
{
	if (filteredTips.IsEmpty())
	{
		return {};
	}

	TArray<FLoadingTipEntry> result = filteredTips;
	result.StableSort([](const FLoadingTipEntry& A, const FLoadingTipEntry& B)
	{
		return A.Priority > B.Priority;
	});

	ShuffleWithinPriorityBands(result);
	return result;
}

void UTipSelectionService::ShuffleWithinPriorityBands(TArray<FLoadingTipEntry>& tips)
{
	if (tips.Num() <= 1)
	{
		return;
	}

	int32 _bandStart = 0;
	for (int32 i = 1; i <= tips.Num(); ++i)
	{
		const bool bBandEnded = (i == tips.Num()) || (tips[i].Priority != tips[_bandStart].Priority);

		if (!bBandEnded)
		{
			continue;
		}

		const int32 _bandEnd = i - 1; // inclusive
		const int32 _bandSize = _bandEnd - _bandStart + 1;
		if (_bandSize > 1)
		{
			// Fisher-Yates shuffle over [_bandStart, _bandEnd]
			for (int32 j = _bandEnd; j > _bandStart; --j)
			{
				const int32 k = FMath::RandRange(_bandStart, j);
				if (k != j)
				{
					tips.Swap(j, k);
				}
			}
		}
		_bandStart = i;
	}
}
