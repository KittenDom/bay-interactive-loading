#pragma once

#include "TipHistoryData.generated.h"

USTRUCT(BlueprintType)
struct FTipHistoryData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) // TODO: SaveGame
	TMap<FGuid, int32> LastShownLoadIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) // TODO: SaveGame
	int32 GlobalLoadIndex;
};
