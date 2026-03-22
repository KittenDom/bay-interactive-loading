// Copyright @subajat1 2026.


#include "LoadingScreenDeveloperSettings.h"

ULoadingScreenDeveloperSettings::ULoadingScreenDeveloperSettings()
{
	CategoryName = TEXT("Plugins");
	SectionName = TEXT("Bay - [Loading-] Screen Settings");
}

#if WITH_EDITOR
void ULoadingScreenDeveloperSettings::PostEditChangeProperty(FPropertyChangedEvent& e)
{
	Super::PostEditChangeProperty(e);

	bool bDirty = false;
	for (FLoadingTipEntry& tip : Tips)
	{
		if (!tip.TipID.IsValid())
		{
			tip.TipID = FGuid::NewGuid();
			bDirty = true;
		}
	}

	if (bDirty)
	{
		SaveConfig();
	}
}
#endif
