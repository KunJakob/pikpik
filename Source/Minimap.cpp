/**
* @file Minimap.cpp
* @author Nat Ryall
* @date 24/07/2008
* @brief Allowing rendering of a map to a texture.
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

// Local.
#include <Minimap.h>

// Other.
#include <Player.h>

//##############################################################################

//##############################################################################
//
//                                   MINIMAP
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         24-Jul-2008
// =============================================================================
CMinimap::CMinimap(CMap* pMap) : CRenderable(RenderableType_Minimap),
	m_pMap(pMap),
	m_hMinimap(NULL),
	m_pSprite(NULL),
	m_iScanlineOffset(0)
{
	m_iWidth = m_pMap->GetWidth();
	m_iHeight = m_pMap->GetHeight();

	m_hMinimap = _HGE->Texture_Create(m_iWidth, m_iHeight);

	m_pSprite = new hgeSprite(m_hMinimap, 0, 0, (xfloat)m_iWidth, (xfloat)m_iHeight);

	m_iTextureWidth = _HGE->Texture_GetWidth(m_hMinimap);
	m_iTextureHeight = _HGE->Texture_GetHeight(m_hMinimap);
}

// =============================================================================
// Nat Ryall                                                         24-Jul-2008
// =============================================================================
CMinimap::~CMinimap()
{
	_HGE->Texture_Free(m_hMinimap);
	delete m_pSprite;
}

// =============================================================================
// Nat Ryall                                                         24-Jul-2008
// =============================================================================
void CMinimap::Render()
{
	_HGE->System_SetState(HGE_TEXTUREFILTER, false);
		
	m_pSprite->RenderEx(10.f, 10.f, 0.f, 4.f, 0.f);
	
	_HGE->System_SetState(HGE_TEXTUREFILTER, true);
}

// =============================================================================
// Nat Ryall                                                         24-Jul-2008
// =============================================================================
HTEXTURE CMinimap::Generate(xuint iElementMask)
{
	DWORD* pMinimap = _HGE->Texture_Lock(m_hMinimap, false);
	{
		for (xint iY = 0; iY < m_iHeight; ++iY)
		{
			for (xint iX = 0; iX < m_iWidth; ++iX)
			{
				CMapBlock* pBlock = m_pMap->GetBlock(xpoint(iX, iY));
				DWORD* pPixel = &pMinimap[iX + (iY * m_iTextureWidth)];

				// Check for any special overrides.
				if (false)
				{

				}
				// Render as a standard block.
				else
				{
					*pPixel = 0xFF000000;

					switch (pBlock->m_iBlockType)
					{
					// Floor.
					case BlockType_Floor:
						{
							if (XFLAGISSET(iElementMask, MinimapElement_Floor))
								*pPixel = 0xFF000000;
						}
						break;

					// Pellet
					case BlockType_Pellet:
						{
							if (XFLAGISSET(iElementMask, MinimapElement_Pellets) && !pBlock->m_bEaten)
								*pPixel = 0xFFFFFFFF;
						}
						break;

					// Wall.
					case BlockType_Wall:
						{
							if (XFLAGISSET(iElementMask, MinimapElement_Walls))
								*pPixel = 0xFF8080FF;
						}
						break;

					// Ghost Wall.
					case BlockType_GhostWall:
						{
							if (XFLAGISSET(iElementMask, MinimapElement_GhostWalls))
								*pPixel = 0xFFC0C0FF;
						}
						break;

					// Ghost Base.
					case BlockType_GhostBase:
						{
							if (XFLAGISSET(iElementMask, MinimapElement_GhostBase))
								*pPixel = 0xFF4040FF;
						}
						break;
					}
				}
			}
		}

		// Players.
		//if (XFLAGISSET(iElementMask, MinimapElement_Pacman) || XFLAGISSET(iElementMask, MinimapElement_Ghost))
		{
			XEN_LIST_FOREACH(t_PlayerList, ppPlayer, _GLOBAL.lpPlayers)
			{
				CPlayer* pPlayer = *ppPlayer;

				if (CMapBlock* pBlock = pPlayer->GetCurrentBlock())
				{
					DWORD* pPixel = &pMinimap[pBlock->m_xPosition.iX + (pBlock->m_xPosition.iY * m_iTextureWidth)];

					switch (pPlayer->GetType())
					{
					// Ghost.
					case PlayerType_Ghost:
						{
							if (XFLAGISSET(iElementMask, MinimapElement_Ghost))
								*pPixel = 0xFFFF8000;
						}
						break;

					// Pacman.
					case PlayerType_Pacman:
						{
							if (XFLAGISSET(iElementMask, MinimapElement_Pacman))
								*pPixel = 0xFFFFFF80;
						}
						break;
					}
				}
			}
		}
	}
	_HGE->Texture_Unlock(m_hMinimap);

	//m_pSprite->SetTexture(m_hMinimap);

	return m_hMinimap;
}

//##############################################################################