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

// =============================================================================
// Nat Ryall                                                         29-Apr-2008
// =============================================================================
HTEXTURE GenerateFieldMask(XUINT iInnerRadius, XUINT iOuterRadius)
{
  XUINT iRadiusDifference = iOuterRadius - iInnerRadius;
  
  static XUINT s_iWidth = _SWIDTH;
  static XUINT s_iHeight = _SHEIGHT;

  static XPOINT s_xCentre = XPOINT(_HSWIDTH, _HSHEIGHT);

  HTEXTURE hFieldMask = _HGE->Texture_Create(s_iWidth, s_iHeight);

  DWORD* pTexMem = _HGE->Texture_Lock(hFieldMask, false);
  {
    for (XUINT iY = 0; iY < s_iHeight; ++iY)
    {
      for (XUINT iX = 0; iX < s_iWidth; ++iX)
      {
        DWORD* pPixel = &pTexMem[iX + (iY * s_iWidth)];
        XUINT iAlpha = 255;

        XPOINT xDistance = XPOINT(abs((int)iX - (int)s_xCentre.iX), abs((int)iY - (int)s_xCentre.iY));
        xDistance *= xDistance;

        XUINT iDistance = (XUINT)sqrt((XFLOAT)xDistance.iX + (XFLOAT)xDistance.iY);

        if (iDistance < iInnerRadius)
          iAlpha = 0;
        else if (iDistance < iOuterRadius)
          iAlpha = ((iDistance - iInnerRadius) * 255) / iRadiusDifference;

        *pPixel = ARGB(Math::Clamp<XUINT>(iAlpha, 0, 255), 0, 0, 0);
      }
    }
  }
  _HGE->Texture_Unlock(hFieldMask);

  return hFieldMask;
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
	_GLOBAL.lpPlayers.push_back(new CGhost(_GLOBAL.pActiveMap->GetSpawnBlock(PlayerType_Ghost)));
	_GLOBAL.lpPlayers.push_back(new CGhost(_GLOBAL.pActiveMap->GetSpawnBlock(PlayerType_Ghost)));
	_GLOBAL.lpPlayers.push_back(new CGhost(_GLOBAL.pActiveMap->GetSpawnBlock(PlayerType_Ghost)));

	_GLOBAL.pActivePlayer = _GLOBAL.lpPlayers.front();

	RenderManager::Add(LayerIndex_Background, &m_Background);
	RenderManager::Add(LayerIndex_Map, _GLOBAL.pActiveMap);

	XEN_LIST_FOREACH(t_PlayerList, ppPlayer, _GLOBAL.lpPlayers)
		RenderManager::Add(LayerIndex_Player, *ppPlayer);

	RenderManager::SetRenderCallback(LayerIndex_Map, &WorldTransform);
	RenderManager::SetRenderCallback(LayerIndex_Player, &WorldTransform);

  m_pFieldMask = new hgeSprite(GenerateFieldMask(48 * 2, 48 * 5), 0, 0, _SWIDTH, _SHEIGHT);
}

// =============================================================================
// Nat Ryall                                                         11-Apr-2008
// =============================================================================
void CGameScreen::Unload()
{
  _HGE->Texture_Free(m_pFieldMask->GetTexture());
  delete m_pFieldMask;

	RenderManager::Reset();

	XEN_LIST_ERASE_ALL(_GLOBAL.lpPlayers);
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
  if (_GLOBAL.pActivePlayer->GetType() == PlayerType_Ghost)
    m_pFieldMask->Render(0, 0);
}

//##############################################################################
#pragma endregion