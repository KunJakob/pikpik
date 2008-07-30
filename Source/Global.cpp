//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Global.
#include <Global.h>

// Other.
#include <Player.h>
#include <Map.h>

//##############################################################################

//##############################################################################
//
//                                   GLOBAL
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         30-Jul-2008
// =============================================================================
void CGlobal::ResetActivePlayers()
{
	xint iPacmanCount = m_pActiveMap->GetPacmanCount();
	xint iGhostCount = m_pActiveMap->GetGhostCount();

	m_lpActivePlayers.clear();

	XEN_LIST_FOREACH(t_PlayerList, ppPlayer, Global.m_lpPlayers)
	{
		CPlayer* pPlayer = *ppPlayer;
		xbool bPlaying = false;

		pPlayer->Reset();

		switch (pPlayer->GetType())
		{
		case PlayerType_Ghost:
			bPlaying = (iGhostCount-- > 0);
			break;

		case PlayerType_Pacman:
			bPlaying = (iPacmanCount-- > 0);
			break;
		}

		if (bPlaying)
		{
			m_lpActivePlayers.push_back(pPlayer);
			pPlayer->SetCurrentBlock(m_pActiveMap->GetSpawnBlock(pPlayer->GetType()));
		}
	}
}

//##############################################################################