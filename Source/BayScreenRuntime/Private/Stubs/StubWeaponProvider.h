// Copyright @subajat1 2026.

#pragma once

#include "GameplayTagContainer.h"
#include "LoadingContextProvider.h"
#include "UObject/Object.h"

#include "StubWeaponProvider.generated.h"

/**
 * 
 */
UCLASS()
class UStubWeaponProvider : public UObject, public ILoadingContextProvider
{
	GENERATED_BODY()

public:
	virtual void ContributeContextTags_Implementation(FGameplayTagContainer& OutTags) const override
	{
		// example:
		// if (UMyWeaponComponent* WC = GetEquippedWeaponComponent())
		// {
		// 	// Map your internal weapon type to a loading screen tag
		// 	OutTags.AddTag(WC->GetLoadingScreenTag());
		// }
		OutTags.AddTag(FGameplayTag::RequestGameplayTag("LoadingScreen.Weapon.Magic"));
	}
};
