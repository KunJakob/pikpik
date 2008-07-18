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
enum t_PlayerLogicType
{
	PlayerLogicType_None,
	PlayerLogicType_Local,
	PlayerLogicType_AI,
	PlayerLogicType_Remote,
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

// The direction to move in.
enum t_PlayerDirection
{
	PlayerDirection_Left = AdjacentDirection_Left,
	PlayerDirection_Up,
	PlayerDirection_Right,
	PlayerDirection_Down,
	/*MAX*/PlayerDirection_Max,
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

	// Reset the player.
	virtual void Reset();

	// Update the object ready for rendering.
	virtual void Update();

	// Render the object.
	virtual void Render();

	// Get the type of the player.
	t_PlayerType GetType()
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

	// Set the player logic type.
	inline void SetLogicType(t_PlayerLogicType _Value) 
	{ 
		m_iLogicType = _Value; 
	}

	// Get the player logic type.
	inline t_PlayerLogicType GetLogicType() 
	{ 
		return m_iLogicType; 
	}

	// Set the player's name.
	void SetName(const xchar* pName)
	{
		strcpy_s(m_cName, _MAXNAMELEN + 1, pName);
	}

	// Get the player's name.
	const xchar* GetName()
	{
		return m_cName;
	}

	// Set the player's position using a map block.
	void SetCurrentBlock(CMapBlock* pBlock);

	// Get the current map block this player is on.
	inline CMapBlock* GetCurrentBlock()
	{
		return m_pCurrentBlock;
	}

	// Set the player's position using a point.
	inline void SetPosition(xpoint xPosition)
	{
		m_pSprite->SetPosition(xPosition);
	}

	// Get the player's screen position.
	inline xpoint GetPosition()
	{
		return m_pSprite->GetPosition();
	}

protected:
	// Constuctor.
	CPlayer(t_PlayerType iType, const xchar* pSpriteName);

	// Called to change the state of the player object.
	virtual void SetState(t_PlayerState iState);

	// Execute a move action for the player.
	void Move(t_PlayerDirection iDirection);

	// The player logic update where decisions are made regarding state changes.
	virtual void Logic();

	// The logic for a local human player.
	virtual void LogicLocal();

	// The logic for computer controlled players (artificial intelligence).
	virtual void LogicAI();

	// The logic for networked human or computer controlled players.
	virtual void LogicRemote();

	// A logic behaviour to let the player wander aimlessly.
	void BehaviourWander();

	// Check if the specified block is passable.
	virtual xbool IsPassable(CMapBlock* pBlock)
	{
		return !pBlock->IsWall();
	}

	// Called when an animation event occurs.
	void OnAnimationEvent(CAnimatedSprite* pSprite, const xchar* pEvent);

	// The type of the derived class.
	t_PlayerType m_iType;

	// The player logic type.
	t_PlayerLogicType m_iLogicType;

	// The state of the player.
	t_PlayerState m_iState;

	// The player sprite.
	CAnimatedSprite* m_pSprite;

	// The current map block.
	CMapBlock* m_pCurrentBlock;

	// The target map block.
	CMapBlock* m_pTargetBlock;

	// The current time set for the operation.
	xuint m_iTime;

	// The total time set for the move.
	xuint m_iMoveTime;

	// The transition distance between two blocks clamped to the range 0.0 to 1.0.
	xfloat m_fTransition;

	// Determines if the player is leaving or entering the map.
	xbool m_bLeaving;

	// The movement direction.
	t_PlayerDirection m_iMoveDir;

	// The transition direction.
	t_PlayerDirection m_iTransitionDir;

	// The player name.
	xchar m_cName[_MAXNAMELEN + 1];
};

//##############################################################################
#pragma endregion

#pragma region PacMan
//##############################################################################
//
//                                   PACMAN
//
//##############################################################################
class CPacman : public CPlayer
{
public:
	// Costructor.
	CPacman();

	// Update the object ready for rendering.
	virtual void Update();

protected:
	// Check if the specified block is passable.
	virtual xbool IsPassable(CMapBlock* pBlock)
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
	CGhost(xuint iColour);

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
