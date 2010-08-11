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

	static xint s_iMaxBlocks = 10;
	xint iSearchedBlocks = 0;

	while (pCurrentBlock && !pCurrentBlock->IsWall() && !pCurrentBlock->IsGhostWall() && iSearchedBlocks++ < s_iMaxBlocks)
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
	xbool bFoundPacman = false;

	// Search for Pacman and if he's found, navigate to him.
	for (xuint iA = 0; iA < PlayerDirection_Max; ++iA)
	{
		t_PlayerList lxVisiblePlayers = ScanCorridor((t_PlayerDirection)iA);

		XEN_LIST_FOREACH(t_PlayerList, ppPlayer, lxVisiblePlayers)
		{
			if ((*ppPlayer)->GetType() == PlayerType_Pacman)
			{
				bFoundPacman = true;

				CMapBlock* pBlock = (*ppPlayer)->m_pCurrentBlock;

				// 50% of the time, the Ghost will follow accurately round corners.
				if ((*ppPlayer)->m_pTargetBlock)
				{
					if (rand() % 10 > 4)
						pBlock = (*ppPlayer)->m_pTargetBlock;
				}

				m_pLastSeen = pBlock; 

				m_pPlayer->NavigateTo(m_pLastSeen); // AI can be dumbed down by always using current block.
			}
		}
	}

	// If we're not heading anywhere specific, just wander around.
	if (!bFoundPacman && !m_pPlayer->GetNavPath())	
		Wander();
}
