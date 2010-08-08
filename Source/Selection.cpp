/**
* @file Selection.cpp
* @author Nat Ryall
* @date 18/04/2008
* @brief Selection screen implementation.
*
* Copyright © SAPIAN
*/

//##############################################################################

// Global.
#include <Global.h>

// Local.
#include <Selection.h>

// Other.
#include <Background.h>

//##############################################################################

// =============================================================================
CSelectionScreen::CSelectionScreen() : CScreen(ScreenIndex_Invalid)
{
}

// =============================================================================
CSelectionScreen::~CSelectionScreen()
{
}

// =============================================================================
void CSelectionScreen::OnUpdate()
{
	if (_HGE->Input_KeyDown(HGEK_ESCAPE))
	{
		ScreenManager.Pop();
		return;
	}
}

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