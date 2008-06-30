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
//                               MATCH MANAGER
//
//##############################################################################

//##############################################################################
//
//                                   SESSION
//
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
// Nat Ryall                                                         30-Jun-2008
// =============================================================================
CSession* CMatchManager::CreateSession(xint iSlots)
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

//##############################################################################

//##############################################################################


//##############################################################################