// Copyright @subajat1 2026.

#pragma once

#include "Containers/Ticker.h"

struct FThreadSafeSharedState;

/**
 * Polls UE5's async loading subsystem at a fixed interval and writes progress [WIP]
 * Runs entirely on the game thread & never touches the render thread.
 * Owns its own FTSTicker handle; call Start() / Stop() from the ULoadingScreenSubsystem.
 */
class FLoadingProgressTracker
{
public:
	explicit FLoadingProgressTracker(TSharedPtr<FThreadSafeSharedState> inSharedState);
	~FLoadingProgressTracker();

public:
	void Start();
	void Stop();

	// handlers
private:
	bool Tick(float deltaTime);
	bool Tick2(float deltaTime);

	/**
	 * Called on game thread during each async loading flush
	 */
	void OnAsyncLoadingFlushUpdate();

private:
	TSharedPtr<FThreadSafeSharedState> sharedState;

	FTSTicker::FDelegateHandle tickerHandle;
	FTSTicker::FDelegateHandle tickerHandle2;
	FDelegateHandle flushUpdateHandle;

	// configs
private:
	float simulatedElapsed = 0.f;
	float smoothedProgress = 0.f;

};
