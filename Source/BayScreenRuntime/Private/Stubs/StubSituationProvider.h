// Copyright @subajat1 2026.

#pragma once

#include "GameplayTagContainer.h"
#include "LoadingContextProvider.h"
#include "UObject/Object.h"

#include "StubSituationProvider.generated.h"

/**
 * 
 */
UCLASS()
class UStubSituationProvider : public UObject, public ILoadingContextProvider
{
	GENERATED_BODY()

public:
	virtual void ContributeContextTags_Implementation(FGameplayTagContainer& OutTags) const override
	{
		// example:
		// if (UMySessionTracker* ST = GetSessionTracker())
		// {
		// 	if (ST->DeathsThisSession >= 3)
		// 		OutTags.AddTag(TAG_LoadingScreen_Situation_DyingFrequently);
		// 	if (ST->bHasMissedSideQuests)
		// 		OutTags.AddTag(TAG_LoadingScreen_Situation_MissingSideQuests);
		// }
		OutTags.AddTag(FGameplayTag::RequestGameplayTag("LoadingScreen.Progression.EarlyGame"));
	}
};
