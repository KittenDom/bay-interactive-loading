// Copyright @subajat1 2026.

#pragma once

#include "Engine/DeveloperSettings.h"
#include "Structs/LoadingTipEntry.h"

#include "LoadingScreenDeveloperSettings.generated.h"

UCLASS(Config=BayLoadingScreen, DefaultConfig, meta = (DisplayName = "Bay - Loading Screen Settings"))
class ULoadingScreenDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	ULoadingScreenDeveloperSettings();

public:
	UPROPERTY(Config, EditAnywhere, Category = "Visuals", meta = (AllowedClasses = "Texture2D", DisplayName = "Background images"))
	TArray<FSoftObjectPath> BackgroundImages;

	UPROPERTY(Config, EditAnywhere, Category = "Visuals", meta = (AllowedClasses = "Texture2D", DisplayName = "Logo / overlay image"))
	FSoftObjectPath LogoImage;

	UPROPERTY(Config, EditAnywhere, Category = "Visuals")
	FLinearColor ProgressBarTint = FLinearColor(1.f, 1.f, 1.f, 1.f);

	UPROPERTY(Config, EditAnywhere, Category = "Visuals", meta = (DisplayName = "Cycle backgrounds randomly"))
	bool bRandomizeBackgrounds = true;

	UPROPERTY(Config, EditAnywhere, Category = "Tips", meta = (TitleProperty = "TipText", DisplayName = "Tip entries"))
	TArray<FLoadingTipEntry> Tips;

	UPROPERTY(Config, EditAnywhere, Category = "Tips", meta = (DisplayName = "Input key: next tip (keyboard)"))
	FKey NextTipKeyboard = EKeys::X;

	UPROPERTY(Config, EditAnywhere, Category = "Tips", meta = (DisplayName = "Input key: next tip (gamepad)"))
	FKey NextTipGamepad = EKeys::Gamepad_FaceButton_Left;

	UPROPERTY(Config, EditAnywhere, Category = "Tips", meta = (DisplayName = "Show tip hint label"))
	bool bShowTipHint = true;

	UPROPERTY(Config, EditAnywhere, Category = "Progress", meta = (DisplayName = "Show loading percentage text"))
	bool bShowPercentageText = false;

	UPROPERTY(Config, EditAnywhere, Category = "Progress - Debug", meta = (DisplayName = "Show current asset name (debug only)"))
	bool bShowCurrentAssetName = false;

	UPROPERTY(Config, EditAnywhere, Category = "Behaviour", meta = (DisplayName = "Minimum display time (seconds)"))
	float MinimumDisplayTime = 2.f;

	UPROPERTY(Config, EditAnywhere, Category = "Behaviour", meta = (DisplayName = "Auto-dismiss when load completes"))
	bool bAutoCompleteWhenLoaded = true;

	UPROPERTY(Config, EditAnywhere, Category = "Behaviour", meta = (DisplayName = "Require press to continue after load completes"))
	bool bRequirePressToContinue = false;

	UPROPERTY(Config, EditAnywhere, Category = "Behaviour", meta = (DisplayName = "Continue key (keyboard)", EditCondition = "bRequirePressToContinue"))
	FKey ContinueKeyKeyboard = EKeys::SpaceBar;

	UPROPERTY(Config, EditAnywhere, Category = "Behaviour", meta = (DisplayName = "Continue key (gamepad)", EditCondition = "bRequirePressToContinue"))
	FKey ContinueKeyGamepad = EKeys::Gamepad_FaceButton_Bottom;

	UPROPERTY(Config, EditAnywhere, Category = "Debug", meta = (DisplayName = "Simulate load duration (seconds, 0 = disabled)", ClampMin = "0.0", ClampMax = "30.0"))
	float SimulatedLoadDurationSeconds = 0.f;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& e) override;
#endif
};
