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
#include <Crypt.h>

//##############################################################################

//##############################################################################
//
//                                   STATIC
//
//##############################################################################

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
//                                   BLOCK
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         23-Jun-2008
// =============================================================================
void CMapBlock::Update()
{
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

		Global.m_pActiveMap->m_iPelletsEaten++;
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
CMap::CMap(CDataset* pDataset) : CRenderable(RenderableType_Map),
	m_pDataset(pDataset),
	m_bLoaded(false),
	m_iPelletsEaten(0),
	m_pTiles(NULL),
	m_xBlocks(NULL)
{
	// Load the rest of the map properties.
	m_pID = pDataset->GetName();
	m_pName = pDataset->GetProperty("Name")->GetString();

	m_iPacmanCount = pDataset->GetProperty("Gobblers")->GetInt();
	m_iGhostCount = pDataset->GetProperty("Chasers")->GetInt();

	m_iWidth = pDataset->GetProperty("Size")->GetInt(0);
	m_iHeight = pDataset->GetProperty("Size")->GetInt(1);

	m_iBlockCount = m_iWidth * m_iHeight;
}

// =============================================================================
// Nat Ryall                                                         11-Apr-2008
// =============================================================================
CMap::~CMap()
{
	if (m_bLoaded)
		Unload();
}

// =============================================================================
// Nat Ryall                                                         29-Jul-2008
// =============================================================================
void CMap::Load()
{
	if (!m_bLoaded)
	{
		// Load and initialise all resources.
		m_pTiles = new CBasicSprite(_SPRITE(m_pDataset->GetProperty("Tiles")->GetString()));

		m_pTileAreas[TileType_Blank]			= m_pTiles->GetMetadata()->FindArea("Blank");
		m_pTileAreas[TileType_Pellet]			= m_pTiles->GetMetadata()->FindArea("Pellet");
		m_pTileAreas[TileType_Power]			= m_pTiles->GetMetadata()->FindArea("Power");
		m_pTileAreas[TileType_Eaten]			= m_pTiles->GetMetadata()->FindArea("Eaten");
		m_pTileAreas[TileType_Solo]				= m_pTiles->GetMetadata()->FindArea("Solo");
		m_pTileAreas[TileType_Tunnel]			= m_pTiles->GetMetadata()->FindArea("Tunnel");
		m_pTileAreas[TileType_Cap]				= m_pTiles->GetMetadata()->FindArea("Cap");
		m_pTileAreas[TileType_Corner]			= m_pTiles->GetMetadata()->FindArea("Corner");
		m_pTileAreas[TileType_Junction]			= m_pTiles->GetMetadata()->FindArea("Junction");
		m_pTileAreas[TileType_Intersection]		= m_pTiles->GetMetadata()->FindArea("Intersection");
		m_pTileAreas[TileType_Entrance]			= m_pTiles->GetMetadata()->FindArea("Entrance");
		m_pTileAreas[TileType_Base]				= m_pTiles->GetMetadata()->FindArea("Base");

		m_pTiles->GetMetadata()->GetSprite()->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHABLEND);

		// Allocate the map block memory.
		m_xBlocks = new CMapBlock[m_iBlockCount];

		// Process the map blocks.
		CProperty* pProperty = m_pDataset->GetProperty("Data");

		for (xint iY = 0; iY < m_iHeight; ++iY)
		{
			for (xint iX = 0; iX < m_iWidth; ++iX)
			{
				xint iIndex = iX + (iY * m_iWidth); 
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

		for (xint iA = 0; iA < m_iBlockCount; ++iA)
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
			m_fChannels[iA] = .5f;
			m_bColouriseDir[iA] = (rand() % 2 == 0);
		}

		// Initialise the map properties.
		m_iPelletsEaten = 0;
	}

	m_bLoaded = true;
}

// =============================================================================
// Nat Ryall                                                         29-Jul-2008
// =============================================================================
void CMap::Unload()
{
	if (m_bLoaded)
	{
		delete m_pTiles;

		delete[] m_xBlocks;
		m_xBlocks = NULL;

		for (xint iA = 0; iA < PlayerType_Max; ++iA)
			m_lpSpawnPoints[iA].clear();
	}

	m_bLoaded = false;
}

// =============================================================================
// Nat Ryall                                                         16-Apr-2008
// =============================================================================
void CMap::Update()
{
	if (Global.m_pLocalPlayer->m_iType == PlayerType_Ghost)
	{
		for (xint iA = 0; iA < m_iBlockCount; ++iA)
			m_xBlocks[iA].m_fVisibility = 0.f;

		AddVisiblePaths(Global.m_pLocalPlayer->m_pCurrentBlock, 1.0f - Global.m_pLocalPlayer->m_fTransition);
		AddVisiblePaths(Global.m_pLocalPlayer->m_pTargetBlock, Global.m_pLocalPlayer->m_fTransition);

		for (xint iA = 0; iA < m_iBlockCount; ++iA)
		{
			m_xBlocks[iA].m_fPlayerVisibility = m_xBlocks[iA].m_fVisibility;

			if (m_xBlocks[iA].IsWall() || m_xBlocks[iA].IsGhostWall())
				m_xBlocks[iA].m_fVisibility = 1.f;
		}
	}
	else
	{
		for (xint iA = 0; iA < m_iBlockCount; ++iA)
		{
			m_xBlocks[iA].m_fVisibility = 1.f;
			m_xBlocks[iA].m_fPlayerVisibility = 1.f;
		}
	}

	for (xint iA = 0; iA < m_iBlockCount; ++iA)
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
			}
		}
	}
}

// =============================================================================
// Nat Ryall                                                         10-Apr-2008
// =============================================================================
void CMap::OnRender()
{
	// Blend the colours based on the music energy.
	/*const static xfloat s_fMinColour = 0.2f;
	const static xfloat s_fMaxColour = 1.0f;

	for (xint iA = 0; iA < 3; ++iA)
	{
		xfloat fChannelEnergy = Global.m_fMusicEnergy * (iA + 1);

		if (m_bColouriseDir[iA])
		{
			m_fChannels[iA] += fChannelEnergy;
			m_bColouriseDir[iA] = !(m_fChannels[iA] > s_fMaxColour);
		}
		else
		{
			m_fChannels[iA] -= fChannelEnergy;
			m_bColouriseDir[iA] = (m_fChannels[iA] < s_fMinColour);
		}

		m_fChannels[iA] = Math::Clamp(m_fChannels[iA], s_fMinColour, s_fMaxColour);
	}*/

	// Draw the map.
	for (xint iA = 0; iA < m_iBlockCount; ++iA)
	{
		static xpoint s_xCentrePoint = xpoint(24, 24);

		if (m_xBlocks[iA].IsWall() || m_xBlocks[iA].IsGhostWall())
			m_pTiles->GetMetadata()->GetSprite()->SetColor(_ARGBF(1.f, m_fChannels[0], m_fChannels[1], m_fChannels[2]));
		else
			m_pTiles->GetMetadata()->GetSprite()->SetColor(0xFFFFFFFF);

		t_TileType iTileType = m_xBlocks[iA].m_bEaten ? TileType_Eaten : m_xBlocks[iA].m_iTileType;

		m_pTiles->Render
		(
			m_xBlocks[iA].GetScreenPosition(), 
			s_xCentrePoint, 
			m_pTileAreas[iTileType]->m_xRect,
			m_xBlocks[iA].m_fVisibility, 
			Math::Radians(m_xBlocks[iA].m_fAngle)
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
		case AdjacentDirection_Up:		return &m_xBlocks[iIndex + m_iBlockCount - m_iWidth];
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

		XEN_LIST_FOREACH(t_PlayerList, ppPlayer, Global.m_lpActivePlayers)
		{
			if ((*ppPlayer)->GetCurrentBlock() == pBlock)
				pBlock = NULL;
		}
	}
	while (!pBlock);

	return pBlock;
}

//##############################################################################

//##############################################################################
//
//                                 MAP MANAGER
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         28-Jul-2008
// =============================================================================
void CMapManager::OnInitialise()
{
	m_pMetadata = _METADATA("Maps");

	// Create a map instance for each map in metadata.
	_DATASET_FOREACH(pDataset, m_pMetadata, "Map", NULL)
		m_lpMaps.push_back(new CMap(pDataset));
}

// =============================================================================
// Nat Ryall                                                         28-Jul-2008
// =============================================================================
void CMapManager::OnDeinitialise()
{
	delete m_pMetadata;

	XEN_LIST_ERASE_ALL(m_lpMaps);
}

// =============================================================================
// Nat Ryall                                                         29-Jul-2008
// =============================================================================
CMap* CMapManager::GetMap(xint iIndex)
{
	XASSERT(iIndex < GetMapCount());

	if (iIndex < GetMapCount())
		return m_lpMaps[iIndex];

	return NULL;
}

// =============================================================================
// Nat Ryall                                                         29-Jul-2008
// =============================================================================
CMap* CMapManager::GetMap(const xchar* pID)
{
	XASSERT(pID);

	XEN_LIST_FOREACH(t_MapList, ppMap, m_lpMaps)
	{
		if (String::IsMatch(pID, (*ppMap)->GetID()))
			return *ppMap;
	}

	return NULL;
}

//##############################################################################