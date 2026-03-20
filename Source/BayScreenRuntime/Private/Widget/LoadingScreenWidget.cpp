// Copyright @subajat1 2026.

#include "Widgets/LoadingScreenWidget.h"
#include "Brushes/SlateDynamicImageBrush.h"
#include "Slate/DeferredCleanupSlateBrush.h"

TOptional<float> SLoadingScreenWidget::GetProgress() const
{
	return SharedState ? TOptional<float>(SharedState->LoadProgress.Load()) : TOptional<float>();
}

FText SLoadingScreenWidget::GetTipText() const
{
	if (!SharedState.IsValid())
	{
		return FText::GetEmpty();
	}

	const TArray<FLoadingTipEntry>& Tips = SharedState->FilteredTips;
	if (Tips.IsEmpty())
	{
		return FText::GetEmpty();
	}

	const int32 Count = SharedState->TipCount.Load();
	if (Count <= 0)
	{
		return FText::GetEmpty();
	}

	const int32 Index = SharedState->TipIndex.Load() % Count;
	if (!Tips.IsValidIndex(Index))
	{
		return FText::GetEmpty();
	}

	return Tips[Index].TipText;
}

EVisibility SLoadingScreenWidget::GetDebugOverlayVisibility() const
{
#if !UE_BUILD_SHIPPING
	if (!Settings)
	{
		return EVisibility::Collapsed;
	}
	return Settings->bShowCurrentAssetName ? EVisibility::HitTestInvisible : EVisibility::Collapsed;
#else
	return EVisibility::Collapsed;
#endif
}

FText SLoadingScreenWidget::GetDebugText() const
{
	if (!SharedState.IsValid())
	{
		return FText::GetEmpty();
	}
	return FText::FromString(SharedState->GetAssetName());
}

EVisibility SLoadingScreenWidget::GetPressToContinueVisibility() const
{
	if (!SharedState.IsValid())
	{
		return EVisibility::Collapsed;
	}

	return SharedState->bWaitingForPressToContinue.Load() ? EVisibility::HitTestInvisible : EVisibility::Collapsed;
}

FText SLoadingScreenWidget::GetPressToContinueText() const
{
	return InteractiveContinueHintText;
}

FText SLoadingScreenWidget::GetPressToNextTipText() const
{
	return InteractiveTipsHintText;
}

const FSlateBrush* SLoadingScreenWidget::GetBackgroundBrush() const
{
	if (!CurrentBackground)
	{
		return FStyleDefaults::GetNoBrush();
	}
	const TSharedPtr<FDeferredCleanupSlateBrush> DeferredBrush = FDeferredCleanupSlateBrush::CreateBrush(CurrentBackground, FVector2D(CurrentBackground->GetSizeX(), CurrentBackground->GetSizeY()));

	return DeferredBrush.IsValid() ? DeferredBrush->GetSlateBrush() : FStyleDefaults::GetNoBrush();
}
