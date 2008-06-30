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
#include <Renderer.h>
#include <Map.h>

//##############################################################################
#pragma endregion

#pragma region Types
//##############################################################################
//
//                                   TYPES
//
//##############################################################################

// The player types.
enum t_PlayerBrainType
{
	PlayerBrainType_Local,
	PlayerBrainType_AI,
	PlayerBrainType_Remote,
};

// The possible player states.
enum t_PlayerState
{
	PlayerState_None,
	PlayerState_Idle,
	PlayerState_Move,
	PlayerState_Warp,
	PlayerState_Die,
};

//##############################################################################
#pragma endregion

#pragma region Base
//##############################################################################
//
//                                    BASE
//
//##############################################################################
class CPlayer : public CRenderable
{
public:
	// Friends.
	friend CMap;

	// Destructor.
	virtual ~CPlayer();

	// Update the object ready for rendering.
	virtual void Update();

	// Render the object.
	virtual void Render();

	// Get the type of the player.
	t_PlayerBrainType GetType()
	{
		return m_iType;
	}

	// Get the current state of the player.
	t_PlayerState GetState()
	{
		return m_iState;
	}

	// Get the internal sprite.
	CAnimatedSprite* GetSprite()
	{
		return m_pSprite;
	}

	// Set the player's name.
	void SetName(const XCHAR* pName)
	{
		strcpy_s(m_cName, _MAXNAMELEN + 1, pName);
	}

	// Get the player's name.
	const XCHAR* GetName()
	{
		return m_cName;
	}

	// Get the current map block this player is on.
	inline CMapBlock* GetCurrentBlock()
	{
		return m_pCurrentBlock;
	}

protected:
	// Constuctor.
	CPlayer(t_PlayerBrainType iType, const XCHAR* pSpriteName);

	// Called to change the state of the player object.
	virtual void SetState(t_PlayerState iState);

	// Check if the specified block is passable.
	virtual XBOOL IsPassable(CMapBlock* pBlock)
	{
		return !pBlock->IsWall();
	}

	// Called when an animation event occurs.
	void OnAnimationEvent(CAnimatedSprite* pSprite, const XCHAR* pEvent);

	// The type of the derived class.
	t_PlayerBrainType m_iType;

	// The state of the player.
	t_PlayerState m_iState;

	// The player sprite.
	CAnimatedSprite* m_pSprite;

	// The current map block.
	CMapBlock* m_pCurrentBlock;

	// The target map block.
	CMapBlock* m_pTargetBlock;

	// The current time set for the operation.
	XUINT m_iTime;

	// The total time set for the move.
	XUINT m_iMoveTime;

	// The transition distance between two blocks clamped to the range 0.0 to 1.0.
	XFLOAT m_fTransition;

	// Determines if the player is leaving or entering the map.
	XBOOL m_bLeaving;

	// The transition direction.
	t_AdjacentDir m_iTransitionDir;

	// The player name.
	XCHAR m_cName[_MAXNAMELEN + 1];
};

//##############################################################################
#pragma endregion

#pragma region PacMan
//##############################################################################
//
//                                   PACMAN
//
//##############################################################################
class CPacMan : public CPlayer
{
public:
	// Costructor.
	CPacMan(CMapBlock* pSpawnBlock);

	// Update the object ready for rendering.
	virtual void Update();

protected:
	// Check if the specified block is passable.
	virtual XBOOL IsPassable(CMapBlock* pBlock)
	{
		return CPlayer::IsPassable(pBlock) && pBlock->cChar != '=';
	}

	// Called to change the state of the player object.
	virtual void SetState(t_PlayerState iState);
};

//##############################################################################
#pragma endregion

#pragma region Ghost
//##############################################################################
//
//                                   GHOST
//
//##############################################################################
class CGhost : public CPlayer
{
public:
	// Constructor.
	CGhost(CMapBlock* pSpawnBlock, xuint iColour);

	// Destructor.
	~CGhost();

	// Update the object ready for rendering.
	virtual void Update();

	// Render the object.
	virtual void Render();

	// Called to change the state of the player object.
	virtual void SetState(t_PlayerState iState);

protected:
	// The ghost's eyes.
	CSprite* m_pEyes;

	// The ghost's colour.
	xuint m_iColour;
};

//##############################################################################
#pragma endregion
