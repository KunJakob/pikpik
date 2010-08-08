/**
* @file Visor.cpp
* @author Nat Ryall
* @date 07/08/2008
* @brief The screen introduction, visor effect.
*
* Copyright © SAPIAN
*/

//##############################################################################

// Global.
#include <Global.h>

// Local.
#include <Visor.h>

//##############################################################################

// =============================================================================
CVisorScreen::CVisorScreen() : CScreen(ScreenIndex_Invalid),
	m_iState(VisorTransitionState_None)
{
}

// =============================================================================
void CVisorScreen::SetText(const xchar* pText, const xchar* pSubText)
{
	m_xText = pText;
	m_xSubText = pSubText;
}

//##############################################################################