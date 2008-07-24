#pragma once

#pragma region Include
//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Global.
#include <Global.h>

// Other.
#include <Metadata.h>
#include <Sprite.h>
#include <Renderer.h>
#include <Trap.h>
#include <Power.h>

//##############################################################################
#pragma endregion

#pragma region Types
//##############################################################################
//
//                                   TYPES
//
//##############################################################################

// Predeclare.
class CPlayer;
class CMap;
class CMapBlock;

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
	BlockType_Base,
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
typedef xvlist<CMapBlock*> t_MapBlockList;

//##############################################################################
#pragma endregion

#pragma region Block
//##############################################################################
//
//                                   BLOCK
//
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
	xbool IsBase()
	{
		return m_iBlockType == BlockType_Base;
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
	Tools::CTimer m_xRespawnTimer;

	// The block visibility.
	xfloat m_fVisibility;

	// The trap (if any) that is bound to this block.
	CTrap* m_pTrap;
	
	// The power (if any) that is bound to this block.
	CPower* m_pPower;

	// The block adjacents.
	CMapBlock* m_pAdjacents[AdjacentDirection_Max];

protected:
	/**
	* Get the bit-index for the adjacent.
	*/
	xuint GetBit(t_AdjacentDirection iAdjacentDir)
	{
		return 1 << (xuint)iAdjacentDir;
	}
};

//##############################################################################
#pragma endregion

#pragma region Map
//##############################################################################
//
//                                     MAP
//
//##############################################################################
class CMap : public CRenderable
{
public:
	// Friends.
	friend class CMapBlock;

	/**
	* Create a new map from metadata.
	*/
	CMap(const xchar* pID);

	/**
	* Clean up the map data on destruction.
	*/
	virtual ~CMap();

	/**
	* Update the map.
	*/
	virtual void Update();

	/**
	* Render the map.
	*/
	virtual void Render();

	/**
	* Get the name of the map as specified in the metadata.
	*/
	const xchar* GetName()
	{
		return m_pName;
	}

	/**
	* Get the block width of the map as specified in the metadata.
	*/
	xuint GetWidth()
	{
		return m_iWidth;
	}

	/**
	* Get the block height of the map as specified in the metadata.
	*/
	xuint GetHeight()
	{
		return m_iHeight;
	}

	/**
	* Set the map offset in pixels to begin the map render.
	*/
	void SetOffset(xpoint xOffset)
	{
		m_xOffset = xOffset;
	}

	/**
	* Get the current offset.
	*/
	xpoint GetOffset()
	{
		return m_xOffset;
	}

	/**
	* Get a map block by index.
	*/
	CMapBlock* GetBlock(xuint iBlockIndex)
	{
		XMASSERT(iBlockIndex < (m_iWidth * m_iHeight), "Block index requested is out of bounds.");
		return &m_xBlocks[iBlockIndex];
	}

	/**
	* Get a map block by block position.
	*/
	CMapBlock* GetBlock(xpoint xBlockPos)
	{
		return GetBlock(xBlockPos.iX + (xBlockPos.iY * m_iWidth));
	}

	/**
	* Get a random player spawn block.
	*/
	CMapBlock* GetSpawnBlock(t_PlayerType iPlayerType);

	/**
	* Get a block in the adjacent direction to the specified block. This will wrap around the map if on the edge.
	*/
	CMapBlock* GetAdjacentBlock(t_AdjacentDirection iAdjacentDir, CMapBlock* pBlock);

protected:
	/**
	* Add the specified visibility to all valid paths from the specified block.
	*/
	void AddVisiblePaths(CMapBlock* pStartingBlock, xfloat fVisibility);

	// The map name.
	const xchar* m_pName;

	// The map dimensions in blocks.
	xuint m_iWidth;
	xuint m_iHeight;

	// The total number of blocks in the map.
	xuint m_iBlockCount;

	// The total number of pellets eaten.
	xuint m_iPelletsEaten;

	// The processed map data.
	CMapBlock* m_xBlocks;

	// The list of player spawn positions.
	t_MapBlockList m_lpSpawnPoints[PlayerType_Max];	

	// The current map offset in pixels.
	xpoint m_xOffset;

	// The map colourisation.
	xfloat m_fColours[3];
	xbool m_bUp[3];
};

//##############################################################################
#pragma endregion
