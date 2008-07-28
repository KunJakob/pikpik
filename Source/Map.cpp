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
static const xfloat s_fRotationAngleLookup[] =
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

// The block type lookup table.
static const t_BlockType s_iBlockTypeLookup[TileType_Max] =
{
	BlockType_Floor,
	BlockType_Pellet,
	BlockType_Pellet,
	BlockType_Floor,
	BlockType_Wall,
	BlockType_Wall,
	BlockType_Wall,
	BlockType_Wall,
	BlockType_Wall,
	BlockType_Wall,
	BlockType_GhostWall,
	BlockType_GhostBase,
};

//##############################################################################

//##############################################################################
//
//                                   MODULE
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         28-Jul-2008
// =============================================================================
void CMapModule::Initialise()
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

// =============================================================================
// Nat Ryall                                                         28-Jul-2008
// =============================================================================
void CMapModule::Deinitialise()
{
	delete s_pTiles;
	delete s_pMetadata;
}

//##############################################################################

//##############################################################################
//
//                                   BLOCK
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         23-Jun-2008
// =============================================================================
void CMapBlock::Update()
{
	//if (m_bEaten && xTimer.IsExpired())
	//	m_bEaten = false;
}

// =============================================================================
// Nat Ryall                                                         17-Apr-2008
// =============================================================================
xbool CMapBlock::IsVisible(CPlayer* pPlayer)
{
	return true;
}

// =============================================================================
// Nat Ryall                                                         23-Jun-2008
// =============================================================================
void CMapBlock::Eat()
{
	if (!m_bEaten)
	{
		m_bEaten = true;
		//xTimer.ExpireAfter(60000);

		_GLOBAL.pActiveMap->m_iPelletsEaten++;
	}
}

//##############################################################################

//##############################################################################
//
//                                     MAP
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         10-Apr-2008
// =============================================================================
CMap::CMap(const xchar* pID) : CRenderable(RenderableType_Map)
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

	for (xuint iY = 0; iY < m_iHeight; ++iY)
	{
		for (xuint iX = 0; iX < m_iWidth; ++iX)
		{
			xuint iIndex = iX + (iY * m_iWidth); 
			CMapBlock* pBlock = &m_xBlocks[iIndex];

			pBlock->m_iIndex = iIndex;
			pBlock->m_cChar = pProperty->GetChar(iIndex);
			pBlock->m_iTileType = TileType_Blank;
			pBlock->m_fAngle = 0.f;
			pBlock->m_xPosition = xpoint(iX, iY);
			pBlock->m_fVisibility = 0.f;
			pBlock->m_fPlayerVisibility = 0.f;
			pBlock->m_bEaten = false;

			pBlock->m_pAdjacents[AdjacentDirection_Left]	= (iIndex % m_iWidth > 0) ? &m_xBlocks[iIndex - 1] : NULL;
			pBlock->m_pAdjacents[AdjacentDirection_Up]		= (iIndex >= m_iWidth) ? &m_xBlocks[iIndex - m_iWidth] : NULL;
			pBlock->m_pAdjacents[AdjacentDirection_Right]	= (iIndex % m_iWidth < m_iWidth - 1) ? &m_xBlocks[iIndex + 1] : NULL;
			pBlock->m_pAdjacents[AdjacentDirection_Down]	= (iIndex < m_iBlockCount - m_iWidth) ? &m_xBlocks[iIndex + m_iWidth] : NULL;
		}
	}

	for (xuint iA = 0; iA < m_iBlockCount; ++iA)
	{
		CMapBlock* pBlock = &m_xBlocks[iA];

		switch (pBlock->m_cChar)
		{
		// Special.
		case '*': pBlock->m_iTileType = TileType_Pellet;	break;
		case '@': pBlock->m_iTileType = TileType_Power;		break;
		case '=': pBlock->m_iTileType = TileType_Entrance;	break;

		// Wall.
		case '#':
			{
				xuint iMask = 0;

				for (xuint iB = 0; iB < AdjacentDirection_Max; ++iB)
				{
					if (pBlock->m_pAdjacents[iB] && pBlock->m_pAdjacents[iB]->m_cChar == '#')
						iMask |= pBlock->GetBit((t_AdjacentDirection)iB);
				}

				pBlock->m_iTileType = s_iTileIndexLookup[iMask];
				pBlock->m_fAngle = s_fRotationAngleLookup[iMask];
			}
			break;

		// Spawn.
		case '$': 
			pBlock->m_iTileType = TileType_Blank;
			m_lpSpawnPoints[PlayerType_Pacman].push_back(pBlock); 
			break;

		case '%': 
			pBlock->m_iTileType = TileType_Base;
			m_lpSpawnPoints[PlayerType_Ghost].push_back(pBlock);
			break;
		}

		// Determine the block type.
		pBlock->m_iBlockType = s_iBlockTypeLookup[pBlock->m_iTileType];
	}

	// Initialise the colourisation.
	for (xint iA = 0; iA < 3; ++iA)
	{
		m_fColours[iA] = .5f;
		m_bUp[iA] = (rand() % 2 == 0);
	}

	// Initialise the map properties.
	m_iPelletsEaten = 0;
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
		for (xuint iA = 0; iA < m_iBlockCount; ++iA)
			m_xBlocks[iA].m_fVisibility = 0.f;

		if (_GLOBAL.pActivePlayer->m_iState != PlayerState_Warp)
		{
			AddVisiblePaths(_GLOBAL.pActivePlayer->m_pCurrentBlock, 1.0f - _GLOBAL.pActivePlayer->m_fTransition);
			AddVisiblePaths(_GLOBAL.pActivePlayer->m_pTargetBlock, _GLOBAL.pActivePlayer->m_fTransition);
		}

		for (xuint iA = 0; iA < m_iBlockCount; ++iA)
		{
			m_xBlocks[iA].m_fPlayerVisibility = m_xBlocks[iA].m_fVisibility;

			if (m_xBlocks[iA].IsWall() || m_xBlocks[iA].IsGhostWall())
				m_xBlocks[iA].m_fVisibility = 1.f;
		}
	}
	else
	{
		for (xuint iA = 0; iA < m_iBlockCount; ++iA)
		{
			m_xBlocks[iA].m_fVisibility = 1.f;
			m_xBlocks[iA].m_fPlayerVisibility = 1.f;
		}
	}

	for (xuint iA = 0; iA < m_iBlockCount; ++iA)
		m_xBlocks[iA].Update();
}

// =============================================================================
// Nat Ryall                                                         16-Apr-2008
// =============================================================================
void CMap::AddVisiblePaths(CMapBlock* pBase, xfloat fVisibility)
{
	if (pBase)
	{
		pBase->m_fVisibility += fVisibility;

		for (xuint iA = 0; iA < AdjacentDirection_Max; ++iA)
		{
			CMapBlock* pBlock = pBase;

			while (pBlock->m_pAdjacents[iA] && !pBlock->m_pAdjacents[iA]->IsWall() && !pBlock->m_pAdjacents[iA]->IsGhostWall())
			{
				pBlock = pBlock->m_pAdjacents[iA];
				pBlock->m_fVisibility += fVisibility;

				//if (pBlock->IsWall() || pBlock->IsGhostWall())
				//	break;
			}
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

	for (xint iA = 0; iA < 3; ++iA)
	{
		xfloat fChannelEnergy = _GLOBAL.fMusicEnergy * (iA + 1);

		if (m_bUp[iA])
		{
			m_fColours[iA] += fChannelEnergy;
			m_bUp[iA] = !(m_fColours[iA] > s_fMaxColour);
		}
		else
		{
			m_fColours[iA] -= fChannelEnergy;
			m_bUp[iA] = (m_fColours[iA] < s_fMinColour);
		}

		m_fColours[iA] = Math::Clamp(m_fColours[iA], s_fMinColour, s_fMaxColour);
	}

	// Draw the map.
	for (xuint iA = 0; iA < m_iBlockCount; ++iA)
	{
		static xpoint s_xCentrePoint = xpoint(24, 24);

		if (m_xBlocks[iA].IsWall() || m_xBlocks[iA].IsGhostWall())
			s_pTiles->GetMetadata()->GetSprite()->SetColor(ARGBF(1.f, m_fColours[0], m_fColours[1], m_fColours[2]));
		else
			s_pTiles->GetMetadata()->GetSprite()->SetColor(0xFFFFFFFF);

		t_TileType iTileType = m_xBlocks[iA].m_bEaten ? TileType_Eaten : m_xBlocks[iA].m_iTileType;

		s_pTiles->Render
		(
			m_xBlocks[iA].GetScreenPosition() - m_xOffset, 
			s_xCentrePoint, 
			s_pTileAreas[iTileType]->xRect,
			m_xBlocks[iA].m_fVisibility * _GLOBAL.fWorldAlpha, 
			(m_xBlocks[iA].m_fAngle / 180.0f) * M_PI
		);
	}
}

// =============================================================================
// Nat Ryall                                                          3-Jun-2008
// =============================================================================
CMapBlock* CMap::GetAdjacentBlock(t_AdjacentDirection iAdjacentDir, CMapBlock* pBlock)
{
	if (pBlock->m_pAdjacents[iAdjacentDir])
		return pBlock->m_pAdjacents[iAdjacentDir];
	else
	{
		xuint iIndex = pBlock->m_iIndex;

		switch (iAdjacentDir)
		{
		case AdjacentDirection_Left:	return &m_xBlocks[iIndex + m_iWidth - 1];
		case AdjacentDirection_Up:		return &m_xBlocks[m_iBlockCount - m_iWidth + iIndex];
		case AdjacentDirection_Right:	return &m_xBlocks[iIndex - m_iWidth + 1];
		case AdjacentDirection_Down:	return &m_xBlocks[iIndex % m_iWidth];

		default:						return NULL;
		}
	}
}

// =============================================================================
// Nat Ryall                                                          4-Jun-2008
// =============================================================================
CMapBlock* CMap::GetSpawnBlock(t_PlayerType iPlayerType)
{
	CMapBlock* pBlock = NULL;

	do
	{
		pBlock = m_lpSpawnPoints[iPlayerType][rand() % m_lpSpawnPoints[iPlayerType].size()];

		XEN_LIST_FOREACH(t_PlayerList, ppPlayer, _GLOBAL.lpPlayers)
		{
			if ((*ppPlayer)->GetCurrentBlock() == pBlock)
				pBlock = NULL;
		}
	}
	while (!pBlock);

	return pBlock;
}

//##############################################################################
