/**
* @file Brain.cpp
* @author Nat Ryall
* @date 08/07/2008
* @brief The game's artificial intelligence.
*
* Copyright 2008, SAPIAN
*/

//##############################################################################

// Global.
#include <Global.h>

// Local.
#include <Brain.h>

// Other.
#include <Player.h>
#include <Map.h>

//##############################################################################

// =============================================================================
CBrain::CBrain(CPlayer* pPlayer) :
	m_pPlayer(pPlayer)
{
}

// =============================================================================
void CBrain::Wander()
{
	CMapBlock* pMoveDirection[PlayerDirection_Max];
	t_PlayerDirection iRealDirection[PlayerDirection_Max];
	xint iDirectionCount = 0;

	for (xint iA = 0; iA < PlayerDirection_Max; ++iA)
	{
		iRealDirection[iA] = (t_PlayerDirection)((m_pPlayer->m_iMoveDir + iA + 3) % PlayerDirection_Max);
		pMoveDirection[iA] = Global.m_pActiveMap->GetAdjacentBlock((t_AdjacentDirection)iRealDirection[iA], m_pPlayer->m_pCurrentBlock);

		if (m_pPlayer->IsPassable(pMoveDirection[iA]))
			iDirectionCount++;
		else
			pMoveDirection[iA] = NULL;		
	}

	// If we have no choice, 
	if (iDirectionCount == 1)
	{
		for (xint iA = 0; iA < PlayerDirection_Max; ++iA)
		{
			if (pMoveDirection[iA])
			{
				m_pPlayer->Move(iRealDirection[iA]);
				break;
			}
		}
	}
	// Otherwise pick a random path to move down.
	else
	{
		xint iRandomDir = rand() % (iDirectionCount - 1);

		for (xint iA = 0; iA < PlayerDirection_Max; ++iA)
		{
			if (pMoveDirection[iA])
				iRandomDir--;

			if (iRandomDir == -1)
			{
				m_pPlayer->Move(iRealDirection[iA]);
				break;
			}
		}
	}
}

// =============================================================================
t_PlayerList CBrain::ScanCorridor(t_PlayerDirection iDirection)
{
	t_PlayerList xPlayerList;
	CMapBlock* pCurrentBlock = m_pPlayer->m_pCurrentBlock->m_pAdjacents[iDirection];

	while (pCurrentBlock && !pCurrentBlock->IsWall() && !pCurrentBlock->IsGhostWall())
	{
		XEN_LIST_FOREACH(t_PlayerList, ppPlayer, Global.m_lpActivePlayers)
		{
			if ((*ppPlayer)->m_pCurrentBlock == pCurrentBlock)
				xPlayerList.push_back(*ppPlayer);
		}

		pCurrentBlock = pCurrentBlock->m_pAdjacents[iDirection];
	}

	return xPlayerList;
}

//##############################################################################

// =============================================================================
CGhostBrain::CGhostBrain(CPlayer* pPlayer) : CBrain(pPlayer)
{
}

// =============================================================================
void CGhostBrain::Think()
{
	// If the Ghost is in the ghost base, navigate directly to Pacman if he is eating pellets.
	/*if (m_pPlayer->m_pCurrentBlock->IsGhostBase())
	{
		XEN_LIST_FOREACH(t_PlayerList, ppPlayer, Global.m_lpActivePlayers)
		{
			if ((*ppPlayer)->GetType() == PlayerType_Pacman)
			{
				m_pLastSeen = (*ppPlayer)->m_pCurrentBlock;
				m_pPlayer->NavigateTo(m_pLastSeen);

				return;
			}
		}
	}*/

	// Search for Pacman and if he's found, navigate to him.
	for (xuint iA = 0; iA < PlayerDirection_Max; ++iA)
	{
		t_PlayerList lxVisiblePlayers = ScanCorridor((t_PlayerDirection)iA);

		XEN_LIST_FOREACH(t_PlayerList, ppPlayer, lxVisiblePlayers)
		{
			if ((*ppPlayer)->GetType() == PlayerType_Pacman)
			{
				//m_pLastSeen = (*ppPlayer)->m_pCurrentBlock; // Medium (often causes ghosts to carry on when changing directions).
				m_pLastSeen = (*ppPlayer)->m_pTargetBlock ? (*ppPlayer)->m_pTargetBlock : (*ppPlayer)->m_pCurrentBlock; // Hard (often allows ghosts to predict where you are going next)

				//xbool bAlternativePath = false;

				// If there is a Ghost between the Ghost and Pacman, try a different route to cut Pacman off.
				/*XEN_LIST_FOREACH(t_PlayerList, ppGhostPlayer, lxVisiblePlayers)
				{
					if ((*ppGhostPlayer)->GetType() == PlayerType_Ghost)
					{
						Global.m_pActiveMap->m_pExpensiveBlock = (*ppGhostPlayer)->m_pCurrentBlock;
						m_pPlayer->NavigateTo(m_pLastSeen);
						//Global.m_pActiveMap->m_pExpensiveBlock = NULL;

						bAlternativePath = true;
					
						break;
					}
				}*/

				//if (!bAlternativePath)
					m_pPlayer->NavigateTo(m_pLastSeen);
			}
		}
	}

	// If we're not heading anywhere specific, just wander around.
	if (!m_pPlayer->GetNavPath())	
		Wander();
}
