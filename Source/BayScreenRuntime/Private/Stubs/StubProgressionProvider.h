// Copyright @subajat1 2026.

#pragma once

#include "GameplayTagContainer.h"
#include "LoadingContextProvider.h"
#include "UObject/Object.h"

#include "StubProgressionProvider.generated.h"

/**
 * 
 */
UCLASS()
class UStubProgressionProvider : public UObject, public ILoadingContextProvider
{
	GENERATED_BODY()

public:
	virtual void ContributeContextTags_Implementation(FGameplayTagContainer& OutTags) const override
	{
		// Pull from whatever your game considers "progression"
		// The plugin never sees USaveGame, UMyPlayerStats, etc.
		// if (UMyPlayerStats* Stats = GetPlayerStats())
		// {
		// 	if (Stats->CurrentWorldIndex <= 3)
		// 		OutTags.AddTag(FGameplayTag::RequestGameplayTag("LoadingScreen.Progression.EarlyGame"));
		// 	else if (Stats->CurrentWorldIndex <= 7)
		// 		OutTags.AddTag(FGameplayTag::RequestGameplayTag("LoadingScreen.Progression.MidGame"));
		// 	// ... etc
		// }

		OutTags.AddTag(FGameplayTag::RequestGameplayTag("LoadingScreen.Situation.IdleInHub"));
	}
};
