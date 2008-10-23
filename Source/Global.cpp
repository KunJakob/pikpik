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
// Nat Ryall                                                         23-Oct-2008
// =============================================================================
const xchar* CGlobal::TranslateLocale(const xchar* pString)
{
	// Check if we have a locale string: "&NAME". 
	if (pString && String::Length(pString) > 1 && pString[0] == '&')
	{
		// If we see "&&" it means we want to use '&'.
		if (pString[1] == '&')
			return &pString[1];
		
		return _LOCALE(&pString[1]);
	}

	// We should just return the string otherwise.
	return pString;
}

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