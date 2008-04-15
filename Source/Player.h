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

// The player identifier type.
enum t_PlayerType
{
	PlayerType_Ghost,
	PlayerType_PacMan,
};

// The possible player states.
enum t_PlayerState
{
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

protected:
	/**
	* Constructor.
	*/
	CPlayer(t_PlayerType iType, const XCHAR* pSpriteName);

	// The type of the derived class.
	t_PlayerType m_iType;

	// The state of the player.
	t_PlayerState m_iState;

	// The player sprite.
	CSprite* m_pSprite;

	// The current map block.
	CMapBlock* m_pCurrentBlock;

	// The target map block.
	CMapBlock* m_pTargetBlock;

	// The time elapsed so far in the existing state.
	XUINT m_iTime;

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

#pragma region Pacman
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

	/**
	* Render the object.
	*/
	//virtual void Render() {}
};

//##############################################################################
#pragma endregion