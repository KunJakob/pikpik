#pragma region Include
//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Global.
#include <Global.h>

// Local.
#include <Map.h>

// Other.
#include <Renderer.h>
#include <Resource.h>
#include <Player.h>

//##############################################################################
#pragma endregion

#pragma region Static
//##############################################################################
//
//                                   STATIC
//
//##############################################################################

// The map metadata.
static CMetadata* s_pMetadata = NULL;

// The tiles used for rendering the map.
static CBasicSprite* s_pTiles = NULL;

// The areas of each map tile.
static CSpriteMetadata::CArea* s_pTileAreas[TileType_Max];

// The tile index lookup table.
static const t_TileType s_iTileIndexLookup[] = 
{
	TileType_Solo,
	TileType_Cap,
	TileType_Cap,
	TileType_Corner,
	TileType_Cap,
	TileType_Tunnel,
	TileType_Corner,
	TileType_Junction,
	TileType_Cap,
	TileType_Corner,
	TileType_Tunnel,
	TileType_Junction,
	TileType_Corner,
	TileType_Junction,
	TileType_Junction,
	TileType_Intersection,
};

// The rotation angle lookup table.
static const XFLOAT s_fRotationAngleLookup[] =
{
	0.0f,
	90.0f,
	180.0f,
	180.0f,
	270.0f,
	90.0f,
	270.0f,
	270.0f,
	0.0f,
	90.0f,
	0.0f,
	180.0f,
	0.0f,
	90.0f,
	0.0f,
	0.0f,
};

//##############################################################################
#pragma endregion

#pragma region Module
//##############################################################################
//
//                                   MODULE
//
//##############################################################################
static class CMapModule : public Xen::CModule
{
public:
	// Constructor.
	CMapModule()
	{
		XMODULE(this);
	}

	// Initialise.
	virtual void Initialise()
	{
		s_pMetadata = new CMetadata(".\\Metadata\\Maps.mta");
		s_pTiles = new CBasicSprite(_SPRITE("Map-Tiles"));

		s_pTileAreas[TileType_Blank]			= s_pTiles->GetMetadata()->FindArea("Blank");
		s_pTileAreas[TileType_Pellet]			= s_pTiles->GetMetadata()->FindArea("Pellet");
		s_pTileAreas[TileType_Power]			= s_pTiles->GetMetadata()->FindArea("Power");
		s_pTileAreas[TileType_Eaten]			= s_pTiles->GetMetadata()->FindArea("Eaten");
		s_pTileAreas[TileType_Solo]				= s_pTiles->GetMetadata()->FindArea("Solo");
		s_pTileAreas[TileType_Tunnel]			= s_pTiles->GetMetadata()->FindArea("Tunnel");
		s_pTileAreas[TileType_Cap]				= s_pTiles->GetMetadata()->FindArea("Cap");
		s_pTileAreas[TileType_Corner]			= s_pTiles->GetMetadata()->FindArea("Corner");
		s_pTileAreas[TileType_Junction]			= s_pTiles->GetMetadata()->FindArea("Junction");
		s_pTileAreas[TileType_Intersection]		= s_pTiles->GetMetadata()->FindArea("Intersection");
		s_pTileAreas[TileType_Entrance]			= s_pTiles->GetMetadata()->FindArea("Entrance");
		s_pTileAreas[TileType_Base]				= s_pTiles->GetMetadata()->FindArea("Base");

		s_pTiles->GetMetadata()->GetSprite()->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHABLEND);
	}

	// Update.
	virtual void Update()
	{
	}

	// Render.
	virtual void Render()
	{
	}

	// Deinitialise.
	virtual void Deinitialise()
	{
		delete s_pTiles;
		delete s_pMetadata;
	}
} 
s_Module;

//##############################################################################
#pragma endregion

#pragma region Block
//##############################################################################
//
//                                   BLOCK
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         17-Apr-2008
// =============================================================================
XBOOL CMapBlock::IsVisible(CPlayer* pPlayer)
{
	return true;
}

//##############################################################################
#pragma endregion

#pragma region Map
//##############################################################################
//
//                                     MAP
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         10-Apr-2008
// =============================================================================
CMap::CMap(const XCHAR* pID) : CRenderable(RenderableType_Map)
{
	CDataset* pDataset = s_pMetadata->GetDataset("Map", pID);

	// Get the map properties.
	m_pName = pDataset->GetProperty("Name")->GetString();

	m_iWidth = pDataset->GetProperty("Size")->GetInt(0);
	m_iHeight = pDataset->GetProperty("Size")->GetInt(1);

	// Allocate the map block memory.
	m_iBlockCount = m_iHeight * m_iWidth;
	m_xBlocks = new CMapBlock[m_iBlockCount];

	// Process the map blocks.
	CProperty* pProperty = pDataset->GetProperty("Data");

	for (XUINT iY = 0; iY < m_iHeight; ++iY)
	{
		for (XUINT iX = 0; iX < m_iWidth; ++iX)
		{
			XUINT iIndex = iX + (iY * m_iWidth); 
			CMapBlock* pBlock = &m_xBlocks[iIndex];

			pBlock->iIndex = iIndex;
			pBlock->cChar = pProperty->GetChar(iIndex);
			pBlock->iType = TileType_Blank;
			pBlock->fAngle = 0.f;
			pBlock->xPosition = XPOINT(iX, iY);
			pBlock->fVisibility = 0.f;
			pBlock->bEaten = false;

			pBlock->pAdjacents[AdjacentDir_Left]		= (iIndex % m_iWidth > 0) ? &m_xBlocks[iIndex - 1] : NULL;
			pBlock->pAdjacents[AdjacentDir_Up]			= (iIndex >= m_iWidth) ? &m_xBlocks[iIndex - m_iWidth] : NULL;
			pBlock->pAdjacents[AdjacentDir_Right]		= (iIndex % m_iWidth < m_iWidth - 1) ? &m_xBlocks[iIndex + 1] : NULL;
			pBlock->pAdjacents[AdjacentDir_Down]		= (iIndex < m_iBlockCount - m_iWidth) ? &m_xBlocks[iIndex + m_iWidth] : NULL;
		}
	}

	for (XUINT iA = 0; iA < m_iBlockCount; ++iA)
	{
		CMapBlock* pBlock = &m_xBlocks[iA];

		switch (pBlock->cChar)
		{
			// Special.
		case '*': pBlock->iType = TileType_Pellet;		break;
		case '@': pBlock->iType = TileType_Power;			break;
		case '=': pBlock->iType = TileType_Entrance;	break;

			// Wall.
		case '#':
			{
				XUINT iMask = 0;

				for (XUINT iB = 0; iB < AdjacentDir_Max; ++iB)
				{
					if (pBlock->pAdjacents[iB] && pBlock->pAdjacents[iB]->cChar == '#')
						iMask |= pBlock->GetBit((t_AdjacentDir)iB);
				}

				pBlock->iType = s_iTileIndexLookup[iMask];
				pBlock->fAngle = s_fRotationAngleLookup[iMask];
			}
			break;

			// Spawn.
		case '$': 
			pBlock->iType = TileType_Blank;
			m_lpSpawnPoints[PlayerType_PacMan].push_back(pBlock); 
			break;

		case '%': 
			pBlock->iType = TileType_Base;
			m_lpSpawnPoints[PlayerType_Ghost].push_back(pBlock);
			break;
		}
	}
}

// =============================================================================
// Nat Ryall                                                         11-Apr-2008
// =============================================================================
CMap::~CMap()
{
	delete[] m_xBlocks;
}

// =============================================================================
// Nat Ryall                                                         16-Apr-2008
// =============================================================================
void CMap::Update()
{
	if (_GLOBAL.pActivePlayer->m_iType == PlayerType_Ghost)
	{
		for (XUINT iA = 0; iA < m_iBlockCount; ++iA)
			m_xBlocks[iA].fVisibility = 0.f;

		if (_GLOBAL.pActivePlayer->m_iState != PlayerState_Warp)
		{
			AddVisiblePaths(_GLOBAL.pActivePlayer->m_pCurrentBlock, 1.0f - _GLOBAL.pActivePlayer->m_fTransition);
			AddVisiblePaths(_GLOBAL.pActivePlayer->m_pTargetBlock, _GLOBAL.pActivePlayer->m_fTransition);
		}

		for (XUINT iA = 0; iA < m_iBlockCount; ++iA)
		{
			if (m_xBlocks[iA].IsWall() || m_xBlocks[iA].IsBase())
				m_xBlocks[iA].fVisibility = 1.f;
		}
	}
	else
	{
		for (XUINT iA = 0; iA < m_iBlockCount; ++iA)
			m_xBlocks[iA].fVisibility = 1.f;
	}
}

// =============================================================================
// Nat Ryall                                                         16-Apr-2008
// =============================================================================
void CMap::AddVisiblePaths(CMapBlock* pBase, XFLOAT fVisibility)
{
	pBase->fVisibility += fVisibility;

	for (XUINT iA = 0; iA < AdjacentDir_Max; ++iA)
	{
		CMapBlock* pBlock = pBase;

		while (pBlock->pAdjacents[iA] && !pBlock->pAdjacents[iA]->IsWall())
		{
			pBlock = pBlock->pAdjacents[iA];
			pBlock->fVisibility += fVisibility;
		}
	}
}

// =============================================================================
// Nat Ryall                                                         10-Apr-2008
// =============================================================================
void CMap::Render()
{
	// Blend the colours based on the music energy.
	const static xfloat s_fMinColour = 0.2f;
	const static xfloat s_fMaxColour = 1.0f;

	static xfloat s_fColours[3] = {0.5f, 0.5f, 0.5f};
	static xbool s_bUp[3] = {rand() % 2 == 0, rand() % 2 == 0, rand() % 2 == 0};

	for (xint iA = 0; iA < 3; ++iA)
	{
		xfloat fChannelEnergy = _GLOBAL.fMusicEnergy * (iA + 1);

		if (s_bUp[iA])
		{
			s_fColours[iA] += fChannelEnergy;
			s_bUp[iA] = !(s_fColours[iA] > s_fMaxColour);
		}
		else
		{
			s_fColours[iA] -= fChannelEnergy;
			s_bUp[iA] = (s_fColours[iA] < s_fMinColour);
		}

		s_fColours[iA] = Math::Clamp(s_fColours[iA], s_fMinColour, s_fMaxColour);
	}

	// Draw the map.
	for (XUINT iA = 0; iA < m_iBlockCount; ++iA)
	{
		static XPOINT s_xCentrePoint = XPOINT(24, 24);

		if (m_xBlocks[iA].IsWall() || m_xBlocks[iA].IsBase())
			s_pTiles->GetMetadata()->GetSprite()->SetColor(ARGBF(1.f, s_fColours[0], s_fColours[1], s_fColours[2]));
		else
			//s_pTiles->GetMetadata()->GetSprite()->SetColor(ARGBF(1.f, 1.f - s_fColours[0], 1.f - s_fColours[1], 1.f - s_fColours[2]));
			s_pTiles->GetMetadata()->GetSprite()->SetColor(0xFFFFFFFF);

		s_pTiles->Render
		(
			m_xBlocks[iA].GetScreenPosition() - m_xOffset, 
			s_xCentrePoint, 
			s_pTileAreas[m_xBlocks[iA].iType]->xRect,
			m_xBlocks[iA].fVisibility * _GLOBAL.fWorldAlpha, 
			(m_xBlocks[iA].fAngle / 180.0f) * M_PI
		);
	}
}

// =============================================================================
// Nat Ryall                                                          3-Jun-2008
// =============================================================================
CMapBlock* CMap::GetAdjacentBlock(t_AdjacentDir iAdjacentDir, CMapBlock* pBlock)
{
	if (pBlock->pAdjacents[iAdjacentDir])
		return pBlock->pAdjacents[iAdjacentDir];
	else
	{
		xuint iIndex = pBlock->iIndex;

		switch (iAdjacentDir)
		{
		case AdjacentDir_Left:		return &m_xBlocks[iIndex + m_iWidth - 1];
		case AdjacentDir_Up:		return &m_xBlocks[m_iBlockCount - m_iWidth + iIndex];
		case AdjacentDir_Right:		return &m_xBlocks[iIndex - m_iWidth + 1];
		case AdjacentDir_Down:		return &m_xBlocks[iIndex % m_iWidth];

		default:					return NULL;
		}
	}
}

//##############################################################################
#pragma endregion