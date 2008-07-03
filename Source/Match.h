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
#define MatchManager CMatchManager::Get()

//##############################################################################

//##############################################################################
//
//                                   TYPES
//
//##############################################################################

// Predeclare.
class CSession;

// The possible session states.
enum t_SessionState
{
	SessionState_Idle,
	SessionState_Creating,
	SessionState_Destroying,
};

// Lists.
typedef xvlist<CSession*> t_SessionList;

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

	// AddValue a key-pair to the query using a string value.
	void AddValue(const xchar* pKey, const xchar* pValue);

	// AddValue a key-pair to the query using an integer value.
	void AddValue(const xchar* pKey, xint iValue);

	// Get the final query to send to the matchmaking server.
	inline const xchar* GetQuery()
	{
		return m_sQuery.c_str();
	}

protected:
	// Format the string to remove any unsafe characters.
	xstring FormatString(xstring sString);

	// The query header.
	const xchar* m_pHeader;

	// The final formatted query to be sent.
	xstring m_sQuery;
};

//##############################################################################

//##############################################################################
//
//                               MATCH MANAGER
//
//##############################################################################
class CMatchManager
{
public:
	// Callbacks.
	typedef xfunction(3)<xbool /*Success*/, xint /*SessionCount*/, CSession* /*Sessions*/> t_OnListSessionsCompleted;
	typedef xfunction(2)<xbool /*Success*/, CSession* /*Session*/> t_OnCreateSessionCompleted;
	typedef xfunction(1)<xbool /*Success*/> t_OnPingSessionCompleted;
	typedef xfunction(2)<xbool /*Success*/, CSession* /*Session*/> t_OnUpdateSessionCompleted;
	typedef xfunction(1)<xbool /*Success*/> t_OnDestroySessionCompleted;

	// Create a new session with the specified properties.
	CSession* TEMP_QueryServer(xint iSlots);

	// Singleton instance.
	static inline CMatchManager& Get() 
	{
		static CMatchManager s_Instance;
		return s_Instance;
	}

	// Constructor.
	CMatchManager();

	// Destructor.
	virtual ~CMatchManager();

	// Initialise the matchmaking service.
	void Initialise();

	// Deinitialise the matchmaking service.
	void Deinitialise();

	//
	void Update();

	// 
	xbool ListSessions(t_OnListSessionsCompleted fpCallback);

	// 
	CSession* CreateSession(xint iTotalSlots, t_OnCreateSessionCompleted fpCallback);

	//
	xbool PingSession(CSession* pSession, t_OnPingSessionCompleted fpCallback);

	// 
	xbool UpdateSession(CSession* pSession, t_OnUpdateSessionCompleted fpCallback);

	//
	void DestroySession(CSession* pSession, t_OnDestroySessionCompleted fpCallback);

protected:
	// Generate a unique session ID.
	xstring GenerateSessionID();

	// The TCP interface to the managing webserver.
	TCPInterface* m_pTCP;

	// The HTTP connection to the managing webserver.
	HTTPConnection* m_pHTTP;

	// The internal session list.
	t_SessionList m_lpSessions;

	// Callbacks.
	t_OnListSessionsCompleted m_fpOnListSessionsCompleted;
	t_OnCreateSessionCompleted m_fpOnCreateSessionCompleted;
	t_OnPingSessionCompleted m_fpOnPingSessionCompleted;
	t_OnUpdateSessionCompleted m_fpOnUpdateSessionCompleted;
	t_OnDestroySessionCompleted m_fpOnDestroySessionCompleted;
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
	friend class CMatchManager;

	//
	void AddPlayer(xstring sName);

	//
	void RemovePlayer(xstring sName);

	//
	inline t_SessionState GetState()
	{
		return m_iState;
	}

protected:
	// Types.
	typedef xlist<xstring> t_StringList;

	// The current session state.
	t_SessionState m_iState;

	// Specifies if the session is owned by the local machine.
	xbool m_bOwned; 

	// The ID that is set internally in the match manager.
	xstring m_sSessionID;

	// The session password returned when the session is created. This is valid only on the host, NULL otherwise.
	xstring m_sPassword;

	// The total number of player slots available in the session.
	xint m_iTotalSlots; 

	// The current players in the session. Used slots is also derived from this list.
	t_StringList m_lpPlayers;

	// The title of the session.
	xstring m_sTitle;

	// The custom session info. Set to NULL or specify extra data to be uploaded.
	xstring m_sSessionInfo;
};

//##############################################################################