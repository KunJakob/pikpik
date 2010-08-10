//##############################################################################

// Global.
#include <Global.h>

// Other.
#include <Player.h>
#include <Map.h>

//##############################################################################

// =============================================================================
const xchar* CGlobal::GetLocale(const xchar* pName)
{
	if (m_pLocale)
	{
		if (XEN_METADATA_PROPERTY_EXISTS(m_pLocale, pName))
			return XEN_METADATA_PROPERTY->GetString();
	}

	return pName;
}

// =============================================================================
const xchar* CGlobal::GetLocaleFromVar(const xchar* pInput)
{
	// Check if we have a locale string: "&NAME". 
	if (pInput && String::Length(pInput) > 1 && pInput[0] == '&')
	{
		// If we see "&&" it means we want to use '&'.
		if (pInput[1] == '&')
			return &pInput[1];
		
		return GetLocale(&pInput[1]);
	}

	// We should just return the string otherwise.
	return pInput;
}

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
			pPlayer->m_pStartingBlock = pPlayer->GetCurrentBlock();
		}
	}
}

//##############################################################################

//##############################################################################