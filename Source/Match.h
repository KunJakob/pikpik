#pragma once

/**
* @file Match.h
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

//##############################################################################

//##############################################################################
//
//                                   MACROS
//
//##############################################################################

// Singleton accessor.
#define Match CMatch::Get()

// Query & result headers.
#define MATCH_QUERY_HEADER "match://"
#define MATCH_RESULT_HEADER "result://"

// The operation timeout in milliseconds.
#define MATCH_OPERATION_TIMEOUT 8000

// The number of rows to retrieve when listing sessions.
#define MATCH_SESSION_LIMIT 8

//##############################################################################

//##############################################################################
//
//                                   TYPES
//
//##############################################################################

// Predeclare.
class CSession;

// Match result error codes (linked to the match service).
enum t_MatchResultError
{
	MatchResultError_Success,
	MatchResultError_QueryFailed,
	MatchResultError_OwnerExists,
	MatchResultError_SessionExists,
	MatchResultError_InvalidSession,
	MatchResultError_InvalidPassword,
	MatchResultError_NoResults,

	MatchResultError_Busy,
	MatchResultError_Timeout,
	MatchResultError_BadResponse,
	MatchResultError_InvalidHeader,
};

// The various operations that can occur in the matchmaking service.
enum t_MatchOperation
{
	MatchOperation_None,
	MatchOperation_ListSessions,
	MatchOperation_CreateSession,
	MatchOperation_PingSession,
	MatchOperation_UpdateSession,
	MatchOperation_CloseSession,
};

// The possible session status (linked to the match service).
enum t_SessionStatus
{
	SessionStatus_Unknown,
	SessionStatus_Creating,
	SessionStatus_Active,
	SessionStatus_Started,
	SessionStatus_Closed,
	SessionStatus_Timeout,
};

// Lists.
//typedef xvlist<CSession*> t_SessionList;

//##############################################################################

//##############################################################################
//
//                                 MATCH QUERY
//
//##############################################################################
class CMatchQuery
{
public:
	// Constructor.
	CMatchQuery();

	// Reset the query to the default string.
	void Reset();

	// Add a key-pair to the query using a string value.
	void AddValue(const xchar* pKey, const xchar* pValue);

	// Add a key-pair to the query using a string value.
	void AddValue(const xchar* pKey, xstring& sValue);

	// Add a key-pair to the query using an integer value.
	void AddValue(const xchar* pKey, xint iValue);

	// Get the final query to send to the matchmaking server.
	inline const xchar* GetQuery()
	{
		return m_sQuery.c_str();
	}

protected:
	// Format the string to remove any unsafe characters.
	xstring FormatString(xstring sString);

	// The final formatted query to be sent.
	xstring m_sQuery;
};

//##############################################################################

//##############################################################################
//
//                                MATCH RESULT
//
//##############################################################################
class CMatchResult
{
public:
	// Process the result returned from the web query.
	t_MatchResultError ProcessResult(const xchar* pResult);

	// Check if a key exists in the result.
	xbool KeyExists(const xchar* pKey);

	// Get a value for the specified key as a string.
	const xchar* GetString(const xchar* pKey);

	// Get a value for the specified key as an integer.
	xint GetInt(const xchar* pKey);

protected:
	// Types.
	typedef xhash<xstring, xstring> t_StringHash;

	// The hash table of key-value pairs.
	t_StringHash m_lpPairs;
};

//##############################################################################

//##############################################################################
//
//                                   MATCH
//
//##############################################################################
class CMatch
{
public:
	// Callbacks.
	typedef xfunction(3)<t_MatchResultError /*Error*/, xint /*SessionCount*/, CSession* /*Sessions*/> t_OnListSessionsCompleted;
	typedef xfunction(2)<t_MatchResultError /*Error*/, CSession* /*Session*/> t_OnCreateSessionCompleted;
	typedef xfunction(2)<t_MatchResultError /*Error*/, CSession* /*Session*/> t_OnPingSessionCompleted;
	typedef xfunction(2)<t_MatchResultError /*Error*/, CSession* /*Session*/> t_OnUpdateSessionCompleted;
	typedef xfunction(2)<t_MatchResultError /*Error*/, CSession* /*Session*/> t_OnCloseSessionCompleted;

	// Singleton instance.
	static inline CMatch& Get() 
	{
		static CMatch s_Instance;
		return s_Instance;
	}

	// Constructor.
	CMatch();

	// Destructor.
	virtual ~CMatch();

	// Initialise the matchmaking service.
	void Initialise();

	// Deinitialise the matchmaking service.
	void Deinitialise();

	// Update the service and process any pending operation.
	void Update();

	// List a selection of available sessions on the server.
	xbool ListSessions(t_OnListSessionsCompleted fpCallback);

	// Create a new session with the specified params.
	CSession* CreateSession(xint iTotalSlots, t_OnCreateSessionCompleted fpCallback);

	// Ping a session to prevent it from timing out.
	xbool PingSession(CSession* pSession, t_OnPingSessionCompleted fpCallback);

	// Update the session details to the matchmaking server.
	xbool UpdateSession(CSession* pSession, t_OnUpdateSessionCompleted fpCallback);

	// Close a session cleanly.
	xbool CloseSession(CSession* pSession, t_OnCloseSessionCompleted fpCallback);

	// Get the currently pending operation.
	inline t_MatchOperation GetCurrentOperation()
	{
		return m_iOperation;
	}

	// Check if the system is currently busy.
	inline xbool IsBusy()
	{
		return m_iOperation != MatchOperation_None;
	}

protected:
	// Process a query result.
	void ProcessResult(RakNet::RakString* pString);

	// Process a local error as a query result.
	void ProcessError(t_MatchResultError iError);

	// Generate a unique session ID.
	xstring GenerateSessionID();

	// The TCP interface to the managing webserver.
	TCPInterface* m_pTCP;

	// The HTTP connection to the managing webserver.
	HTTPConnection* m_pHTTP;

	// The current matchmaking operation.
	t_MatchOperation m_iOperation;

	// The session being used in the current operation.
	CSession* m_pSession;

	// The operation timeout counter.
	xuint m_iTimeout;

	// The internal session list.
	//t_SessionList m_lpSessions;

	// Callbacks.
	t_OnListSessionsCompleted m_fpOnListSessionsCompleted;
	t_OnCreateSessionCompleted m_fpOnCreateSessionCompleted;
	t_OnPingSessionCompleted m_fpOnPingSessionCompleted;
	t_OnUpdateSessionCompleted m_fpOnUpdateSessionCompleted;
	t_OnCloseSessionCompleted m_fpOnCloseSessionCompleted;
};

//##############################################################################

//##############################################################################
//
//                                   SESSION
//
//##############################################################################
class CSession
{
public:
	// Friends.
	friend class CMatch;

	// Add a player to the session by name.
	xbool AddPlayer(xstring sName);

	// Remove a player from the session by name.
	xbool RemovePlayer(xstring sName);

	// Get the current session status.
	inline t_SessionStatus GetStatus()
	{
		return m_iStatus;
	}

protected:
	// Check to see if the specified player exists in the session.
	xbool PlayerExists(xstring sName);
	
	// Types.
	typedef xlist<xstring> t_StringList;

	// The current session state.
	t_SessionStatus m_iStatus;

	// Specifies if the session is owned by the local machine.
	xbool m_bOwned; 

	// The ID that is set internally in the match manager.
	xstring m_sSessionID;

	// The session password returned when the session is created. This is valid only on the host, NULL otherwise.
	xstring m_sPassword;

	// The IP address of the session.
	xstring m_sIP;

	// The title of the session.
	xstring m_sTitle;

	// The total number of player slots available in the session.
	xint m_iTotalSlots; 

	// The number of currently used slots in the session.
	xint m_iUsedSlots;

	// The current players in the session.
	t_StringList m_lpPlayers;

	// The custom session info. Set to NULL or specify extra data to be uploaded.
	xstring m_sInfo;
};

//##############################################################################