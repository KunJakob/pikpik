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

// Lists.
typedef xvlist<CSession*> t_SessionList;

//##############################################################################

//##############################################################################
//
//                               MATCH MANAGER
//
//##############################################################################
class CMatchManager
{
public:
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

	// Create a new session with the specified properties.
	CSession* CreateSession(xint iSlots);

protected:
	// The TCP interface to the managing webserver.
	TCPInterface* m_pTCP;

	// The HTTP connection to the managing webserver.
	HTTPConnection* m_pHTTP;

	// The internal session list.
	t_SessionList m_lpSessions;
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
	// The number of used player slots in the session.
	xint m_iUsedSlots;

	// The total number of player slots available in the session.
	xint m_iTotalSlots;
};

//##############################################################################