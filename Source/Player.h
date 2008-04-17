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

// The possible player states.
enum t_PlayerState
{
	PlayerState_None,
	PlayerState_Idle,
	PlayerState_Move,
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

	/**
	* Destructor.
	*/
	virtual ~CPlayer();

	/**
	* Update the object ready for rendering.
	*/
	virtual void Update();

	/**
	* Render the object.
	*/
	virtual void Render();

	/**
	* Get the type of the player.
	*/
	t_PlayerType GetType()
	{
		return m_iType;
	}

	/**
	* Get the current state of the player.
	*/
	t_PlayerState GetState()
	{
		return m_iState;
	}

	/**
	* Get the internal sprite.
	*/
	CAnimatedSprite* GetSprite()
	{
		return m_pSprite;
	}

protected:
	/**
	* Constructor.
	*/
	CPlayer(t_PlayerType iType, const XCHAR* pSpriteName);

	/**
	* Called to change the state of the player object.
	*/
	virtual void SetState(t_PlayerState iState);

	/**
	* Check if the specified block is passable.
	*/
	virtual XBOOL IsPassable(CMapBlock* pBlock)
	{
		return !pBlock->IsWall();
	}

	/**
	* Called when an animation event occurs.
	*/
	static void OnAnimationEvent(const XCHAR* pEvent, void* pObject);

	// The type of the derived class.
	t_PlayerType m_iType;

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

	// The transition direction.
	t_AdjacentDir m_iTransitionDir;

	// TODO: TRAP & POWERUP
};

//##############################################################################
#pragma endregion

#pragma region Ghost
//##############################################################################
//
//                                   GHOST
//
//##############################################################################


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
	/**
	* Constructor.
	*/
	CPacMan();

	/**
	* Update the object ready for rendering.
	*/
	virtual void Update();

protected:
	/**
	* Check if the specified block is passable.
	*/
	virtual XBOOL IsPassable(CMapBlock* pBlock)
	{
		return CPlayer::IsPassable(pBlock) && pBlock->cChar != '=';
	}

	/**
	* Called to change the state of the player object.
	*/
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
	/**
	* Constructor.
	*/
	CGhost();

	/**
	* Destructor.
	*/
	~CGhost();

	/**
	* Update the object ready for rendering.
	*/
	virtual void Update();

	/**
	* Render the object.
	*/
	virtual void Render();

	/**
	* Called to change the state of the player object.
	*/
	virtual void SetState(t_PlayerState iState);

protected:
	// The ghost's eyes.
	CSprite* m_pEyes;
};

//##############################################################################
#pragma endregion