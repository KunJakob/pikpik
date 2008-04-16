/**
* @file Game.cpp
* @author Nat Ryall
* @date 11/04/2008
* @brief The actual game screen.
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
#include <Game.h>

//##############################################################################
#pragma endregion

#pragma region Static
//##############################################################################
//
//                                   STATIC
//
//##############################################################################

// The world transform offset.
static XPOINT s_xOffset;

//##############################################################################
#pragma endregion

#pragma region Local
//##############################################################################
//
//                                   LOCAL
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         14-Apr-2008
// =============================================================================
/*void WorldTransform(CRenderable* pRenderable)
{
	switch (pRenderable->GetRenderableType())
	{
	case RenderableType_Map:
		{
			CMap* pMap = (CMap*)pRenderable;
			pMap->SetOffset(s_xOffset);
		}
		break;
	}

	pRenderable->Render();
}*/

//##############################################################################
#pragma endregion

#pragma region Defenition
//##############################################################################
//
//                                 DEFENITION
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         11-Apr-2008
// =============================================================================
void CGameScreen::Load()
{
	m_pMap = new CMap(XFORMAT("M%03d", 3));
	MapManager::SetMap(m_pMap);

	m_pPacMan = new CPacMan;
	m_pGhost = new CGhost;

	MapManager::SetPlayer(m_pGhost);

	RenderManager::Add(LayerIndex_Background, &m_Background);
	RenderManager::Add(LayerIndex_Map, m_pMap);
	RenderManager::Add(LayerIndex_Player, m_pGhost);
	RenderManager::Add(LayerIndex_Player, m_pPacMan);

	//RenderManager::SetRenderCallback(LayerIndex_Map, &WorldTransform);
	//RenderManager::SetRenderCallback(LayerIndex_Player, &WorldTransform);
}

// =============================================================================
// Nat Ryall                                                         11-Apr-2008
// =============================================================================
void CGameScreen::Unload()
{
	RenderManager::Reset();

	delete m_pGhost;
	delete m_pPacMan;
	delete m_pMap;
}

// =============================================================================
// Nat Ryall                                                         11-Apr-2008
// =============================================================================
void CGameScreen::Update()
{
	if (_HGE->Input_KeyDown(HGEK_ESCAPE))
	{
		ScreenManager::Pop();
		return;
	}

	//s_xOffset = (m_pPacMan->m_pCurrentBlock->xPosition * 48) - XPOINT(_HSWIDTH, _HSHEIGHT);
}

// =============================================================================
// Nat Ryall                                                         11-Apr-2008
// =============================================================================
void CGameScreen::Render()
{
}

//##############################################################################
#pragma endregion