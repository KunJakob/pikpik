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
	Reset();
}

// =============================================================================
// Nat Ryall                                                         02-Jul-2008
// =============================================================================
void CMatchQuery::Reset()
{
	m_sQuery = MATCH_QUERY_HEADER;
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
		if (m_sQuery.length() != strlen(MATCH_QUERY_HEADER))
			m_sQuery += '&';

		m_sQuery += sKey;
		m_sQuery += '=';
		m_sQuery += sValue;
	}
}

// =============================================================================
// Nat Ryall                                                         08-Jul-2008
// =============================================================================
void CMatchQuery::AddValue(const xchar* pKey, xstring& sValue)
{
	AddValue(pKey, sValue.c_str());
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
//                                MATCH RESULT
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         08-Jul-2008
// =============================================================================
t_MatchResultError CMatchResult::ProcessResult(const xchar* pResult)
{
	m_lpPairs.clear();

	// Make sure the header checks out so that we know we have a valid result.
	if (pResult && strncmp(pResult, MATCH_RESULT_HEADER, strlen(MATCH_RESULT_HEADER)) == 0)
	{
		pResult += strlen(MATCH_RESULT_HEADER);

		// Copy the result string into internal memory so that we can manipulate it.
		xchar* pLocalResult = NULL;
		xint iResultLen = strlen(pResult);

		if (iResultLen)
		{
			pLocalResult = new xchar[++iResultLen];
			strcpy_s(pLocalResult, iResultLen, pResult);
		}
		else
			return MatchResultError_InvalidHeader;

		// Tokenise the key-pairs.
		static xchar* s_pSeparators = "&";

		xchar* pContext = NULL;
		xchar* pToken = strtok_s(pLocalResult, s_pSeparators, &pContext);

		while (pToken)
		{
			xchar* pValue = strchr(pToken, '=');
			*pValue = 0;
			++pValue;

			if (strlen(pToken) && strlen(pValue))
				m_lpPairs[pToken] = pValue;

			pToken = strtok_s(NULL, s_pSeparators, &pContext);
		}

		delete [] pLocalResult;
		pLocalResult = NULL;

		if (KeyExists("error"))
			return (t_MatchResultError)GetInt("error");
		else
			return MatchResultError_InvalidHeader;
	}
	else
		return MatchResultError_InvalidHeader;
}

// =============================================================================
// Nat Ryall                                                         08-Jul-2008
// =============================================================================
xbool CMatchResult::KeyExists(const xchar* pKey)
{
	return m_lpPairs.find(pKey) != m_lpPairs.end();
}

// =============================================================================
// Nat Ryall                                                         08-Jul-2008
// =============================================================================
const xchar* CMatchResult::GetString(const xchar* pKey)
{
	return m_lpPairs[pKey].c_str();
}

// =============================================================================
// Nat Ryall                                                         08-Jul-2008
// =============================================================================
xint CMatchResult::GetInt(const xchar* pKey)
{
	return atoi(m_lpPairs[pKey].c_str());
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
	m_pHTTP(NULL),
	m_iOperation(MatchOperation_None),
	m_iTimeout(0)
{
	m_fpOnListSessionsCompleted = NULL;
	m_fpOnCreateSessionCompleted = NULL;
	m_fpOnPingSessionCompleted = NULL;
	m_fpOnUpdateSessionCompleted = NULL;
	m_fpOnDestroySessionCompleted = NULL;
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

	m_iOperation = MatchOperation_None;
	m_iTimeout = 0;
}

// =============================================================================
// Nat Ryall                                                         02-Jul-2008
// =============================================================================
void CMatchManager::Update()
{
	if (m_iOperation != MatchOperation_None)
	{
		if (m_pHTTP->IsBusy())
		{
			if (m_iTimeout == 0)
				m_iTimeout = _TIMEMS + MATCH_OPERATION_TIMEOUT;

			if (_TIMEMS > m_iTimeout)
			{
				// TODO: Restart the TCP server.
				// TODO: Notify of failure.
			}

			Packet* pPacket = m_pTCP->Receive();

			if (pPacket)
			{
				if (m_pHTTP->ProcessFinalTCPPacket(pPacket))
				{
					int iErrorCode = 0;
					RakNet::RakString xErrorString;

					if (m_pHTTP->HasBadResponse(&iErrorCode, &xErrorString))
						XLOG("[MatchManager] Error: %d, '%s'", iErrorCode, xErrorString.C_String());
					else
					{
						RakNet::RakString xString = m_pHTTP->Read();
						XLOG("[MatchManager] Response: '%s'", xString.C_String());

						ProcessResult(&xString);
					}
				}

				m_pTCP->DeallocatePacket(pPacket);
			}

			m_pHTTP->Update();
		}
	}
}

// =============================================================================
// Nat Ryall                                                         02-Jul-2008
// =============================================================================
xbool CMatchManager::ListSessions(t_OnListSessionsCompleted fpCallback)
{
	XMASSERT(false, "Operation is not yet implemented.");

	CMatchQuery xQuery;

	xQuery.AddValue("gid", _MATCHGID);
	xQuery.AddValue("max", 10);
	xQuery.AddValue("slots", 1);

	m_pHTTP->Post("/match.php?list", xQuery.GetQuery());

	return false;
}

// =============================================================================
// Nat Ryall                                                         02-Jul-2008
// =============================================================================
CSession* CMatchManager::CreateSession(xint iTotalSlots, t_OnCreateSessionCompleted fpCallback)
{
	XMASSERT(m_iOperation == MatchOperation_None, "An operation is already processing.");

	if (m_iOperation == MatchOperation_None)
	{
		// Create a new session object.
		CSession* pSession = new CSession();

		pSession->m_iStatus = SessionStatus_Creating;
		pSession->m_bOwned = true;
		pSession->m_sSessionID = GenerateSessionID().c_str();
		pSession->m_iTotalSlots = iTotalSlots;
		pSession->m_sTitle = "PikPik";

		// Generate and send the operation query.
		CMatchQuery xQuery;

		xQuery.AddValue("gid", _MATCHGID);
		xQuery.AddValue("sid", pSession->m_sSessionID);
		xQuery.AddValue("title", pSession->m_sTitle);
		xQuery.AddValue("slots", iTotalSlots);

		m_pHTTP->Post("/match.php?create", xQuery.GetQuery());

		// Start the operation.
		m_iOperation = MatchOperation_CreateSession;
		m_fpOnCreateSessionCompleted = fpCallback;

		return pSession;
	}

	return NULL;
}

// =============================================================================
// Nat Ryall                                                         03-Jul-2008
// =============================================================================
xbool CMatchManager::PingSession(CSession* pSession, t_OnPingSessionCompleted fpCallback)
{
	XMASSERT(false, "Operation is not yet implemented.");

	CMatchQuery xQuery;

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
	XMASSERT(false, "Operation is not yet implemented.");

	CMatchQuery xQuery;

	xQuery.AddValue("sid", pSession->m_sSessionID.c_str());
	xQuery.AddValue("pass", pSession->m_sPassword.c_str());
	xQuery.AddValue("state", pSession->m_iStatus);
	xQuery.AddValue("slots", pSession->m_iTotalSlots);
	xQuery.AddValue("players", "krakken:slygamer123:xxRaDiXxx"); // TODO: Change this to use pSession->m_lpPlayers
	xQuery.AddValue("info", pSession->m_sSessionInfo.c_str());

	m_pHTTP->Post("/match.php?update", xQuery.GetQuery());

	return false;
}

// =============================================================================
// Nat Ryall                                                         02-Jul-2008
// =============================================================================
void CMatchManager::DestroySession(CSession* pSession, t_OnDestroySessionCompleted fpCallback)
{
	XMASSERT(false, "Operation is not yet implemented.");

	CMatchQuery xQuery;

	xQuery.AddValue("sid", pSession->m_sSessionID.c_str());
	xQuery.AddValue("pass", pSession->m_sPassword.c_str());

	m_pHTTP->Post("/match.php?destroy", xQuery.GetQuery());
}

// =============================================================================
// Nat Ryall                                                         08-Jul-2008
// =============================================================================
void CMatchManager::ProcessResult(RakNet::RakString* pResult)
{
	CMatchResult xResult;

	t_MatchResultError iError = xResult.ProcessResult(pResult->C_String());
	xbool bSuccess = (iError == MatchResultError_Success);

	t_MatchOperation iLastOperation = m_iOperation;
	m_iOperation = MatchOperation_None;

	switch (iLastOperation)
	{
	// Create Session.
	case MatchOperation_CreateSession:
		{
			if (m_fpOnCreateSessionCompleted)
			{
				if (bSuccess)
					m_fpOnCreateSessionCompleted(true, m_pSession);
				else
				{
					delete m_pSession;
					m_fpOnCreateSessionCompleted(false, NULL);
				}

				m_fpOnCreateSessionCompleted = NULL;
			}
		}
		break;
	}
}

// =============================================================================
// Nat Ryall                                                         02-Jul-2008
// =============================================================================
xstring CMatchManager::GenerateSessionID()
{
	static const xchar* s_pChars = "0123456789ABCDEF";
	static const xint s_iNumChars = strlen(s_pChars);

	xstring sSessionID = "";

	for (xint iA = 0; iA < 4; ++iA)
	{
		for (xint iB = 0; iB < 8; ++iB)
			sSessionID += s_pChars[rand() % s_iNumChars];

		if (iA != 3)
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