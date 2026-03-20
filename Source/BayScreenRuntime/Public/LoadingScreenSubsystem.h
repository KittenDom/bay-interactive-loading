// Copyright @subajat1 2026.

#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "Structs/ThreadSafeSharedState.h"

#include "LoadingScreenSubsystem.generated.h"

class FLoadingProgressTracker;
class FLoadingScreenInputProcessor;
class ULoadingContextAggregator;
class UTipHistoryService;
class UTipFilterService;
class UTipSelectionService;
class ULoadingScreenDeveloperSettings;

/**
 * Role: As orchestrator only
 * Owns all plugin services and manages the loading screen lifecycle.
 */

UCLASS()
class BAYSCREENRUNTIME_API ULoadingScreenSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

public:
	TSharedPtr<FThreadSafeSharedState> GetSharedState() const { return sharedState; }
	ULoadingContextAggregator* GetAggregator() const { return Aggregator; }
	UTipHistoryService* GetHistoryService() const { return HistoryService; }

	// delegate handlers
private:
	void OnPreLoadMap(const FString& mapURL);
	void OnPostLoadMap(UWorld* loadedWorld);

	bool TickPressToContinue(float deltaTime);

private:
	void BeginLoadingScreen();
	void EndLoadingScreen();
	void BuildAndCommitTipSnapshot() const;
	void FinaliseProgress();
	void StartPressToContinuePoll();
	void StopPressToContinuePoll();

private:
	UPROPERTY()
	TObjectPtr<ULoadingContextAggregator> Aggregator;
	UPROPERTY()
	TObjectPtr<UTipFilterService> FilterService;
	UPROPERTY()
	TObjectPtr<UTipHistoryService> HistoryService;
	UPROPERTY()
	TObjectPtr<UTipSelectionService> SelectionService;

	FLoadingProgressTracker* ProgressTracker = nullptr;

	TSharedPtr<FThreadSafeSharedState> sharedState;
	TSharedPtr<FLoadingScreenInputProcessor> inputProcessor;

	// handles
private:
	FDelegateHandle _preLoadMapHandle;
	FDelegateHandle _postLoadMapHandle;
	FTSTicker::FDelegateHandle _pressToContinueTickerHandle;

	// flags
private:
	bool bLoadingScreenActive = false;
	bool bWaitingForContinue = false;

	// caches
private:
	TSharedPtr<class SLoadingScreenWidget> _continueOverlayWidget;
	TWeakObjectPtr<UGameViewportClient> _engineViewport;
};
