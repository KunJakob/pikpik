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
	xstring sKey = EncodeString(pKey);
	xstring sValue = EncodeString(pValue);

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
xstring CMatchQuery::EncodeString(xstring sString)
{
	xstring sResult;

	for (xuint iA = 0; iA < sString.length(); ++iA)
	{
		xchar cChar = sString[iA];

		// Only numbers and letters are valid.
		// 48-57  == '0'-'9'
		// 65-90  == 'A'-'Z'
		// 97-122 == 'a'-'z'
		if ((cChar >= 48 && cChar <= 57) || (cChar >= 65 && cChar <= 90) || (cChar >= 97 && cChar <= 122))
		{
			sResult += cChar;
		}
		// Otherwise encode in a '%FF' format.
		else
		{
			sResult += XFORMAT("%%%02X", cChar);
		}
	}

	return sResult;
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
		xint iResultLen = (xint)strlen(pResult);

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
CMatch::CMatch() :
	m_pTCP(NULL),
	m_pHTTP(NULL),
	m_iOperation(MatchOperation_None),
	m_iTimeout(0)
{
	m_fpOnListSessionsCompleted = NULL;
	m_fpOnCreateSessionCompleted = NULL;
	m_fpOnPingSessionCompleted = NULL;
	m_fpOnUpdateSessionCompleted = NULL;
	m_fpOnCloseSessionCompleted = NULL;
}

// =============================================================================
// Nat Ryall                                                         30-Jun-2008
// =============================================================================
CMatch::~CMatch()
{
	OnDeinitialise();
}

// =============================================================================
// Nat Ryall                                                         30-Jun-2008
// =============================================================================
void CMatch::OnInitialise()
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
void CMatch::OnDeinitialise()
{
	if (m_pHTTP)
	{
		delete m_pHTTP;
		m_pHTTP = NULL;
	}

	if (m_pTCP)
	{
		m_pTCP->Stop();

		delete m_pTCP;
		m_pTCP = NULL;
	}

	m_iOperation = MatchOperation_None;
	m_iTimeout = 0;
}

// =============================================================================
// Nat Ryall                                                         02-Jul-2008
// =============================================================================
void CMatch::OnUpdate()
{
	if (IsBusy())
	{
		if (m_iTimeout == 0)
			m_iTimeout = _TIMEMS + MATCH_OPERATION_TIMEOUT;

		if (_TIMEMS > m_iTimeout)
		{
			m_pTCP->Stop();
			m_pTCP->Start(0, 64);

			ProcessError(MatchResultError_Timeout);

			return;
		}

		Packet* pPacket = m_pTCP->Receive();

		if (pPacket)
		{
			if (m_pHTTP->ProcessFinalTCPPacket(pPacket))
			{
				int iErrorCode = 0;
				RakNet::RakString xErrorString;

				if (m_pHTTP->HasBadResponse(&iErrorCode, &xErrorString))
				{
					XLOG("[MatchManager] Error: %d, '%s'", iErrorCode, xErrorString.C_String());
					ProcessError(MatchResultError_BadResponse);
				}
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

// =============================================================================
// Nat Ryall                                                         02-Jul-2008
// =============================================================================
xbool CMatch::ListSessions(t_OnListSessionsCompleted fpCallback)
{
	XMASSERT(!IsBusy(), "An operation is already processing.");
	XMASSERT(fpCallback, "A callback must be specified for this operation.");

	if (!IsBusy() && fpCallback)
	{
		// Generate and send the operation query.
		CMatchQuery xQuery;

		xQuery.AddValue("gid", _GID);
		xQuery.AddValue("limit", MATCH_SESSION_LIMIT);

		m_pHTTP->Post("/match.php?list", xQuery.GetQuery(), MATCH_ENCODING_TYPE);

		// Start the operation.
		m_iOperation = MatchOperation_ListSessions;
		m_fpOnListSessionsCompleted = fpCallback;

		return true;
	}

	if (fpCallback)
		fpCallback(MatchResultError_Busy, 0, NULL);

	return false;
}

// =============================================================================
// Nat Ryall                                                         02-Jul-2008
// =============================================================================
CSession* CMatch::CreateSession(xint iTotalSlots, t_OnCreateSessionCompleted fpCallback)
{
	XMASSERT(!IsBusy(), "An operation is already processing.");

	if (!IsBusy())
	{
		// Create a new session object.
		m_pSession = new CSession();

		m_pSession->m_iStatus = SessionStatus_Creating;
		m_pSession->m_bOwned = true;
		m_pSession->m_sSessionID = GenerateSessionID().c_str();
		m_pSession->m_iTotalSlots = iTotalSlots;
		m_pSession->m_sTitle = "PikPik";

		// Generate and send the operation query.
		CMatchQuery xQuery;

		xQuery.AddValue("gid", _GID);
		xQuery.AddValue("sid", m_pSession->m_sSessionID);
		xQuery.AddValue("title", m_pSession->m_sTitle);
		xQuery.AddValue("tslots", iTotalSlots);
		xQuery.AddValue("uslots", 1);
		xQuery.AddValue("info", m_pSession->m_sInfo);

		m_pHTTP->Post("/match.php?create", xQuery.GetQuery(), MATCH_ENCODING_TYPE);

		// Start the operation.
		m_iOperation = MatchOperation_CreateSession;
		m_fpOnCreateSessionCompleted = fpCallback;

		return m_pSession;
	}

	if (fpCallback)
		fpCallback(MatchResultError_Busy, NULL);

	return NULL;
}

// =============================================================================
// Nat Ryall                                                         03-Jul-2008
// =============================================================================
xbool CMatch::PingSession(CSession* pSession, t_OnPingSessionCompleted fpCallback)
{
	XMASSERT(!IsBusy(), "An operation is already processing.");

	if (!IsBusy())
	{
		// Generate and send the operation query.
		CMatchQuery xQuery;

		xQuery.AddValue("sid", pSession->m_sSessionID);
		xQuery.AddValue("pass", pSession->m_sPassword);

		m_pHTTP->Post("/match.php?ping", xQuery.GetQuery(), MATCH_ENCODING_TYPE);

		// Start the operation.
		m_pSession = pSession;
		m_iOperation = MatchOperation_PingSession;
		m_fpOnPingSessionCompleted = fpCallback;

		return true;
	}

	if (fpCallback)
		fpCallback(MatchResultError_Busy, pSession);

	return false;
}

// =============================================================================
// Nat Ryall                                                         02-Jul-2008
// =============================================================================
xbool CMatch::UpdateSession(CSession* pSession, t_OnUpdateSessionCompleted fpCallback)
{
	XMASSERT(!IsBusy(), "An operation is already processing.");

	if (!IsBusy())
	{
		// Generate and send the operation query.
		CMatchQuery xQuery;

		xQuery.AddValue("sid", pSession->m_sSessionID);
		xQuery.AddValue("pass", pSession->m_sPassword);
		xQuery.AddValue("state", pSession->m_iStatus);
		xQuery.AddValue("uslots", pSession->m_iTotalSlots);
		xQuery.AddValue("info", pSession->m_sInfo);

		m_pHTTP->Post("/match.php?update", xQuery.GetQuery(), MATCH_ENCODING_TYPE);

		// Start the operation.
		m_pSession = pSession;
		m_iOperation = MatchOperation_UpdateSession;
		m_fpOnUpdateSessionCompleted = fpCallback;

		return true;
	}

	if (fpCallback)
		fpCallback(MatchResultError_Busy, pSession);

	return false;
}

// =============================================================================
// Nat Ryall                                                         02-Jul-2008
// =============================================================================
xbool CMatch::CloseSession(CSession* pSession, t_OnCloseSessionCompleted fpCallback)
{
	XMASSERT(!IsBusy(), "An operation is already processing.");

	if (!IsBusy())
	{
		// Generate and send the operation query.
		CMatchQuery xQuery;

		xQuery.AddValue("sid", pSession->m_sSessionID);
		xQuery.AddValue("pass", pSession->m_sPassword);

		m_pHTTP->Post("/match.php?close", xQuery.GetQuery(), MATCH_ENCODING_TYPE);

		// Start the operation.
		pSession->m_iStatus = SessionStatus_Closing;
		m_pSession = pSession;

		m_iOperation = MatchOperation_CloseSession;
		m_fpOnCloseSessionCompleted = fpCallback;
		
		return true;
	}

	if (fpCallback)
		fpCallback(MatchResultError_Busy, pSession);

	return false;
}

// =============================================================================
// Nat Ryall                                                         08-Jul-2008
// =============================================================================
void CMatch::ProcessResult(RakNet::RakString* pResult)
{
	CMatchResult xResult;

	t_MatchResultError iError = xResult.ProcessResult(pResult->C_String());
	xbool bSuccess = (iError == MatchResultError_Success);

	t_MatchOperation iLastOperation = m_iOperation;

	m_iOperation = MatchOperation_None;
	m_iTimeout = 0;

	switch (iLastOperation)
	{
	// List Sessions.
	case MatchOperation_ListSessions:
		{
			static CSession s_xSessions[MATCH_SESSION_LIMIT];
			xint iSessionCount = 0;

			if (bSuccess)
			{
				iSessionCount = xResult.GetInt("results");
				
				for (xint iA = 0; iA < iSessionCount; ++iA)
				{
					s_xSessions[iA].m_iStatus = SessionStatus_Active;
					s_xSessions[iA].m_bOwned = false;
					s_xSessions[iA].m_sSessionID = xResult.GetString(XFORMAT("sid:%d", iA));
					s_xSessions[iA].m_sIP = xResult.GetString(XFORMAT("ip:%d", iA));
					s_xSessions[iA].m_sTitle = xResult.GetString(XFORMAT("title:%d", iA));
					s_xSessions[iA].m_iTotalSlots = xResult.GetInt(XFORMAT("tslots:%d", iA));
					s_xSessions[iA].m_iUsedSlots = xResult.GetInt(XFORMAT("uslots:%d", iA));
					s_xSessions[iA].m_sInfo = xResult.GetString(XFORMAT("info:%d", iA));
				}
			}

			if (m_fpOnListSessionsCompleted)
				m_fpOnListSessionsCompleted(iError, iSessionCount, bSuccess ? s_xSessions : NULL);
		}
		break;

	// Create Session.
	case MatchOperation_CreateSession:
		{
			if (bSuccess)
			{
				m_pSession->m_iStatus = SessionStatus_Active;
				m_pSession->m_sPassword = xResult.GetString("pass");
			}
			else
				m_pSession->m_iStatus = SessionStatus_Closed;

			if (m_fpOnCreateSessionCompleted)
				m_fpOnCreateSessionCompleted(iError, m_pSession);
		}
		break;

	// Ping Session.
	case MatchOperation_PingSession:
		{
			if (m_fpOnPingSessionCompleted)
				m_fpOnPingSessionCompleted(iError, m_pSession);
		}
		break;

	// Update Session.
	case MatchOperation_UpdateSession:
		{
			if (m_fpOnUpdateSessionCompleted)
				m_fpOnUpdateSessionCompleted(iError, m_pSession);
		}
		break;

	// Close Session.
	case MatchOperation_CloseSession:
		{
			m_pSession->m_iStatus = SessionStatus_Closed;

			if (m_fpOnCloseSessionCompleted)
				m_fpOnCloseSessionCompleted(iError, m_pSession);
		}
		break;
	}
}

// =============================================================================
// Nat Ryall                                                         09-Jul-2008
// =============================================================================
void CMatch::ProcessError(t_MatchResultError iError)
{
	RakNet::RakString sError = XFORMAT("%serror=%d", MATCH_RESULT_HEADER, iError);
	ProcessResult(&sError);
}

// =============================================================================
// Nat Ryall                                                         02-Jul-2008
// =============================================================================
xstring CMatch::GenerateSessionID()
{
	static const xchar* s_pChars = "0123456789ABCDEF";
	static const xint s_iNumChars = (xint)strlen(s_pChars);

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
// Nat Ryall                                                         09-Jul-2008
// =============================================================================
CSession::CSession() :
	m_iStatus(SessionStatus_Unknown),
	m_bOwned(false),
	m_iTotalSlots(0),
	m_iUsedSlots(0)
{
}

// =============================================================================
// Nat Ryall                                                         09-Jul-2008
// =============================================================================
xbool CSession::PlayerExists(xstring sName)
{
	XEN_LIST_FOREACH(t_StringList, ppPlayer, m_lpPlayers)
	{
		if (*ppPlayer == sName)
			return true;
	}

	return false;
}

// =============================================================================
// Nat Ryall                                                         02-Jul-2008
// =============================================================================
xbool CSession::AddPlayer(xstring sName)
{
	if (!PlayerExists(sName) && m_iUsedSlots < m_iTotalSlots)
	{
		m_iUsedSlots++;
		m_lpPlayers.push_back(sName);

		return true;
	}

	return false;
}

// =============================================================================
// Nat Ryall                                                         02-Jul-2008
// =============================================================================
xbool CSession::RemovePlayer(xstring sName)
{
	if (PlayerExists(sName) && m_iUsedSlots)
	{
		m_iUsedSlots--;
		XEN_LIST_REMOVE(t_StringList, m_lpPlayers, sName);

		return true;
	}

	return false;
}

//##############################################################################