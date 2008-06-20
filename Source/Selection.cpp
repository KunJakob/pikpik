/**
* @file Selection.cpp
* @author Nat Ryall
* @date 18/04/2008
* @brief Selection screen implementation.
*
* Copyright © SAPIAN
*/

#pragma region Include
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
#pragma endregion

#pragma region Selection
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
	/*m_pFont[FontType_Main] = ResourceManager::CreateFont("Selection-Main");
	m_pFont[FontType_Name] = ResourceManager::CreateFont("Selection-Player");
	m_pFont[FontType_Info] = ResourceManager::CreateFont("Selection-Info");
	m_pFont[FontType_Char] = ResourceManager::CreateFont("Selection-Character");*/
}

// =============================================================================
// Nat Ryall                                                         18-Apr-2008
// =============================================================================
CSelectionScreen::~CSelectionScreen()
{
	/*for (XUINT iA = 0; iA < FontType_Max; ++iA)
	ResourceManager::FreeResource(m_pFont[iA]);*/
}

// =============================================================================
// Nat Ryall                                                         19-Apr-2008
// =============================================================================
void CSelectionScreen::Update()
{
	if (_HGE->Input_KeyDown(HGEK_ESCAPE))
	{
		ScreenManager::Pop();
		return;
	}
}

// =============================================================================
// Nat Ryall                                                         19-Apr-2008
// =============================================================================
void CSelectionScreen::Render()
{
	//XUINT iGhostCount = m_lpPlayers.size() - 1;
	//XUINT iPacCount = 1;
	//XUINT iLineCount = 8;
	//XUINT iTotalLineCount = iGhostCount + iPacCount + iLineCount;

	//XFLOAT fLineHeight = m_pFont[FontType_Main]->GetHeight() * 0.8f;
	//XFLOAT fSmallLineHeight = fLineHeight * m_pFont[FontType_Info]->GetHeight();

	//XFLOAT fTotalHeight = ((XFLOAT)iTotalLineCount * fLineHeight) + (fSmallLineHeight * 2.f);
	//XFLOAT fHalfTotalHeight = fTotalHeight * .5f;

	//XFLOAT fX[3] = {(XFLOAT)_HSWIDTH - 150.f, (XFLOAT)_HSWIDTH, (XFLOAT)_HSWIDTH + 150.f};
	//XFLOAT fY = (XFLOAT)_HSHEIGHT - fHalfTotalHeight - (fLineHeight * .5f);
	//
	//// Render.
	//_HGE->Gfx_Clear(ARGB(255, 255, 128, 0));

	//XEN_LIST_FOREACH(t_PlayerList, ppPlayer, m_lpPlayers)
	//{
	//	if ((*ppPlayer)->GetType() == PlayerType_PacMan)
	//	{
	//		m_pFont[FontType_Name]->Render(XPOINT((XUINT)fX[0], (XUINT)fY), (*ppPlayer)->GetName());
	//		fY += fLineHeight;
	//	}
	//}

	//m_pPacMan->GetSprite()->SetPosition(XPOINT((XUINT)fX[2], (XUINT)fY), false);
	//m_pPacMan->Render();

	//m_pFont[FontType_Info]->Render(XPOINT((XUINT)fX[0], (XUINT)fY), "AS");
	//fY += fSmallLineHeight;

	//m_pFont[FontType_Char]->Render(XPOINT((XUINT)fX[0], (XUINT)fY), "PacMan");
	//fY += fLineHeight * 2.f;

	//m_pFont[FontType_Main]->Render(XPOINT((XUINT)fX[1], (XUINT)fY), "VS");
	//fY += fLineHeight * 2.f;

	//XEN_LIST_FOREACH(t_PlayerList, ppPlayer, m_lpPlayers)
	//{
	//	if ((*ppPlayer)->GetType() == PlayerType_Ghost)
	//	{
	//		m_pFont[FontType_Name]->Render(XPOINT((XUINT)fX[0], (XUINT)fY), (*ppPlayer)->GetName());
	//		fY += fLineHeight;
	//	}
	//}

	//m_pGhost->GetSprite()->SetPosition(XPOINT((XUINT)fX[2], (XUINT)fY), false);
	//m_pGhost->Render();

	//m_pFont[FontType_Info]->Render(XPOINT((XUINT)fX[0], (XUINT)fY), "AS");
	//fY += fSmallLineHeight;

	//m_pFont[FontType_Char]->Render(XPOINT((XUINT)fX[0], (XUINT)fY), "Ghost");
	//fY += fLineHeight * 3.f;

	//m_pFont[FontType_Main]->Render(XPOINT((XUINT)fX[1], (XUINT)fY), "Starting in 3 seconds");
}

// =============================================================================
// Nat Ryall                                                         19-Apr-2008
// =============================================================================
void CSelectionScreen::PushPlayer(CPlayer* pPlayer)
{
	if (pPlayer->GetType() == PlayerType_PacMan)
		m_pPacMan = (CPacMan*)pPlayer;
	else
		m_pGhost = (CGhost*)pPlayer;

	m_lpPlayers.push_back(pPlayer);
}

//##############################################################################
#pragma endregion