/**
* @file Selection.cpp
* @author Nat Ryall
* @date 18/04/2008
* @brief Selection screen implementation.
*
* Copyright � SAPIAN
*/

//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Global.
#include <Global.h>

// Local.
#include <Selection.h>

// Other.
#include <Background.h>

//##############################################################################

//##############################################################################
//
//                                 SELECTION
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         18-Apr-2008
// =============================================================================
CSelectionScreen::CSelectionScreen() : CScreen(ScreenIndex_SelectionScreen)
{
}

// =============================================================================
// Nat Ryall                                                         18-Apr-2008
// =============================================================================
CSelectionScreen::~CSelectionScreen()
{
}

// =============================================================================
// Nat Ryall                                                         19-Apr-2008
// =============================================================================
void CSelectionScreen::Update()
{
	if (_HGE->Input_KeyDown(HGEK_ESCAPE))
	{
		ScreenManager.Pop();
		return;
	}
}

// =============================================================================
// Nat Ryall                                                         19-Apr-2008
// =============================================================================
void CSelectionScreen::Render()
{
}

// =============================================================================
// Nat Ryall                                                         19-Apr-2008
// =============================================================================
void CSelectionScreen::PushPlayer(CPlayer* pPlayer)
{
	if (pPlayer->GetType() == PlayerType_Pacman)
		m_pPacMan = (CPacman*)pPlayer;
	else
		m_pGhost = (CGhost*)pPlayer;

	m_lpPlayers.push_back(pPlayer);
}

//##############################################################################