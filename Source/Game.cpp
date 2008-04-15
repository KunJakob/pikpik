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

	case RenderableType_Player:
		{
			CPlayer* pPlayer = (CPlayer*)pRenderable;

			if (pPlayer->GetType() == PlayerType_PacMan)
			{
				pPlayer->m_pSprite->SetPosition(XPOINT(_HSWIDTH, _HSHEIGHT));
			}
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
	m_pMap = new CMap("M003");
	MapManager::SetCurrentMap(m_pMap);

	m_pPacMan = new CPacMan;

	RenderManager::Add(LayerIndex_Map, m_pMap);
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