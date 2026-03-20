// Copyright @subajat1 2026.

#pragma once

#include "UObject/Interface.h"

#include "LoadingContextProvider.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class ULoadingContextProvider : public UInterface
{
	GENERATED_BODY()
};

class BAYSCREENRUNTIME_API ILoadingContextProvider
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Loading Screen")
	void ContributeContextTags(struct FGameplayTagContainer& outTags) const;
};
