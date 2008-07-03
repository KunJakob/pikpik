/**
* @file Match.cpp
* @author Nat Ryall
* @date 27/05/2008
* @brief Network matchmaking routines.
*
* Copyright © SAPIAN
*/

//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Global.
#include <Global.h>

// Local.
#include <Match.h>

//##############################################################################

//##############################################################################
//
//                                 MATCH QUERY
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         02-Jul-2008
// =============================================================================
CMatchQuery::CMatchQuery()
{
	m_pHeader = "match://";
	Reset();
}

// =============================================================================
// Nat Ryall                                                         02-Jul-2008
// =============================================================================
void CMatchQuery::Reset()
{
	m_sQuery = m_pHeader;
}

// =============================================================================
// Nat Ryall                                                         02-Jul-2008
// =============================================================================
void CMatchQuery::AddValue(const xchar* pKey, const xchar* pValue)
{
	xstring sKey = FormatString(pKey);
	xstring sValue = FormatString(pValue);

	if (sKey.length() && sValue.length())
	{
		if (m_sQuery.length() != strlen(m_pHeader))
			m_sQuery += '&';

		m_sQuery += sKey;
		m_sQuery += '=';
		m_sQuery += sValue;
	}
}

// =============================================================================
// Nat Ryall                                                         02-Jul-2008
// =============================================================================
void CMatchQuery::AddValue(const xchar* pKey, xint iValue)
{
	xchar cBuffer[32];
	_itoa_s(iValue, cBuffer, 10);

	AddValue(pKey, cBuffer);
}

// =============================================================================
// Nat Ryall                                                         02-Jul-2008
// =============================================================================
xstring CMatchQuery::FormatString(xstring sString)
{
	return sString;
}

//##############################################################################

//##############################################################################
//
//                               MATCH MANAGER
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         30-Jun-2008
// =============================================================================
CMatchManager::CMatchManager() :
	m_pTCP(NULL),
	m_pHTTP(NULL)
{
}

// =============================================================================
// Nat Ryall                                                         30-Jun-2008
// =============================================================================
CMatchManager::~CMatchManager()
{
	Deinitialise();
}

// =============================================================================
// Nat Ryall                                                         30-Jun-2008
// =============================================================================
CSession* CMatchManager::TEMP_QueryServer(xint iSlots)
{
	if (!m_pHTTP->IsBusy())
		m_pHTTP->Post("/match.php?create", "sessid=11819430-18241074-17187362&pass=H82A9ME1PQ");

	while (true)
	{
		Packet* pPacket = m_pTCP->Receive();

		if (pPacket)
		{
			if (m_pHTTP->ProcessFinalTCPPacket(pPacket))
			{
				int iErrorCode = 0;
				RakNet::RakString xErrorString;

				if (m_pHTTP->HasBadResponse(&iErrorCode, &xErrorString))
					XLOG("Error: %d, '%s'", iErrorCode, xErrorString.C_String());
				else
				{
					RakNet::RakString xString = m_pHTTP->Read();
					XLOG("Response: '%s'", xString.C_String());
				}

				break;
			}

			m_pTCP->DeallocatePacket(pPacket);
		}

		m_pHTTP->Update();
		Sleep(10);
	}

	return NULL;
}

// =============================================================================
// Nat Ryall                                                         30-Jun-2008
// =============================================================================
void CMatchManager::Initialise()
{
	if (!m_pTCP)
	{
		m_pTCP = new TCPInterface();
		m_pTCP->Start(0, 64);

		m_pHTTP = new HTTPConnection(*m_pTCP, "sapian.net");
	}
}

// =============================================================================
// Nat Ryall                                                         30-Jun-2008
// =============================================================================
void CMatchManager::Deinitialise()
{
	if (m_pHTTP)
	{
		delete m_pHTTP;
		m_pHTTP = NULL;
	}

	if (m_pTCP)
	{
		delete m_pTCP;
		m_pTCP = NULL;
	}
}

// =============================================================================
// Nat Ryall                                                         02-Jul-2008
// =============================================================================
void CMatchManager::Update()
{
	XEN_LIST_FOREACH(t_SessionList, ppSession, m_lpSessions)
	{
		//if ((*ppSession)->m_iState != SessionState_Idle)
	}
}

// =============================================================================
// Nat Ryall                                                         02-Jul-2008
// =============================================================================
xbool CMatchManager::ListSessions(t_OnListSessionsCompleted fpCallback)
{
	CMatchQuery xQuery;

	xQuery.AddValue("gid", _MATCHGID);
	xQuery.AddValue("slots", 1);
	xQuery.AddValue("info", "noradar");

	m_pHTTP->Post("/match.php?list", xQuery.GetQuery());

	return false;
}

// =============================================================================
// Nat Ryall                                                         02-Jul-2008
// =============================================================================
CSession* CMatchManager::CreateSession(xint iTotalSlots, t_OnCreateSessionCompleted fpCallback)
{
	CMatchQuery xQuery;

	xQuery.AddValue("gid", _MATCHGID);
	xQuery.AddValue("sid", GenerateSessionID().c_str());
	xQuery.AddValue("title", "_PIKPIK_");
	xQuery.AddValue("slots", 5);
	xQuery.AddValue("info", "noradar:notraps");

	m_pHTTP->Post("/match.php?create", xQuery.GetQuery());
	
	while (true)
	{
		Packet* pPacket = m_pTCP->Receive();

		if (pPacket)
		{
			if (m_pHTTP->ProcessFinalTCPPacket(pPacket))
			{
				int iErrorCode = 0;
				RakNet::RakString xErrorString;

				if (m_pHTTP->HasBadResponse(&iErrorCode, &xErrorString))
					XLOG("Error: %d, '%s'", iErrorCode, xErrorString.C_String());
				else
				{
					RakNet::RakString xString = m_pHTTP->Read();
					XLOG("Response: '%s'", xString.C_String());
				}

				break;
			}

			m_pTCP->DeallocatePacket(pPacket);
		}

		m_pHTTP->Update();
		Sleep(1);
	}

	/*m_fpOnCreateSessionCompleted = fpCallback;

	if (m_fpOnCreateSessionCompleted)
	{
		m_fpOnCreateSessionCompleted(true, NULL);
		m_fpOnCreateSessionCompleted = NULL;
	}*/

	return NULL;
}

// =============================================================================
// Nat Ryall                                                         03-Jul-2008
// =============================================================================
xbool CMatchManager::PingSession(CSession* pSession, t_OnPingSessionCompleted fpCallback)
{
	CMatchQuery xQuery;

	xQuery.AddValue("gid", _MATCHGID);
	xQuery.AddValue("sid", pSession->m_sSessionID.c_str());
	xQuery.AddValue("pass", pSession->m_sPassword.c_str());

	m_pHTTP->Post("/match.php?ping", xQuery.GetQuery());

	return false;
}

// =============================================================================
// Nat Ryall                                                         02-Jul-2008
// =============================================================================
xbool CMatchManager::UpdateSession(CSession* pSession, t_OnUpdateSessionCompleted fpCallback)
{
	CMatchQuery xQuery;

	xQuery.AddValue("gid", _MATCHGID);
	xQuery.AddValue("sid", pSession->m_sSessionID.c_str());
	xQuery.AddValue("pass", pSession->m_sPassword.c_str());
	xQuery.AddValue("state", pSession->m_iState);
	xQuery.AddValue("title", pSession->m_sTitle.c_str());
	xQuery.AddValue("slots", pSession->m_iTotalSlots);
	xQuery.AddValue("players", "Krakken,slygamer123,xxRaDiXxx"); // TODO: Change this to use pSession->m_lpPlayers
	xQuery.AddValue("info", pSession->m_sSessionInfo.c_str());

	m_pHTTP->Post("/match.php?update", xQuery.GetQuery());

	return false;
}

// =============================================================================
// Nat Ryall                                                         02-Jul-2008
// =============================================================================
void CMatchManager::DestroySession(CSession* pSession, t_OnDestroySessionCompleted fpCallback)
{
	CMatchQuery xQuery;

	xQuery.AddValue("gid", _MATCHGID);
	xQuery.AddValue("sid", pSession->m_sSessionID.c_str());
	xQuery.AddValue("pass", pSession->m_sPassword.c_str());

	m_pHTTP->Post("/match.php?destroy", xQuery.GetQuery());
}

// =============================================================================
// Nat Ryall                                                         02-Jul-2008
// =============================================================================
xstring CMatchManager::GenerateSessionID()
{
	static const xchar* s_pChars = "0123456789";
	static const xint s_iNumChars = strlen(s_pChars);

	xstring sSessionID = "";

	for (xint iA = 0; iA < 3; ++iA)
	{
		for (xint iB = 0; iB < 8; ++iB)
			sSessionID += s_pChars[rand() % s_iNumChars];

		if (iA != 2)
			sSessionID += '-';
	}

	return sSessionID;
}

//##############################################################################

//##############################################################################
//
//                                   SESSION
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         02-Jul-2008
// =============================================================================
void CSession::AddPlayer(xstring sName)
{

}

// =============================================================================
// Nat Ryall                                                         02-Jul-2008
// =============================================================================
void CSession::RemovePlayer(xstring sName)
{

}

//##############################################################################


