// Copyright @subajat1 2026.

#pragma once

#include "LoadingScreenDeveloperSettings.h"
#include "Structs/ThreadSafeSharedState.h"
#include "Widgets/Notifications/SProgressBar.h"

class SLoadingScreenWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SLoadingScreenWidget) : _SharedState(nullptr)
	                                         , _Settings(nullptr)
	                                         , _LogoTexture(nullptr)
		{
		}

		SLATE_ARGUMENT(TSharedPtr<FThreadSafeSharedState>, SharedState)
		SLATE_ARGUMENT(const ULoadingScreenDeveloperSettings*, Settings)
		SLATE_ARGUMENT(TArray<UTexture2D*>, BackgroundTextures)
		SLATE_ARGUMENT(UTexture2D*, LogoTexture)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs)
	{
		SharedState = InArgs._SharedState;
		Settings = InArgs._Settings;
		BackgroundTextures = InArgs._BackgroundTextures;
		LogoTexture = InArgs._LogoTexture;

		if (Settings)
		{
			InteractiveTipsHintText = FText::FromString(FString::Printf(TEXT("Press %s to see next tip."), *Settings->NextTipKeyboard.ToString()));

			if (Settings->bRequirePressToContinue)
			{
				InteractiveContinueHintText = FText::FromString(FString::Printf(TEXT("Press %s to continue."), *Settings->ContinueKeyKeyboard.ToString()));
			}
		}

		if (BackgroundTextures.Num() > 0)
		{
			const int32 StartIndex = Settings->bRandomizeBackgrounds ? FMath::RandRange(0, BackgroundTextures.Num() - 1) : 0;
			CurrentBackground = BackgroundTextures[StartIndex];
		}

		ChildSlot
		[
			SNew(SOverlay)

			// Background
			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SImage)
				.Image(this, &SLoadingScreenWidget::GetBackgroundBrush)
			]

			// Bottom Section
			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Bottom)
			.Padding(60.f, 0.f, 60.f, 60.f)
			[
				SNew(SVerticalBox)

#if !UE_BUILD_SHIPPING
				// DEBUG OVERLAY
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.f, 0.f, 0.f, 8.f)
				[
					SNew(STextBlock)
					.Text(this, &SLoadingScreenWidget::GetDebugText)
					.Visibility(this, &SLoadingScreenWidget::GetDebugOverlayVisibility)
					.ColorAndOpacity(FSlateColor(FLinearColor(1.f, 1.f, 0.f, 0.75f)))
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 20))
				]
#endif

				// A Tip to Display
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.f, 0.f, 0.f, 16.f)
				[
					SNew(STextBlock)
					.Text(this, &SLoadingScreenWidget::GetTipText)
					.AutoWrapText(true)
					.ColorAndOpacity(FSlateColor(FLinearColor::White))
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 16))
				]

				// The Progress Bar
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SProgressBar)
					.Percent(this, &SLoadingScreenWidget::GetProgress)
				]

				// Interactivity Hint (to Show Next Tip)
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.f, 8.f, 0.f, 0.f)
				[
					SNew(STextBlock)
					.Text(this, &SLoadingScreenWidget::GetPressToNextTipText)
					.Visibility(Settings && Settings->bShowTipHint ? EVisibility::HitTestInvisible : EVisibility::Collapsed)
					.ColorAndOpacity(FSlateColor(FLinearColor(1.f, 1.f, 1.f, 0.7f)))
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 12))
				]

				// Interactivity Hint (to Continue to gameplay)
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.f, 24.f, 0.f, 0.f)
				.HAlign(HAlign_Center)
				[
					SNew(STextBlock)
					.Text(this, &SLoadingScreenWidget::GetPressToContinueText)
					.Visibility(this, &SLoadingScreenWidget::GetPressToContinueVisibility)
					.ColorAndOpacity(FSlateColor(FLinearColor(1.f, 1.f, 1.f, 1.0f)))
					.Font(FCoreStyle::GetDefaultFontStyle("Bold", 24))
				]
			]
		];
	}

	// Attribute Binding Functions [render thread] --> read SharedState only
private:
	TOptional<float> GetProgress() const;
	FText GetTipText() const;
	FText GetPressToContinueText() const;
	FText GetPressToNextTipText() const;
	EVisibility GetPressToContinueVisibility() const;
	EVisibility GetDebugOverlayVisibility() const;
	FText GetDebugText() const;
	const FSlateBrush* GetBackgroundBrush() const;

	// Members — all set at Construct time on the game thread, read-only afterward
private:
	TSharedPtr<FThreadSafeSharedState> SharedState;
	const ULoadingScreenDeveloperSettings* Settings = nullptr;
	TArray<UTexture2D*> BackgroundTextures;
	UTexture2D* LogoTexture = nullptr;
	UTexture2D* CurrentBackground = nullptr;
	mutable bool bBrushInitialised = false;
	FText InteractiveTipsHintText;
	FText InteractiveContinueHintText;
};
