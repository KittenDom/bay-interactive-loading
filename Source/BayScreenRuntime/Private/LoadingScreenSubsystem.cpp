// Copyright @subajat1 2026.

#include "LoadingScreenSubsystem.h"

#include "LoadingContextAggregator.h"
#include "LoadingProgressTracker.h"
#include "LoadingScreenDeveloperSettings.h"
#include "LoadingScreenInputProcessor.h"
#include "MoviePlayer.h"
#include "TipFilterService.h"
#include "TipHistoryService.h"
#include "TipSelectionService.h"
#include "Widgets/LoadingScreenWidget.h"

void ULoadingScreenSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	sharedState = MakeShared<FThreadSafeSharedState>();

	Aggregator = NewObject<ULoadingContextAggregator>(this);
	HistoryService = NewObject<UTipHistoryService>(this);
	FilterService = NewObject<UTipFilterService>(this);
	SelectionService = NewObject<UTipSelectionService>(this);
	SelectionService->Initialize(HistoryService);

	// ProgressTracker = new FLoadingProgressTracker(sharedState);

	_preLoadMapHandle = FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &ThisClass::OnPreLoadMap);
	_postLoadMapHandle = FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ThisClass::OnPostLoadMap);
}

void ULoadingScreenSubsystem::Deinitialize()
{
	if (bLoadingScreenActive)
	{
		EndLoadingScreen();
	}

	FCoreUObjectDelegates::PreLoadMap.Remove(_preLoadMapHandle);
	FCoreUObjectDelegates::PostLoadMapWithWorld.Remove(_postLoadMapHandle);

	delete ProgressTracker;
	ProgressTracker = nullptr;

	sharedState.Reset();

	Super::Deinitialize();
}

void ULoadingScreenSubsystem::OnPreLoadMap(const FString& mapURL)
{
	ProgressTracker = new FLoadingProgressTracker(sharedState);
	BeginLoadingScreen();
}

void ULoadingScreenSubsystem::OnPostLoadMap(UWorld* loadedWorld)
{
	if (!bLoadingScreenActive)
	{
		return;
	}

	const ULoadingScreenDeveloperSettings* setting = GetDefault<ULoadingScreenDeveloperSettings>();
	if (!setting)
	{
		return;
	}

	TArray<UTexture2D*> loadedBackgroundImages;
	{
		for (const FSoftObjectPath& Path : setting->BackgroundImages)
		{
			if (!Path.IsValid())
			{
				continue;
			}
			if (UTexture2D* Tex = Cast<UTexture2D>(Path.TryLoad()))
			{
				loadedBackgroundImages.Add(Tex);
			}
		}
	}

	UTexture2D* logoImage = nullptr;
	if (setting->LogoImage.IsValid())
	{
		logoImage = Cast<UTexture2D>(setting->LogoImage.TryLoad());
	}

	ProgressTracker->Stop(); // Always stop the progress tracker — loading is done
	sharedState->LoadProgress.Store(1.f); // debug // Drive progress to 100%

	if (!setting->bRequirePressToContinue)
	{
		EndLoadingScreen();
		return;
	}

	// --------- below this part -> when setting->bRequirePressToContinue
	if (_continueOverlayWidget.IsValid()) // Slate ContinueOverlayWidget exists alr
	{
		return;
	}

	bWaitingForContinue = true;
	sharedState->bWaitingForPressToContinue.Store(true);
	sharedState->bPressToContinueActivated.Store(false);

	if (GEngine && GEngine->GameViewport)
	{
		if (!_engineViewport.IsValid())
		{
			_engineViewport = GEngine->GameViewport;
		}

		_continueOverlayWidget = SNew(SLoadingScreenWidget)
			.SharedState(sharedState)
			.Settings(setting)
			.BackgroundTextures(loadedBackgroundImages)
			.LogoTexture(logoImage);

		if (_engineViewport.IsValid())
		{
			_engineViewport.Get()->AddViewportWidgetContent(_continueOverlayWidget.ToSharedRef(), 9999);

			UE_LOG(LogBayScreen, Warning, TEXT("[] SLoadingScreenWidget: ULoadingScreenSubsystem::OnPostLoadMap"));
		}
	}

	StartPressToContinuePoll();
}

bool ULoadingScreenSubsystem::TickPressToContinue(float deltaTime)
{
	if (!sharedState.IsValid())
	{
		UE_LOG(LogBayScreen, Error, TEXT("[Loading] SharedState is: INVALID in %hs"), __FUNCTION__);
		return false;
	}

	if (!sharedState->bPressToContinueActivated.Load())
	{
		return true;
	}

	if (_engineViewport.IsValid() && _continueOverlayWidget.IsValid())
	{
		_engineViewport->RemoveViewportWidgetContent(_continueOverlayWidget.ToSharedRef());
		_continueOverlayWidget.Reset();
	}

	EndLoadingScreen();
	return false;
}

void ULoadingScreenSubsystem::BeginLoadingScreen()
{
	if (!ProgressTracker || !sharedState.IsValid())
	{
		UE_LOG(LogBayScreen, Error, TEXT("Called before Initialize | %hs"), __FUNCTION__);
		return;
	}

	if (bLoadingScreenActive)
	{
		UE_LOG(LogBayScreen, Warning, TEXT("Called while already active | %hs"), __FUNCTION__);
		return;
	}
	bLoadingScreenActive = true;
	bWaitingForContinue = false;

	const ULoadingScreenDeveloperSettings* setting = GetDefault<ULoadingScreenDeveloperSettings>();

	HistoryService->IncrementLoadIndex();

	sharedState->LoadProgress.Store(0.f);
	sharedState->TipIndex.Store(0);
	sharedState->TipCount.Store(0);
	sharedState->FilteredTips.Empty();
	sharedState->SetAssetName(TEXT(""));
	sharedState->bWaitingForPressToContinue.Store(false);
	sharedState->bPressToContinueActivated.Store(false);

	BuildAndCommitTipSnapshot();

	TArray<UTexture2D*> LoadedBackgrounds;
	{
		for (const FSoftObjectPath& Path : setting->BackgroundImages)
		{
			if (Path.IsValid())
			{
				if (UTexture2D* Tex = Cast<UTexture2D>(Path.TryLoad()))
				{
					LoadedBackgrounds.Add(Tex);
				}
			}
		}
	}

	UTexture2D* LoadedLogo = nullptr;
	if (setting->LogoImage.IsValid())
	{
		LoadedLogo = Cast<UTexture2D>(setting->LogoImage.TryLoad());
	}

	ProgressTracker->Start();

	TWeakPtr<FThreadSafeSharedState> _weakState = sharedState;
	inputProcessor = MakeShared<FLoadingScreenInputProcessor>(_weakState, setting);
	FSlateApplication::Get().RegisterInputPreProcessor(inputProcessor, 0);

	// ── 6. Build Slate widget and hand to movie player
	FLoadingScreenAttributes loadAttrs;
	loadAttrs.MinimumLoadingScreenDisplayTime = setting->MinimumDisplayTime;
	loadAttrs.bAutoCompleteWhenLoadingCompletes = true;
	loadAttrs.bMoviesAreSkippable = false;
	loadAttrs.bWaitForManualStop = false;

	loadAttrs.WidgetLoadingScreen = SNew(SLoadingScreenWidget)
		.SharedState(sharedState)
		.Settings(setting)
		.BackgroundTextures(LoadedBackgrounds)
		.LogoTexture(LoadedLogo);
	GetMoviePlayer()->SetupLoadingScreen(loadAttrs);

	UE_LOG(LogBayScreen, Warning, TEXT("[] SLoadingScreenWidget: ULoadingScreenSubsystem::BeginLoadingScreen"));
}

void ULoadingScreenSubsystem::EndLoadingScreen()
{
	if (!bLoadingScreenActive)
	{
		return;
	}

	bLoadingScreenActive = false;
	bWaitingForContinue = false;

	StopPressToContinuePoll();

	if (_engineViewport.IsValid() && _continueOverlayWidget.IsValid())
	{
		_engineViewport->RemoveViewportWidgetContent(_continueOverlayWidget.ToSharedRef());
		_continueOverlayWidget.Reset();
	}

	if (inputProcessor.IsValid())
	{
		if (FSlateApplication::IsInitialized())
		{
			FSlateApplication::Get().UnregisterInputPreProcessor(inputProcessor);
		}
		inputProcessor.Reset();
	}

	const TArray<FLoadingTipEntry>& tips = sharedState->FilteredTips;
	if (!tips.IsEmpty())
	{
		const int32 currentIdx = sharedState->TipIndex.Load() % tips.Num();
		if (tips.IsValidIndex(currentIdx))
		{
			HistoryService->RecordTipShown(tips[currentIdx].TipID);
		}
	}

	if (GetMoviePlayer()) // Stop the movie player — this dismisses the screen
	{
		GetMoviePlayer()->StopMovie();
	}
}

void ULoadingScreenSubsystem::BuildAndCommitTipSnapshot() const
{
	const ULoadingScreenDeveloperSettings* setting = GetDefault<ULoadingScreenDeveloperSettings>();
	const FGameplayTagContainer context = Aggregator->BuildContext();

	TArray<FLoadingTipEntry> filtered = FilterService->Filter(setting->Tips, context, HistoryService);
	if (filtered.IsEmpty())
	{
		filtered = FilterService->Filter(setting->Tips, context, nullptr);
	}
	if (filtered.IsEmpty())
	{
		filtered = FilterService->FilterUniversal(setting->Tips);
	}

	TArray<FLoadingTipEntry> selected = SelectionService->Select(filtered);
	sharedState->FilteredTips = MoveTemp(selected);
	sharedState->TipCount.Store(sharedState->FilteredTips.Num());
	sharedState->TipIndex.Store(0);

	UE_LOG(LogBayScreen, Log, TEXT("[LoadingScreen] FilteredTips committed: %d  TipCount atomic: %d"), sharedState->FilteredTips.Num(), sharedState->TipCount.Load());
}

void ULoadingScreenSubsystem::FinaliseProgress()
{
	sharedState->LoadProgress.Store(1.f); // Drive progress to 1.0 so the bar fills on successful load.

	const ULoadingScreenDeveloperSettings* setting = GetDefault<ULoadingScreenDeveloperSettings>();
	if (!setting->bRequirePressToContinue)
	{
		return;
	}

	sharedState->bWaitingForPressToContinue.Store(true);
	sharedState->bPressToContinueActivated.Store(false);

	StartPressToContinuePoll();
}

void ULoadingScreenSubsystem::StartPressToContinuePoll()
{
	if (_pressToContinueTickerHandle.IsValid())
	{
		return;
	}

	_pressToContinueTickerHandle = FTSTicker::GetCoreTicker().AddTicker(
		FTickerDelegate::CreateUObject(this, &ULoadingScreenSubsystem::TickPressToContinue),
		1.f / 30.f);
}

void ULoadingScreenSubsystem::StopPressToContinuePoll()
{
	if (_pressToContinueTickerHandle.IsValid())
	{
		FTSTicker::GetCoreTicker().RemoveTicker(_pressToContinueTickerHandle);
		_pressToContinueTickerHandle.Reset();
	}
}
