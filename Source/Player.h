#pragma once

//##############################################################################

// Global.
#include <Global.h>

// Other.
#include <Renderer.h>
#include <Map.h>
#include <Network.h>
#include <Collision.h>

//##############################################################################

// Shortcuts.
#define PlayerManager CPlayerManager::Get()

//##############################################################################

// Predeclaration.
class CPlayerManager;
class CBrain;
class CGhostBrain;

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
    PlayerDirection_None = -1,
	PlayerDirection_Left = AdjacentDirection_Left,
	PlayerDirection_Up,
	PlayerDirection_Right,
	PlayerDirection_Down,

	PlayerDirection_Max,
};

// The player stream types.
enum t_PlayerStreamType
{
	PlayerStreamType_Move,
};

// Lists.
typedef xarray<CPlayer*> t_PlayerList;
typedef xlist<t_PlayerDirection> t_PlayerDirectionList;

//##############################################################################
class CPlayer : public CRenderable
{
public:
	// Friends.
	friend CPlayerManager;
	friend CMap;
	friend CBrain;
	friend CGhostBrain;

	// Destructor.
	virtual ~CPlayer();

	// Reset the player.
	virtual void Reset();

	// Revive the player from a dead state.
	virtual void Revive();

	// Update the object ready for rendering.
	virtual void Update();

	// Render the object.
	virtual void OnRender();

	// Get the player's list index.
	xint GetIndex()
	{
		return m_iIndex;
	}

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
	void SetLogicType(t_PlayerLogicType _Value);

	// Get the player logic type.
	inline t_PlayerLogicType GetLogicType() 
	{ 
		return m_iLogicType; 
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

	// Navigate the player to a specific block on the map.
	void NavigateTo(CMapBlock* pBlock);

	// Set a navigation path for this player (this will override default behaviours).
	void SetNavPath(CNavigationPath* pPath);

	// Get the current navigation path for this player.
	inline CNavigationPath* GetNavPath()
	{
		return m_pNavPath;
	}

	// Clear any navigation path this player might have.
	void ClearNavPath();

	// Process incoming streams.
	static void OnReceivePlayerUpdate(CNetworkPeer* pFrom, BitStream* pStream);

	// The player's starting block.
	CMapBlock* m_pStartingBlock;

protected:
	// Constuctor.
	CPlayer(t_PlayerType iType, const xchar* pSpriteName);

	// Called to change the state of the player object.
	virtual void SetState(t_PlayerState iState);

	// Execute a move action for the player.
	void Move(t_PlayerDirection iDirection);

	// The player logic update where decisions are made regarding state changes.
	virtual void Logic();

	// The logic for pathfinding.
	virtual void LogicPath();

	// The logic for a local human player.
	virtual void LogicLocal();

	// The logic for computer controlled players (artificial intelligence).
	virtual void LogicAI();

	// The logic for networked human or computer controlled players.
	virtual void LogicRemote();

	// Check if the specified block is passable.
	virtual xbool IsPassable(CMapBlock* pBlock)
	{
		return !pBlock || !pBlock->IsWall();
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
	xint m_iTime;

	// The total time set for the move.
	xint m_iMoveTime;

	// The transition distance between two blocks clamped to the range 0.0 to 1.0.
	xfloat m_fTransition;

	// Determines if the player is leaving or entering the map.
	xbool m_bLeaving;

    // The last requested direction.
	t_PlayerDirection m_iRequestedDir;

    // The last direction we travelled.
	t_PlayerDirection m_iLastDir;

	// The movement direction.
	t_PlayerDirection m_iMoveDir;

	// The transition direction.
	t_PlayerDirection m_iTransitionDir;

	// The queued moves from the network.
	t_PlayerDirectionList m_liQueuedMoves;

	// The current navigation path for this player (overrides other behaviours).
	CNavigationPath* m_pNavPath;

	// The player's brain!
	CBrain* m_pBrain;

private:
	// The player's index.
	xint m_iIndex;
};

//##############################################################################
class CPacman : public CPlayer, public CCollidable
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
		return CPlayer::IsPassable(pBlock) && (!pBlock || pBlock->m_cChar != '=');
	}

	// Called to change the state of the player object.
	virtual void SetState(t_PlayerState iState);

	// Get the collision circle.
	virtual xcircle GetCollisionCircle();

	// Check if the object is actually collidable at the present time. 
	// ~note It is important that both objects must remain valid for the duration of the call.
	virtual xbool IsCollidable(CCollidable* pWith);

	// Callback that is executed when a valid collision occurs.
	virtual void OnCollision(CCollidable* pWith);
};

//##############################################################################
class CGhost : public CPlayer, public CCollidable
{
public:
	// Constructor.
	CGhost(xuint iColour);

	// Destructor.
	~CGhost();

	// Update the object ready for rendering.
	virtual void Update();

	// Render the object.
	virtual void OnRender();

	// Called to change the state of the player object.
	virtual void SetState(t_PlayerState iState);

	// Get the ghost's colour.
	inline xuint GetColour() 
	{ 
		return m_iColour; 
	}

protected:
	// Get the collision circle.
	virtual xcircle GetCollisionCircle();

	// Check if the object is actually collidable at the present time. 
	// ~note It is important that both objects must remain valid for the duration of the call.
	virtual xbool IsCollidable(CCollidable* pWith);

	// Callback that is executed when a valid collision occurs.
	virtual void OnCollision(CCollidable* pWith);

	// The ghost's eyes.
	CSprite* m_pEyes;

	// The ghost's colour.
	xuint m_iColour;
};

//##############################################################################
class CPlayerManager : public CModule
{
public:
    // Singleton instance.
	static inline CPlayerManager& Get() 
	{
		static CPlayerManager s_Instance;
		return s_Instance;
	}

    // Load all player resources.
	void Initialise();

    // Free all player resources.
	virtual void OnDeinitialise();

	// Update the active players.
	void Update();

    // Initialise the players for play.
    void InitialisePlayers(t_PlayerLogicType iLogicType);

	// Enable or disable all players logic and movement.
	void SetPlayersEnabled(xbool bEnabled)
	{
		m_bPlayersEnabled = bEnabled;
	}

    // Get the total number of players (active and inactive) available to the game.
    xint GetPlayerCount()
    {
        return m_lpPlayers.size();
    }

    // Get a player by index.
    CPlayer* GetPlayer(xint iIndex)
    {
        XASSERT(iIndex < GetPlayerCount());
        return m_lpPlayers[iIndex];
    }

	// Get the list of all players (active and inactive) available to the game.
	t_PlayerList& GetPlayers()
	{
		return m_lpPlayers;
	}

    // Get the total number of active players in the game.
    xint GetActivePlayerCount()
    {
        return m_lpActivePlayers.size();
    }

    // Get an active player by index.
    CPlayer* GetActivePlayer(xint iIndex)
    {
        XASSERT(iIndex < GetActivePlayerCount());
        return m_lpActivePlayers[iIndex];
    }

	// Get the list of all active players in the game.
	t_PlayerList& GetActivePlayers()
	{
		return m_lpActivePlayers;
	}

	// Sets the player being controlled locally.
	void SetLocalPlayer(CPlayer* pPlayer)
	{
		SetLocalPlayer(pPlayer->m_iIndex);
	}

	// Sets the player being controlled locally.
	void SetLocalPlayer(xint iIndex);

    // Get the player being controlled locally.
    CPlayer* GetLocalPlayer()
    {
        return m_pLocalPlayer;
    }

protected:
	// Reset all players.
	void ResetPlayers();

    // Determine the list of players for the active map and position them.
	void EstablishActivePlayers();

    // The list of all players available to the game.
	t_PlayerList m_lpPlayers;

	// The list of all active players in the current game.
	t_PlayerList m_lpActivePlayers;

	// The currently active player on the local machine.
	CPlayer* m_pLocalPlayer;

	// Whether or not to update players.
	xbool m_bPlayersEnabled;
};