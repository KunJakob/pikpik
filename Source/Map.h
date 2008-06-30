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

// Adjacent direction.
enum t_AdjacentDir
{
	AdjacentDir_None = -1,
	AdjacentDir_Left,
	AdjacentDir_Up,
	AdjacentDir_Right,
	AdjacentDir_Down,
	/*MAX*/AdjacentDir_Max,
};

// Lists.
typedef XVLIST<CMapBlock*> t_MapBlockList;

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
	XBOOL IsWall()
	{
		return cChar == '#';
	}

	// Check if the block is part of a ghost base.
	XBOOL IsBase()
	{
		return iType == TileType_Entrance || iType == TileType_Base;
	}

	// Check if the block is edible.
	xbool IsEdible()
	{
		return !bEaten && (iType == TileType_Pellet || iType == TileType_Power);
	}

	// Check if the specified player can see this block.
	XBOOL IsVisible(CPlayer* pPlayer);

	// Get the screen position of a block.
	XPOINT GetScreenPosition()
	{
		return xPosition * 48;
	}

	// Eat this block.
	void Eat();

	// The block index for the parent map.
	XUINT iIndex;

	// The original map char.
	XCHAR cChar;

	// The processed tile type.
	t_TileType iType;

	// The map rotation angle.
	XFLOAT fAngle;

	// The map block position.
	XPOINT xPosition;

	// The block "eaten" status.
	XBOOL bEaten;

	// The "eaten" and "respawn" timer.
	Tools::CTimer xTimer;

	// The block visibility.
	XFLOAT fVisibility;

	// The trap (if any) that is bound to this block.
	CTrap* m_pTrap;
	
	// The power (if any) that is bound to this block.
	CPower* m_pPower;

	// The block adjacents.
	CMapBlock* pAdjacents[AdjacentDir_Max];

protected:
	/**
	* Get the bit-index for the adjacent.
	*/
	XUINT GetBit(t_AdjacentDir iAdjacentDir)
	{
		return 1 << (XUINT)iAdjacentDir;
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
	CMap(const XCHAR* pID);

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
	const XCHAR* GetName()
	{
		return m_pName;
	}

	/**
	* Get the block width of the map as specified in the metadata.
	*/
	XUINT GetWidth()
	{
		return m_iWidth;
	}

	/**
	* Get the block height of the map as specified in the metadata.
	*/
	XUINT GetHeight()
	{
		return m_iHeight;
	}

	/**
	* Set the map offset in pixels to begin the map render.
	*/
	void SetOffset(XPOINT xOffset)
	{
		m_xOffset = xOffset;
	}

	/**
	* Get the current offset.
	*/
	XPOINT GetOffset()
	{
		return m_xOffset;
	}

	/**
	* Get a map block by index.
	*/
	CMapBlock* GetBlock(XUINT iBlockIndex)
	{
		XMASSERT(iBlockIndex < (m_iWidth * m_iHeight), "Block index requested is out of bounds.");
		return &m_xBlocks[iBlockIndex];
	}

	/**
	* Get a map block by block position.
	*/
	CMapBlock* GetBlock(XPOINT xBlockPos)
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
	CMapBlock* GetAdjacentBlock(t_AdjacentDir iAdjacentDir, CMapBlock* pBlock);

protected:
	/**
	* Add the specified visibility to all valid paths from the specified block.
	*/
	void AddVisiblePaths(CMapBlock* pStartingBlock, XFLOAT fVisibility);

	// The map name.
	const XCHAR* m_pName;

	// The map dimensions in blocks.
	XUINT m_iWidth;
	XUINT m_iHeight;

	// The total number of blocks in the map.
	XUINT m_iBlockCount;

	// The total number of pellets eaten.
	xuint m_iPelletsEaten;

	// The processed map data.
	CMapBlock* m_xBlocks;

	// The list of player spawn positions.
	t_MapBlockList m_lpSpawnPoints[PlayerType_Max];	

	// The current map offset in pixels.
	XPOINT m_xOffset;

	// The map colourisation.
	xfloat m_fColours[3];
	xbool m_bUp[3];
};

//##############################################################################
#pragma endregion
