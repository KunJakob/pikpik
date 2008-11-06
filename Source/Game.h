#pragma once

/**
* @file Game.h
* @author Nat Ryall
* @date 11/04/2008
* @brief The actual game screen.
*
* Copyright © SAPIAN
*/

//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Global.
#include <Global.h>

// Other.
#include <Background.h>
#include <Interface.h>

//##############################################################################

//##############################################################################
//
//                                 DECLARATION
//
//##############################################################################
class CGameScreen : public CScreen
{
public:
	// Constructor.
	CGameScreen() : CScreen(ScreenIndex_GameScreen) {}

protected:
	// Called to load the screen resources.
	virtual void OnLoad() {}

	// Called to unload the screen resources.
	virtual void OnUnload() {}

	// Called when the screen is activated.
	virtual void OnActivate() {}

	// Called when the state is deactivated.
	virtual void OnDeactivate() {}

	// Called when the screen is active and is the immediate screen in the stack.
	virtual void OnWake();

	// Called when the screen is no longer the immediate screen in the stack but is still active.
	virtual void OnSleep();

	// Called to update the screen (updates the parent screen by default).
	virtual void OnUpdate();
};

//##############################################################################