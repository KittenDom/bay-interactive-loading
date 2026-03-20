// Copyright @subajat1 2026.

#pragma once

#include "UObject/Object.h"

#include "LoadingContextAggregator.generated.h"

UCLASS()
class ULoadingContextAggregator : public UObject
{
	GENERATED_BODY()

public:
	void RegisterProvider(TScriptInterface<class ILoadingContextProvider> provider);
	void UnregisterProvider(const TScriptInterface<ILoadingContextProvider>& provider);
	struct FGameplayTagContainer BuildContext() const;

private:
	TArray<TScriptInterface<ILoadingContextProvider>> providers;
};
