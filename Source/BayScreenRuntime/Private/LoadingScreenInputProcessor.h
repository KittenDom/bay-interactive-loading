// Copyright @subajat1 2026.

#pragma once

#include "BayScreenLog.h"
#include "Framework/Application/IInputProcessor.h"

#include "Structs/ThreadSafeSharedState.h"

class FLoadingScreenInputProcessor : public IInputProcessor
{
public:
	FLoadingScreenInputProcessor(TWeakPtr<FThreadSafeSharedState> inSharedState, const ULoadingScreenDeveloperSettings* inSettings) : sharedState(MoveTemp(inSharedState)), setting(inSettings)
	{
		check(setting);
	}

	virtual bool HandleKeyDownEvent(FSlateApplication& slateApp, const FKeyEvent& e) override
	{
		TSharedPtr<FThreadSafeSharedState> state = sharedState.Pin();
		if (!state.IsValid())
		{
			return false;
		}

#if !UE_BUILD_SHIPPING
		UE_LOG(LogBayScreen, Warning, TEXT("[Loading] keyEvent: %s"), *e.GetKey().GetDisplayName().ToString());
#endif

		if (IsNextTipKey(e)) // While waiting to continue, still allow tip cycling
		{
			state->AdvanceTip();
			return true;
		}

		if (state->bWaitingForPressToContinue.Load()) // Press to continue — only active once the load is complete
		{
			if (IsContinueKey(e))
			{
				state->bPressToContinueActivated.Store(true);
				return true;
			}

			return false;
		}

		return true; // Consumed — do not propagate to widgets
	}

	virtual bool HandleAnalogInputEvent(FSlateApplication& slateApp, const FAnalogInputEvent& e) override
	{
		return false; // Not handled intentionally
	}

	virtual bool HandleMouseMoveEvent(FSlateApplication& slateApp, const FPointerEvent& e) override
	{
		return false; // Not handled intentionally
	}

	virtual void Tick(const float deltaTime, FSlateApplication& slateApp, TSharedRef<ICursor> cursor) override
	{
		// Intentionally empty - only react to discrete key events
	}

private:
	bool IsContinueKey(const FKeyEvent& keyEvent) const
	{
		if (!setting)
		{
			return false;
		}

		const FKey& pressedKey = keyEvent.GetKey();
		if (pressedKey == setting->ContinueKeyKeyboard)
		{
			return true;
		}
		if (pressedKey == setting->ContinueKeyGamepad)
		{
			return true;
		}
		return false;
	}

	bool IsNextTipKey(const FKeyEvent& keyEvent) const
	{
		if (!setting)
		{
			return false;
		}

		const FKey& pressedKey = keyEvent.GetKey();

		if (pressedKey == setting->NextTipKeyboard)
		{
			return true;
		}
		if (pressedKey == setting->NextTipGamepad)
		{
			return true;
		}

		return false;
	}

	// caches
private:
	TWeakPtr<FThreadSafeSharedState> sharedState;
	const ULoadingScreenDeveloperSettings* setting;
};
