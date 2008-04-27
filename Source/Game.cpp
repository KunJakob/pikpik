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
void WorldTransform(CRenderable* pRenderable)
{
	switch (pRenderable->GetRenderableType())
	{
	case RenderableType_Map:
		{
			CMap* pMap = (CMap*)pRenderable;
			pMap->SetOffset(s_xOffset);
			pMap->Render();
		}
		break;

	case RenderableType_Player:
		{
			CPlayer* pPlayer = (CPlayer*)pRenderable;

			XPOINT xPosition = pPlayer->GetSprite()->GetPosition();

			pPlayer->GetSprite()->SetPosition(xPosition - s_xOffset);
			pPlayer->Render();
			pPlayer->GetSprite()->SetPosition(xPosition);
		}
		break;

	default:
		pRenderable->Render();
	}
}

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
	_GLOBAL.pActiveMap = new CMap(XFORMAT("M%03d", 4));

	_GLOBAL.lpPlayers.push_back(new CPacMan(_GLOBAL.pActiveMap->GetSpawnBlock(PlayerType_PacMan)));
	_GLOBAL.lpPlayers.back()->SetName("Krakken");
	_GLOBAL.lpPlayers.push_back(new CGhost(_GLOBAL.pActiveMap->GetSpawnBlock(PlayerType_Ghost)));
	_GLOBAL.lpPlayers.back()->SetName("Yossarian");
	_GLOBAL.lpPlayers.push_back(new CGhost(_GLOBAL.pActiveMap->GetSpawnBlock(PlayerType_Ghost)));
	_GLOBAL.lpPlayers.back()->SetName("Schmee43");
	_GLOBAL.lpPlayers.push_back(new CGhost(_GLOBAL.pActiveMap->GetSpawnBlock(PlayerType_Ghost)));
	_GLOBAL.lpPlayers.back()->SetName("SlyGamer123");

	_GLOBAL.pActivePlayer = _GLOBAL.lpPlayers.front();

	RenderManager::Add(LayerIndex_Background, &m_Background);
	RenderManager::Add(LayerIndex_Map, _GLOBAL.pActiveMap);

	XEN_LIST_FOREACH(t_PlayerList, ppPlayer, _GLOBAL.lpPlayers)
		RenderManager::Add(LayerIndex_Player, *ppPlayer);

	RenderManager::SetRenderCallback(LayerIndex_Map, &WorldTransform);
	RenderManager::SetRenderCallback(LayerIndex_Player, &WorldTransform);
}

// =============================================================================
// Nat Ryall                                                         11-Apr-2008
// =============================================================================
void CGameScreen::Unload()
{
	RenderManager::Reset();

	XEN_LIST_ERASEMEM(_GLOBAL.lpPlayers);
	delete _GLOBAL.pActiveMap;
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

	if (_HGE->Input_KeyDown(HGEK_SPACE))
	{
		XEN_LIST_FOREACH(t_PlayerList, ppPlayer, _GLOBAL.lpPlayers)
		{
			if (_GLOBAL.pActivePlayer == *ppPlayer)
			{
				if (*ppPlayer == _GLOBAL.lpPlayers.back())
					_GLOBAL.pActivePlayer = _GLOBAL.lpPlayers.front();
				else
				{
					ppPlayer++;
					_GLOBAL.pActivePlayer = *ppPlayer;
				}
			}
		}
	}

	s_xOffset = _GLOBAL.pActivePlayer->GetSprite()->GetPosition() - XPOINT(_HSWIDTH, _HSHEIGHT);
}

// =============================================================================
// Nat Ryall                                                         11-Apr-2008
// =============================================================================
void CGameScreen::Render()
{
}

//##############################################################################
#pragma endregion