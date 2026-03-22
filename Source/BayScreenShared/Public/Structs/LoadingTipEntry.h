#pragma once

#include "GameplayTagContainer.h"

#include "LoadingTipEntry.generated.h"

USTRUCT(BlueprintType)
struct FLoadingTipEntry
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGuid TipID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MultiLine = true))
	FText TipText;

	/**
	 * This tip shows when ALL of these tags are present in the context.
	 * e.g., LoadingScreen.Situation.DyingFrequently + LoadingScreen.Weapon.Melee
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer RequiredTags;

	/**
	 * This tip is suppressed if ANY of these tags are present
	 * e.g., hide beginner tips once LoadingScreen.Progression.MidGame is set
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer BlockedByTags;

	/**
	 * Higher = shown first within the same context match
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Priority = 0;

	/**
	 * Cooldown: don't show this tip again for N loading screens [0 = no cooldown]
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CooldownLoadCount = 0;

	bool MatchesContext(const FGameplayTagContainer& contextTags) const
	{
		if (!RequiredTags.IsEmpty() && !contextTags.HasAll(RequiredTags))
		{
			return false;
		}
		if (!BlockedByTags.IsEmpty() && contextTags.HasAny(BlockedByTags))
		{
			return false;
		}
		return true;
	}
};
