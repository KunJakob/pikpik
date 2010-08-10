#pragma once

/**
* @file Brain.h
* @author Nat Ryall
* @date 08/07/2008
* @brief The game's artificial intelligence.
*
* Copyright 2008, SAPIAN
*/

//##############################################################################

// Global.
#include <Global.h>

// Other.
#include <Player.h>

//##############################################################################
class CBrain
{
public:
	// Constructor.
	CBrain(CPlayer* pPlayer);

	// Execute the behavioural logic.
	virtual void Think() = 0;

protected:
	// Execute a basic wander logic.
	void Wander();

	// Approach a specific target

	// Scan a corridor for other players.
	t_PlayerList ScanCorridor(t_PlayerDirection iDirection);

	// The player object associated with this brain.
	CPlayer* m_pPlayer;
};

//##############################################################################
class CPacmanBrain : public CBrain
{
};

//##############################################################################
class CGhostBrain : public CBrain
{
public:
	// Constructor.
	CGhostBrain(CPlayer* pPlayer);

	// Execute the behavioural logic.
	virtual void Think();

	// The last point Pacman was seen.
	CMapBlock* m_pLastSeen;
};

//##############################################################################