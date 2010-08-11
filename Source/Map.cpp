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

// The animation/area name lookup table.
static const xchar* s_pTileNameLookup[TileType_Max] =
{
	"Blank",
	"Pellet",
	"Power",
	"Eaten",
	"Solo",
	"Tunnel",
	"Cap",
	"Corner",
	"Junction",
	"Intersection",
	"Entrance",
	"Base",
};

//##############################################################################

// =============================================================================
void CMapBlock::Update()
{
}

// =============================================================================
xbool CMapBlock::IsVisible(CPlayer* pPlayer)
{
	return true;
}

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

// =============================================================================
CMap::CMap(CDataset* pDataset) : CRenderable(RenderableType_Map),
	m_pDataset(pDataset),
	m_bLoaded(false),
	m_iPelletsEaten(0),
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
CMap::~CMap()
{
	if (m_bLoaded)
		Unload();
}

// =============================================================================
void CMap::Load()
{
	if (!m_bLoaded)
	{
		// Load and initialise all resources.
		const xchar* pTilesMetadata = m_pDataset->GetProperty("Tiles")->GetString();

		// Load the tiles and set a default animation or area for each one.
		for (xint iA = 0; iA < TileType_Max; ++iA)
		{
			m_pTiles[iA] = new CAnimatedSprite(_SPRITE(pTilesMetadata));
			m_pTiles[iA]->GetMetadata()->GetSprite()->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHABLEND);
			
			if (m_pTiles[iA]->HasAnimation(s_pTileNameLookup[iA]))
				m_pTiles[iA]->Play(s_pTileNameLookup[iA]);
			else
				m_pTiles[iA]->SetArea(s_pTileNameLookup[iA]);
		}

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
				pBlock->m_pPower = NULL;
				pBlock->m_pTrap = NULL;
				pBlock->m_pNavNode = NULL;
				//pBlock->m_pTile = NULL;

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
					//pBlock->m_pTile = m_pAnimatedTiles[TileType_];
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

		// Initialise the navigation mesh from the map.
		m_pNavMesh = new CNavigationMesh(m_iBlockCount);

		for (xint iA = 0; iA < m_iBlockCount; ++iA)
		{
			CMapBlock* pBlock = &m_xBlocks[iA];
			CNavigationNode* pNode = m_pNavMesh->GetNode(iA);

			pBlock->m_pNavNode = pNode;
			pNode->SetData(pBlock);

			// Connect adjacent pathways.
			if (!pBlock->IsWall())
			{
				for (xuint iB = 0; iB < AdjacentDirection_Max; ++iB)
				{
					CMapBlock* pAdjacentBlock = GetAdjacentBlock((t_AdjacentDirection)iB, pBlock);

					if (pAdjacentBlock && !pAdjacentBlock->IsWall())
						pNode->LinkTo(m_pNavMesh->GetNode(pAdjacentBlock->m_iIndex));
				}
			}
		}

		// Initialise the map properties.
		m_iPelletsEaten = 0;
	}

	m_bLoaded = true;
}

// =============================================================================
void CMap::Unload()
{
	if (m_bLoaded)
	{
		for (xint iA = 0; iA < TileType_Max; ++iA)
			delete m_pTiles[iA];

		delete[] m_xBlocks;
		m_xBlocks = NULL;

		for (xint iA = 0; iA < PlayerType_Max; ++iA)
			m_lpSpawnPoints[iA].clear();

		delete m_pNavMesh;
	}

	m_bLoaded = false;
}

// =============================================================================
void CMap::Update()
{
	// Update each tile so that animations progress.
	for (xint iA = 0; iA < TileType_Max; ++iA)
		m_pTiles[iA]->Update();

	// Calculate the block visibility.
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
void CMap::OnRender()
{
	// Draw the map.
	for (xint iA = 0; iA < m_iBlockCount; ++iA)
	{
		t_TileType iTileType = m_xBlocks[iA].m_bEaten ? TileType_Eaten : m_xBlocks[iA].m_iTileType;
		CAnimatedSprite* pTile = m_pTiles[iTileType];

		if (m_xBlocks[iA].IsWall() || m_xBlocks[iA].IsGhostWall())
			pTile->GetMetadata()->GetSprite()->SetColor(_ARGBF(1.f, Global.m_fColourChannels[0], Global.m_fColourChannels[1], Global.m_fColourChannels[2]));
		else
			pTile->GetMetadata()->GetSprite()->SetColor(0xFFFFFFFF);

		pTile->SetAnchor(pTile->GetAreaCentre());
		pTile->SetPosition(m_xBlocks[iA].GetScreenPosition());
		pTile->SetAlpha(m_xBlocks[iA].m_fVisibility * Global.m_fMapAlpha);
		pTile->SetAngle(Math::Radians(m_xBlocks[iA].m_fAngle));

		pTile->Render();
	}
}

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

// =============================================================================
xbool CMapEvaluator::IsAllowed(CNavigationRequest* pRequest, CNavigationNode* pNode)
{
	CMapBlock* pBlock = pNode->GetDataAs<CMapBlock>();

	if (m_pPlayer->GetType() == PlayerType_Pacman)
		return !pBlock->IsGhostWall();

	return true;
}

// =============================================================================
xfloat CMapEvaluator::GetCost(CNavigationRequest* pRequest, CNavigationNode* pParentNode, CNavigationNode* pCurrentNode)
{
	CMapBlock* pParentBlock = pParentNode->GetDataAs<CMapBlock>();
	CMapBlock* pCurrentBlock = pCurrentNode->GetDataAs<CMapBlock>();

	return (pCurrentBlock->IsGhostWall()) ? 3.0f : 1.0f;
}

// =============================================================================
xfloat CMapEvaluator::GetHeuristic(CNavigationRequest* pRequest, CNavigationNode* pCurrentNode, CNavigationNode* pGoalNode)
{
	CMapBlock* pCurrentBlock = pCurrentNode->GetDataAs<CMapBlock>();
	CMapBlock* pGoalBlock = pGoalNode->GetDataAs<CMapBlock>();

	xpoint xDifference = pGoalBlock->m_xPosition - pCurrentBlock->m_xPosition;

	return Math::SquareRoot((xfloat)Math::SquaredMagnitude(xDifference));
}

//##############################################################################

// =============================================================================
void CMapManager::OnInitialise()
{
	m_pMetadata = _METADATA("Maps");

	// Create a map instance for each map in metadata.
	XEN_METADATA_DATASET_FOREACH(pDataset, m_pMetadata, "Map", NULL)
		m_lpMaps.push_back(new CMap(pDataset));
}

// =============================================================================
void CMapManager::OnDeinitialise()
{
	delete m_pMetadata;

	XEN_LIST_ERASE_ALL(m_lpMaps);
}

// =============================================================================
CMap* CMapManager::GetMap(xint iIndex)
{
	XASSERT(iIndex < GetMapCount());

	if (iIndex < GetMapCount())
		return m_lpMaps[iIndex];

	return NULL;
}

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
