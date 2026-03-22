// Copyright @subajat1 2026.


#include "LoadingProgressTracker.h"

#include "BayScreenLog.h"
#include "LoadingScreenDeveloperSettings.h"
#include "Structs/ThreadSafeSharedState.h"

#include "UObject/UObjectGlobals.h"
#include "Misc/CoreDelegates.h"

#define LOAD_TRACKER_TICK_INTERVAL .1f

FLoadingProgressTracker::FLoadingProgressTracker(TSharedPtr<FThreadSafeSharedState> inSharedState) : sharedState(MoveTemp(inSharedState))
{
	UE_LOG(LogBayScreen, Warning, TEXT("FLoadingProgressTracker::ctor sharedState: %s"), sharedState.IsValid() ? *sharedState.Get()->CurrentAssetName : TEXT("invalid"));
}

FLoadingProgressTracker::~FLoadingProgressTracker()
{
	Stop();
}

void FLoadingProgressTracker::Start()
{
	check(!tickerHandle.IsValid()); // to prevent start more than once

	UE_LOG(LogBayScreen, Warning, TEXT("FLoadingProgressTracker::Start done"));

	simulatedElapsed = 0.f;
	smoothedProgress = 0.f;

	//tickerHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &FLoadingProgressTracker::Tick), 0.f);
	tickerHandle2 = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &FLoadingProgressTracker::Tick2), 0.f);

	UE_LOG(LogBayScreen, Warning, TEXT("FLoadingProgressTracker::Start binding tick"));

	// flushUpdateHandle = FCoreDelegates::OnAsyncLoadingFlushUpdate.AddLambda([this]()
	// {
	// 	OnAsyncLoadingFlushUpdate();
	// });
}

void FLoadingProgressTracker::Stop()
{
	if (tickerHandle.IsValid())
	{
		FTSTicker::GetCoreTicker().RemoveTicker(tickerHandle);
		tickerHandle.Reset();
	}

	if (flushUpdateHandle.IsValid())
	{
		FCoreDelegates::OnAsyncLoadingFlushUpdate.Remove(flushUpdateHandle);
		flushUpdateHandle.Reset();
	}
}

bool FLoadingProgressTracker::Tick(float deltaTime)
{
	// if (!sharedState.IsValid())
	// {
	// 	UE_LOG(LogBayScreen, Warning, TEXT("FLoadingProgressTracker::Tick sharedState: %s"), sharedState.IsValid() ? *sharedState.Get()->CurrentAssetName : TEXT("invalid"));
	// 	return false; // SharedState destroyed -> stop ticking
	// }

	UE_LOG(LogBayScreen, Warning, TEXT("FLoadingProgressTracker::Tick ticking..."));

	const ULoadingScreenDeveloperSettings* setting = GetDefault<ULoadingScreenDeveloperSettings>();

#if !UE_BUILD_SHIPPING
	// Simulated load -> overrides real async load percentage
	if (setting && setting->SimulatedLoadDurationSeconds > 0.f)
	{
		simulatedElapsed += deltaTime;
		const float simulatedProgress = FMath::Clamp(simulatedElapsed / setting->SimulatedLoadDurationSeconds, 0.f, 0.95f);
		sharedState->LoadProgress.Store(simulatedProgress);
		UE_LOG(LogBayScreen, Warning, TEXT("FLoadingProgressTracker::Tick: progress: %f"), simulatedProgress);

		return true;
	}
#endif

	const float RawPercent = GetAsyncLoadPercentage(NAME_None);
	float Progress = 0.f;

	if (RawPercent >= 0.f)
	{
		// GetAsyncLoadPercentage returns 0-100; clamp to [0, 0.95] until PostLoadMapWithWorld fires. 
		// This prevents the bar hitting 100% while the world is still initializing, which looks like a freeze.
		const float clampedProgress = FMath::Clamp(RawPercent / 100.f, 0.f, .95f);
		sharedState->LoadProgress.Store(clampedProgress);
		smoothedProgress = clampedProgress;
	}
	else
	{
		// No async load percentage available — advance a smooth estimator
		// that asymptotically approaches 0.9 so the bar always moves.
		// Formula: progress += (target - current) * rate * dt
		// This gives a natural deceleration as it approaches the cap.
		const float _target = 0.9f; // wip tuning
		const float _rate = 0.4f; // tune this — higher = faster initial movement
		smoothedProgress += (_target - smoothedProgress) * _rate * deltaTime;
		smoothedProgress = FMath::Clamp(smoothedProgress, 0.f, 0.9f);
		sharedState->LoadProgress.Store(smoothedProgress);
	}

	// Fallback: if the MoviePlayer reports nothing, try async load percentage
	if (Progress <= 0.f)
	{
		const float fallbackPercent = GetAsyncLoadPercentage(NAME_None);
		if (fallbackPercent >= 0.f)
		{
			Progress = fallbackPercent / 100.f;
		}
	}

#if !UE_BUILD_SHIPPING
	sharedState->SetAssetName(FString::Printf(
		TEXT("Loading... %.0f%%  (async packages: %d)"),
		sharedState->LoadProgress.Load() * 100.f,
		FMath::Max(0, (int32)GetNumAsyncPackages())));
#endif

	// Clamp to 0.95 — FinaliseProgress drives it to 1.0 on PostLoadMapWithWorld
	sharedState->LoadProgress.Store(FMath::Clamp(Progress, 0.f, 0.95f));
	return true;
}

bool FLoadingProgressTracker::Tick2(float deltaTime)
{
	const ULoadingScreenDeveloperSettings* setting = GetDefault<ULoadingScreenDeveloperSettings>();

#if !UE_BUILD_SHIPPING // Simulated Loading -> overrides real async load percentage
	if (setting && setting->SimulatedLoadDurationSeconds > 0.f)
	{
		simulatedElapsed += deltaTime;
		const float simulatedProgress = FMath::Clamp(simulatedElapsed / setting->SimulatedLoadDurationSeconds, 0.f, 1.f);
		sharedState->LoadProgress.Store(simulatedProgress);

		const bool bLoadingDone = simulatedProgress >= 1.f;
		sharedState->bWaitingForPressToContinue.Store(bLoadingDone);

		if (!bLoadingDone)
		{
			const int32 randomizedPackageID = FMath::RandRange(1000, 99999);
			if (randomizedPackageID > 80000)
			{
				const FString assetLoadStr = FString::Printf(TEXT("Loading... %.0f%%  (async package ID: %d)"), sharedState->LoadProgress.Load() * 100.f, randomizedPackageID);
				sharedState->SetAssetName(assetLoadStr);
			}
		}
		else
		{
			sharedState->SetAssetName(TEXT(""));
		}

		// UE_LOG(LogBayScreen, Warning, TEXT("FLoadingProgressTracker::Tick: progress: %f | asset loading: %s | bShowCurrentAssetName: [%s]"), simulatedProgress, *sharedState->GetAssetName(), setting->bShowCurrentAssetName ? TEXT("true") : TEXT("false"));

		return true;
	}
#endif

	/// *** BEGIN: PACKAGE LOAD TRACKING
	const float RawPercent = GetAsyncLoadPercentage(NAME_None);
	float Progress = 0.f;

	if (RawPercent >= 0.f)
	{
		// GetAsyncLoadPercentage returns 0-100; clamp to [0, 0.95] until PostLoadMapWithWorld fires. 
		// This prevents the bar hitting 100% while the world is still initializing, which looks like a freeze.
		const float clampedProgress = FMath::Clamp(RawPercent / 100.f, 0.f, .95f);
		sharedState->LoadProgress.Store(clampedProgress);
		smoothedProgress = clampedProgress;
	}
	else
	{
		// No async load percentage available — advance a smooth estimator
		// that asymptotically approaches 0.9 so the bar always moves.
		// Formula: progress += (target - current) * rate * dt
		// This gives a natural deceleration as it approaches the cap.
		const float _target = 0.9f; // wip tuning
		const float _rate = 0.4f; // tune this — higher = faster initial movement
		smoothedProgress += (_target - smoothedProgress) * _rate * deltaTime;
		smoothedProgress = FMath::Clamp(smoothedProgress, 0.f, 0.9f);
		sharedState->LoadProgress.Store(smoothedProgress);
	}

	// Fallback: if the MoviePlayer reports nothing, try async load percentage
	if (Progress <= 0.f)
	{
		const float fallbackPercent = GetAsyncLoadPercentage(NAME_None);
		if (fallbackPercent >= 0.f)
		{
			Progress = fallbackPercent / 100.f;
		}
	}

#if !UE_BUILD_SHIPPING
	sharedState->SetAssetName(FString::Printf(
		TEXT("Loading... %.0f%%  (async packages: %d)"),
		sharedState->LoadProgress.Load() * 100.f,
		FMath::Max(0, (int32)GetNumAsyncPackages())));
#endif

	// Clamp to 0.95 — FinaliseProgress drives it to 1.0 on PostLoadMapWithWorld
	sharedState->LoadProgress.Store(FMath::Clamp(Progress, 0.f, 0.95f));
	
	/// *** END: PACKAGE LOAD TRACKING

	return true;
}

void FLoadingProgressTracker::OnAsyncLoadingFlushUpdate()
{
	if (!sharedState.IsValid())
		return;

	// GetAsyncLoadingFlushCount / active packages vary by UE version.
	// We query the lowest-overhead API available: the active package count
	// reported by the async loading thread. For the PoC debug display we
	// grab the first active package name from the internal loading queue.
#if !UE_BUILD_SHIPPING
	const int32 Pending = GetNumAsyncPackages();
	if (Pending > 0)
	{
		const FString DisplayName = FString::Printf(TEXT("Loading packages: %d remaining"), Pending);
		sharedState->SetAssetName(DisplayName);
	}
#endif
}
