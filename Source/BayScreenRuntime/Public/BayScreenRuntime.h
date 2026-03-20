// Copyright @subajat1 2026.

#pragma once

#include "Modules/ModuleManager.h"

class FBayScreenRuntimeModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

public:
	static FBayScreenRuntimeModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FBayScreenRuntimeModule>("BayScreenRuntimeModule");
	}

	static bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("BayScreenRuntimeModule");
	}

private:
	void OnPreLoadMap(const FString& mapNameStr);
	void OnPostLoadMapWithWorld(UWorld* loadedWorld);

private:
	FDelegateHandle _preLoadMapHandle;
	FDelegateHandle _postLoadMapHandle;
};
