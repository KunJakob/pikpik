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
static CSprite* s_pTiles = NULL;

// The areas of each map tile.
static CSurfaceTemplate::CArea* s_pTileAreas[TileType_Max];

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
		s_pTiles = ResourceManager::CreateSprite("Map-Tiles");

		s_pTileAreas[TileType_Blank]				= s_pTiles->FindArea("Blank");
		s_pTileAreas[TileType_Pellet]				= s_pTiles->FindArea("Pellet");
		s_pTileAreas[TileType_Power]				= s_pTiles->FindArea("Power");
		s_pTileAreas[TileType_Eaten]				= s_pTiles->FindArea("Eaten");
		s_pTileAreas[TileType_Solo]					= s_pTiles->FindArea("Solo");
		s_pTileAreas[TileType_Tunnel]				= s_pTiles->FindArea("Tunnel");
		s_pTileAreas[TileType_Cap]					= s_pTiles->FindArea("Cap");
		s_pTileAreas[TileType_Corner]				= s_pTiles->FindArea("Corner");
		s_pTileAreas[TileType_Junction]			= s_pTiles->FindArea("Junction");
		s_pTileAreas[TileType_Intersection] = s_pTiles->FindArea("Intersection");
		s_pTileAreas[TileType_Entrance]			= s_pTiles->FindArea("Entrance");
		s_pTileAreas[TileType_Base]					= s_pTiles->FindArea("Base");
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
		ResourceManager::FreeResource(s_pTiles);
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
	/*switch (pPlayer->GetType())
	{
	case PlayerType_PacMan:
		{
			return true;
		}
		break;

	case PlayerType_Ghost:
		{
			return true;
		}
		break;
	}*/

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

			pBlock->cChar = pProperty->GetChar(iIndex);
			pBlock->iType = TileType_Blank;
			pBlock->fRotation = 0.f;
			pBlock->xPosition = XPOINT(iX, iY);
			pBlock->bEaten = false;
			pBlock->fVisibility = 0.f;

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
		// Debug.
		case '^': pBlock->iType = TileType_Pellet; pBlock->bEaten = true;		break;	

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
				pBlock->fRotation = s_fRotationAngleLookup[iMask];
			}
			break;

		// Spawn.
		case '$': 
			pBlock->iType = TileType_Pellet;
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
			m_xBlocks[iA].fVisibility = m_xBlocks[iA].IsWall() || m_xBlocks[iA].IsBase();

		AddVisibility(_GLOBAL.pActivePlayer->m_pCurrentBlock, 1.0f - _GLOBAL.pActivePlayer->m_fTransition);
		AddVisibility(_GLOBAL.pActivePlayer->m_pTargetBlock, _GLOBAL.pActivePlayer->m_fTransition);
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
void CMap::AddVisibility(CMapBlock* pBase, XFLOAT fVisibility)
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
	for (XUINT iA = 0; iA < m_iBlockCount; ++iA)
	{
		s_pTiles->SetAlpha(m_xBlocks[iA].fVisibility);
		s_pTiles->SetRotation(m_xBlocks[iA].fRotation, true);
		s_pTiles->SetArea(s_pTileAreas[m_xBlocks[iA].iType]);
		s_pTiles->SetPosition(m_xBlocks[iA].GetScreenPosition() - m_xOffset);
		s_pTiles->Render();
	}
}

//##############################################################################
#pragma endregion