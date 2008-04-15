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

	/**
	* Get the screen position of a block.
	*/
	XPOINT GetScreenPosition()
	{
		return xPosition * 48;
	}

	/**
	* Check if the block is passable.
	*/
	XBOOL IsPassable(CPlayer* pPlayer) { return false; }

	// The original map char.
	XCHAR cChar;

	// The processed tile type.
	t_TileType iType;

	// The map rotation angle.
	XFLOAT fRotation;

	// The map block position.
	XPOINT xPosition;

	//CTrap* m_pTrap;
	//CPower* m_pPower;

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
	/**
	* Create a new map from metadata.
	*/
	CMap(const XCHAR* pID);

	/**
	* Clean up the map data on destruction.
	*/
	virtual ~CMap();

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
		XASSERT(iBlockIndex < (m_iWidth * m_iHeight), "Block index requested is out of bounds.");
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
	CMapBlock* GetRandomSpawnBlock()
	{
		return m_lpSpawnBlocks[rand() % m_lpSpawnBlocks.size()];
	}

protected:
	/**
	* Generate an optimised map format from the map metadata.
	*/
	void ProcessMap();

	// The map name.
	const XCHAR* m_pName;

	// The map dimensions in blocks.
	XUINT m_iWidth;
	XUINT m_iHeight;

	// The processed map data.
	CMapBlock* m_xBlocks;

	// The current map offset in pixels.
	XPOINT m_xOffset;

	// The list of player spawn positions.
	t_MapBlockList m_lpSpawnBlocks;
};

//##############################################################################
#pragma endregion

#pragma region Declaration
//##############################################################################
//
//                                 DECLARATION
//
//##############################################################################
namespace MapManager
{
	// Initialise(MAPMETADATA);
	// GetCurrentMap();
	// GetNextMap();
	// GetMapCount();
	// GetCurrentMapIndex();

	void SetCurrentMap(CMap* pMap);
	CMap* GetCurrentMap();
}

//##############################################################################
#pragma endregion