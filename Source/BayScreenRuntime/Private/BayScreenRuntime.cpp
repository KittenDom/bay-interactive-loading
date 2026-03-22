// Copyright @subajat1 2026.

#include "BayScreenRuntime.h"

#include "BayScreenLog.h"
#include "LoadingScreenDeveloperSettings.h"
#include "LoadingScreenSubsystem.h"
#include "MoviePlayer.h"
#include "Widgets/LoadingScreenWidget.h"

#define LOCTEXT_NAMESPACE "FBayScreenRuntimeModule"

void FBayScreenRuntimeModule::StartupModule()
{
	if (IsRunningDedicatedServer() || IsRunningCommandlet())
	{
		UE_LOG(LogBayScreen, Log, TEXT("Skipping module startup - dedicated server or commandlet. [%hs]"), __FUNCTION__);
		return;
	}

	if (!GetMoviePlayer())
	{
		UE_LOG(LogBayScreen, Warning, TEXT("MoviePlayer is not available at StartupModule. [%hs]"), __FUNCTION__);
		return;
	}

	FCoreUObjectDelegates::PreLoadMap.AddRaw(this, &FBayScreenRuntimeModule::OnPreLoadMap);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddRaw(this, &FBayScreenRuntimeModule::OnPostLoadMapWithWorld);

	UE_LOG(LogBayScreen, Log, TEXT("BayScreenRuntimeModule module started. [%hs]"), __FUNCTION__);
}

void FBayScreenRuntimeModule::ShutdownModule()
{
	if (_preLoadMapHandle.IsValid())
	{
		FCoreUObjectDelegates::PreLoadMap.Remove(_preLoadMapHandle);
		_preLoadMapHandle.Reset();
	}

	if (_postLoadMapHandle.IsValid())
	{
		FCoreUObjectDelegates::PostLoadMapWithWorld.Remove(_postLoadMapHandle);
		_postLoadMapHandle.Reset();
	}

	UE_LOG(LogBayScreen, Log, TEXT("BayScreenRuntimeModule module shut down. [%hs]"), __FUNCTION__);
}

void FBayScreenRuntimeModule::OnPreLoadMap(const FString& mapNameStr)
{
	// GUARD: LoadingScreenSubsystem
	if (GEngine)
	{
		for (const FWorldContext& ctx : GEngine->GetWorldContexts())
		{
			if (ctx.OwningGameInstance)
			{
				if (const ULoadingScreenSubsystem* loadingSystem = ctx.OwningGameInstance->GetSubsystem<ULoadingScreenSubsystem>())
				{
					return;
				}
			}
		}
	}

	const ULoadingScreenDeveloperSettings* settings = GetDefault<ULoadingScreenDeveloperSettings>();
	if (!settings)
	{
		return;
	}

	TSharedPtr<FThreadSafeSharedState> _startupSharedState = MakeShared<FThreadSafeSharedState>();
	_startupSharedState->TipCount.Store(0);
	_startupSharedState->TipIndex.Store(0);
	_startupSharedState->LoadProgress.Store(0.f);

	FLoadingScreenAttributes loadingAttrs;
	loadingAttrs.MinimumLoadingScreenDisplayTime = settings->MinimumDisplayTime;
	loadingAttrs.bAutoCompleteWhenLoadingCompletes = true;
	loadingAttrs.bMoviesAreSkippable = false;
	loadingAttrs.bWaitForManualStop = false;

	loadingAttrs.WidgetLoadingScreen = SNew(SLoadingScreenWidget)
		.SharedState(_startupSharedState)
		.Settings(settings);

	GetMoviePlayer()->SetupLoadingScreen(loadingAttrs);

	UE_LOG(LogBayScreen, Warning, TEXT("[] SLoadingScreenWidget: FBayScreenRuntimeModule::OnPreLoadMap !! Ideally this one should never been called, please check onULoadingScreenSubsystem !!"));
}

void FBayScreenRuntimeModule::OnPostLoadMapWithWorld(UWorld* loadedWorld)
{
	// TODO: placeholder
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FBayScreenRuntimeModule, BayScreenRuntime)
