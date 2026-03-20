// Copyright @subajat1 2026.


#include "LoadingContextAggregator.h"

#include "LoadingContextProvider.h"
#include "GameplayTagContainer.h"

void ULoadingContextAggregator::RegisterProvider(TScriptInterface<ILoadingContextProvider> provider)
{
	// TODO register check
	providers.Emplace(provider);
}

void ULoadingContextAggregator::UnregisterProvider(const TScriptInterface<ILoadingContextProvider>& provider)
{
	// TODO unregister check
	if (providers.Contains(provider))
	{
		providers.Remove(provider);
	}
}

FGameplayTagContainer ULoadingContextAggregator::BuildContext() const
{
	FGameplayTagContainer tags;
	for (const TScriptInterface<ILoadingContextProvider> p : providers)
	{
		FGameplayTagContainer tag;
		p.GetInterface()->ContributeContextTags(tag);
		tags.AppendTags(tag);
	}

	return tags;
}
