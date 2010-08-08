/**
* @file Game.cpp
* @author Nat Ryall
* @date 11/04/2008
* @brief The actual game screen.
*
* Copyright © SAPIAN
*/

//##############################################################################

// Global.
#include <Global.h>

// Local.
#include <Game.h>

//##############################################################################

// =============================================================================
void CGameScreen::OnWake()
{
	InterfaceManager.SetCursorVisible(false);
}

// =============================================================================
void CGameScreen::OnSleep()
{
	InterfaceManager.SetCursorVisible(true);
}

// =============================================================================
void CGameScreen::OnUpdate()
{
	// Exit the game.
	if (_HGE->Input_KeyUp(HGEK_ESCAPE))
	{
		ScreenManager.Pop();
		return;
	}
}

//##############################################################################