#pragma once

//##############################################################################

// Global.
#include <Global.h>

// Other.
#include <Sprite.h>
#include <Renderer.h>
#include <Trap.h>
#include <Power.h>
#include <Navigation.h>

//##############################################################################

// Shortcuts.
#define MapManager CMapManager::Get()

//##############################################################################

// Predeclare.
class CPlayer;
class CMap;
class CMapBlock;
class CMapManager;

// Tile type.
enum t_TileType
{
	TileType_Blank,
	TileType_Pellet,
	TileType_Power,
	TileType_Eaten,
	TileType_Solo,
	TileType_Tunnel,
	TileType_Cap,
	TileType_Corner,
	TileType_Junction,
	TileType_Intersection,
	TileType_Entrance,
	TileType_Base,
	/*MAX*/TileType_Max,
};

// A generic block type.
enum t_BlockType
{
	BlockType_Floor,
	BlockType_Pellet,
	BlockType_Wall,
	BlockType_GhostWall,
	BlockType_GhostBase,
};

// Adjacent direction.
enum t_AdjacentDirection
{
	AdjacentDirection_None = -1,
	AdjacentDirection_Left,
	AdjacentDirection_Up,
	AdjacentDirection_Right,
	AdjacentDirection_Down,
	/*MAX*/AdjacentDirection_Max,
};

// Lists.
typedef xarray<CMapBlock*> t_MapBlockList;
typedef xarray<CMap*> t_MapList;

//##############################################################################
class CMapBlock
{
public:
	// Friends.
	friend CMap;

	// Update this block each frame.
	void Update();

	// Check if the block is a wall tile.
	xbool IsWall()
	{
		return m_iBlockType == BlockType_Wall;
	}

	// Check if the block is part of a ghost base.
	xbool IsGhostBase()
	{
		return m_iBlockType == BlockType_GhostBase;
	}

	// Check if the block is a ghost wall.
	xbool IsGhostWall()
	{
		return m_iBlockType == BlockType_GhostWall;
	}

	// Check if the block is edible.
	xbool IsEdible()
	{
		return !m_bEaten && m_iBlockType == BlockType_Pellet;
	}

	// Check if the specified player can see this block.
	xbool IsVisible(CPlayer* pPlayer);

	// Get the screen position of a block.
	xpoint GetScreenPosition()
	{
		return m_xPosition * 48;
	}

	// Eat this block.
	void Eat();

	// The block index for the parent map.
	xuint m_iIndex;

	// The original map char.
	xchar m_cChar;

	// The processed tile type.
	t_TileType m_iTileType;

	// The generic block type (group).
	t_BlockType m_iBlockType;

	// The map rotation angle.
	xfloat m_fAngle;

	// The map block position.
	xpoint m_xPosition;

	// The block "eaten" status.
	xbool m_bEaten;

	// The "eaten" and "respawn" timer.
	CTimer m_xRespawnTimer;

	// The block visibility.
	xfloat m_fVisibility;

	// The player visibility.
	xfloat m_fPlayerVisibility;

	// The trap (if any) that is bound to this block.
	CTrap* m_pTrap;
	
	// The power (if any) that is bound to this block.
	CPower* m_pPower;

	// The block adjacents.
	CMapBlock* m_pAdjacents[AdjacentDirection_Max];

	// The navigation node attached to this block.
	CNavigationNode* m_pNavNode;

	// The tile being used to render this block.
	//CAnimatedSprite* m_pTile;

protected:
	// Get the bit-index for the adjacent.
	xuint GetBit(t_AdjacentDirection iAdjacentDir)
	{
		return 1 << (xuint)iAdjacentDir;
	}
};

//##############################################################################
class CMap : public CRenderable
{
public:
	// Friends.
	friend class CMapBlock;
	friend class CMapManager;

	// Create a new map from metadata.
	CMap(CDataset* pDataset);

	// Clean up the map data on destruction.
	virtual ~CMap();

	// Render the map.
	virtual void OnRender();

	// Determine if the map instance is currently loaded into memory.
	inline xbool IsLoaded()
	{
		return m_bLoaded;
	}

	// Get the map ID.
	inline const xchar* GetID()
	{
		return m_pID;
	}

	// Get the name of the map as specified in the metadata.
	inline const xchar* GetName()
	{
		return m_pName;
	}

	// Get the number of pacman players allowed on this map.
	inline xint GetPacmanCount()
	{
		return m_iPacmanCount;
	}

	// Get the number of ghost players allowed on this map.
	inline xint GetGhostCount()
	{
		return m_iGhostCount;
	}

	// Get the block width of the map as specified in the metadata.
	inline xint GetWidth()
	{
		return m_iWidth;
	}

	// Get the block height of the map as specified in the metadata.
	inline xint GetHeight()
	{
		return m_iHeight;
	}

	// Set the map offset in pixels to begin the map render.
	inline void SetOffset(xpoint xOffset)
	{
		m_xOffset = xOffset;
	}

	// Get the current offset.
	inline xpoint GetOffset()
	{
		return m_xOffset;
	}

	// Get the total count of blocks in the map.
	inline xint GetBlockCount()
	{
		return m_iBlockCount;
	}

	// Get a map block by index.
	inline CMapBlock* GetBlock(xint iBlockIndex)
	{
		XMASSERT(iBlockIndex < (GetWidth() * GetHeight()), "Block index requested is out of bounds.");
		return &m_xBlocks[iBlockIndex];
	}

	// Get a map block by block position.
	inline CMapBlock* GetBlock(xpoint xBlockPos)
	{
		return GetBlock(xBlockPos.m_tX + (xBlockPos.m_tY * GetWidth()));
	}

	// Get the navigation mesh for this map.
	inline CNavigationMesh* GetNavMesh() const
	{ 
		return m_pNavMesh; 
	}

	// Get a random player spawn block.
	CMapBlock* GetSpawnBlock(t_PlayerType iPlayerType);

	// Get a block in the adjacent direction to the specified block. This will wrap around the map if on the edge.
	CMapBlock* GetAdjacentBlock(t_AdjacentDirection iAdjacentDir, CMapBlock* pBlock);

protected:
	// Load the map into memory so that it's playable.
	void Load();

	// Unload the map from memory.
	void Unload();

	// Update the map.
	void Update();

	// Add the specified visibility to all valid paths from the specified block.
	void AddVisiblePaths(CMapBlock* pStartingBlock, xfloat fVisibility);

	// The map dataset.
	CDataset* m_pDataset;

	// Determines if the map is currently loaded into memory.
	xbool m_bLoaded;

	// The map identifier.
	const xchar* m_pID;

	// The title of the map.
	const xchar* m_pName;

	// The number of "pacman" players allowed on the map.
	xint m_iPacmanCount;

	// The number of "ghost" players allowed on the map.
	xint m_iGhostCount;

	// The map width.
	xint m_iWidth;

	// The map height.
	xint m_iHeight;

	// The total number of blocks in the map.
	xint m_iBlockCount;

	// The total number of pellets eaten.
	xint m_iPelletsEaten;

	// The tiles used for rendering the map.
	CAnimatedSprite* m_pTiles[TileType_Max];

	// The areas of each map tile.
	CSpriteMetadata::CArea* m_pTileAreas[TileType_Max];

	// The processed map data.
	CMapBlock* m_xBlocks;

	// The list of player spawn positions.
	t_MapBlockList m_lpSpawnPoints[PlayerType_Max];	

	// The current map offset in pixels.
	xpoint m_xOffset;

	// The map's navigation mesh.
	CNavigationMesh* m_pNavMesh;
};

//##############################################################################
class CMapEvaluator : public CNavigationEvaluator
{
public:
	// Constructor.
	CMapEvaluator(CPlayer* pPlayer)
	{
		m_pPlayer = pPlayer;
	}

protected:
	// Determine if the specified link is valid in this evaluator.
	virtual xbool IsAllowed(CNavigationRequest* pRequest, CNavigationNode* pNode);

	// Get the cost between the parent and current node.
	virtual xfloat GetCost(CNavigationRequest* pRequest, CNavigationNode* pParentNode, CNavigationNode* pCurrentNode);

	// Get the heuristic between the current and goal node.
	virtual xfloat GetHeuristic(CNavigationRequest* pRequest, CNavigationNode* pCurrentNode, CNavigationNode* pGoalNode);

	// The player for which the path is being generated.
	CPlayer* m_pPlayer;
};

//##############################################################################
class CMapManager : public CModule
{
public:
	// Friends.
	friend class CMap;

	// Singleton instance.
	static inline CMapManager& Get() 
	{
		static CMapManager s_Instance;
		return s_Instance;
	}

	// Load all map resources and determine available maps.
	virtual void OnInitialise();

	// Free all map resources and any loaded maps.
	virtual void OnDeinitialise();

	// Update the map manager.
	void Update();

	// Get a specific map by index.
	CMap* GetMap(xint iIndex);

	// Get a specific map by ID.
	CMap* GetMap(const xchar* pID);

	// Load and set the currently active map by index.
	CMap* SetCurrentMap(xint iIndex);

    // Load and set the currently active map by ID.
    CMap* SetCurrentMap(const xchar* pID);

	// Unload and clear the current map.
	void ClearCurrentMap();

    // Get the active map.
    CMap* GetCurrentMap()
	{
		XASSERT(m_pCurrentMap);
		return m_pCurrentMap;
	}

	// Get the number of known maps.
	inline xint GetMapCount()
	{
		return (xint)m_lpMaps.size();
	}

protected:
	// The map metadata.
	CMetadata* m_pMetadata;

	// The list of available maps.
	t_MapList m_lpMaps;

	// The current map being used.
	CMap* m_pCurrentMap;
};

//##############################################################################