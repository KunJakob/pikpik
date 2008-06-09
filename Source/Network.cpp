//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Global.
#include <Global.h>

// Local.
#include <Network.h>

//##############################################################################

//##############################################################################
//
//                                   NETWORK
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         08-Jun-2008
// =============================================================================
CNetwork::CNetwork()
{
	m_pInterface = NULL;

	Reset();
}

// =============================================================================
// Nat Ryall                                                         08-Jun-2008
// =============================================================================
void CNetwork::StartHost(xint iMaxPeers)
{
	XASSERT(!m_pInterface);

	if (!m_pInterface)
	{
		m_bHosting = true;
		m_pInterface = RakNetworkFactory::GetRakPeerInterface();

		m_xSocket.hostAddress[0] = NULL;
		m_xSocket.port = 20557;

		m_pInterface->Startup(iMaxPeers, 0, &m_xSocket, 1);
		m_pInterface->SetMaximumIncomingConnections(iMaxPeers);

		m_pLocalPeer = CreatePeer();
	}
}

// =============================================================================
// Nat Ryall                                                         08-Jun-2008
// =============================================================================
void CNetwork::StartClient(const xchar* pHostAddress, xint iHostPort)
{
	XASSERT(!m_pInterface);

	if (!m_pInterface)
	{
		m_bHosting = false;
		m_pInterface = RakNetworkFactory::GetRakPeerInterface();

		m_xSocket.hostAddress[0] = NULL;
		m_xSocket.port = 20558;

		m_pInterface->Startup(1, 0, &m_xSocket, 1);
		m_pInterface->Connect(pHostAddress, iHostPort, NULL, 0, 0);
	}
}

// =============================================================================
// Nat Ryall                                                         08-Jun-2008
// =============================================================================
void CNetwork::Stop()
{
	XASSERT(m_pInterface);

	if (m_pInterface)
	{
		RakNetworkFactory::DestroyRakPeerInterface(m_pInterface);
		m_pInterface = NULL;

		Reset();
	}
}

// =============================================================================
// Nat Ryall                                                         08-Jun-2008
// =============================================================================
void CNetwork::Update()
{
	if (m_pInterface)
	{
		if (Packet* pPacket = m_pInterface->Receive())
		{
			xchar cIdentifier = pPacket->data[0];

			xchar* pData = &pPacket->data[1];
			xint iDataSize = pPacket->length;

			ProcessHostNotifications(cIdentifier, pPacket, pData, iDataSize);
			ProcessClientNotifications(cIdentifier, pPacket, pData, iDataSize);
			
			m_pInterface->DeallocatePacket(pPacket);
		}
	}
}

// =============================================================================
// Nat Ryall                                                         08-Jun-2008
// =============================================================================
void CNetwork::Reset()
{
	XASSERT(!m_pInterface);

	if (!m_pInterface)
	{
		m_bHosting = false;
		m_bConnected = false;

		m_iLastPeerID = 0;

		m_pLocalPeer = NULL;
		m_pHostPeer = NULL;

		XEN_LIST_ERASE_ALL(m_lpPeers);

		m_xCallbacks.m_fpConnectionEstablished = NULL;
		m_xCallbacks.m_fpConnectionLost = NULL;
		m_xCallbacks.m_fpPeerJoined = NULL;
		m_xCallbacks.m_fpPeerLeaving = NULL;
		m_xCallbacks.m_fpPacketReceived = NULL;
	}
}

// =============================================================================
// Nat Ryall                                                         09-Jun-2008
// =============================================================================
CNetworkPeer* CNetwork::CreatePeer()
{
	CNetworkPeer* pPeer = new CNetworkPeer();


	//	pStream->Read(&pPeer->m_iID, 2);


	pPeer->m_iID = (m_bHosting) ? (m_iLastPeerID++ % (1 << 8)) : -1;
	pPeer->m_pData = NULL;

	m_lpPeers.push_back(pPeer);

	return pPeer;
}

// =============================================================================
// Nat Ryall                                                         09-Jun-2008
// =============================================================================
void CNetwork::DestroyPeer(SystemAddress* pAddress)
{
	CNetworkPeer* pPeer = FindPeer(pAddress);

	if (pPeer)
		XEN_LIST_ERASE(t_NetworkPeerList, m_lpPeers, pPeer);
}

// =============================================================================
// Nat Ryall                                                         09-Jun-2008
// =============================================================================
void CNetwork::DestroyPeer(xint iPeerID)
{
	CNetworkPeer* pPeer = FindPeer(iPeerID);

	if (pPeer)
		XEN_LIST_ERASE(t_NetworkPeerList, m_lpPeers, pPeer);
}

// =============================================================================
// Nat Ryall                                                         09-Jun-2008
// =============================================================================
void CNetwork::FindPeer(SystemAddress* pAddress)
{
	XEN_LIST_FOREACH(t_NetworkPeerList, ppPeer, m_lpPeers)
	{
		if ((*ppPeer)->m_xSystemAddress == *pAddress)
			return *ppPeer;
	}

	return NULL;
}

// =============================================================================
// Nat Ryall                                                         09-Jun-2008
// =============================================================================
CNetworkPeer* CNetwork::FindPeer(xint iPeerID)
{
	XEN_LIST_FOREACH(t_NetworkPeerList, ppPeer, m_lpPeers)
	{
		if ((*ppPeer)->m_iID == iPeerID)
			return *ppPeer;
	}

	return NULL;
}

// =============================================================================
// Nat Ryall                                                         09-Jun-2008
// =============================================================================
void CNetwork::ProcessHostNotifications(xchar* cIdentifier, Packet* pPacket, xchar* pData, xint iDataSize)
{
	BitStream xInStream(pData, iDataSize, false);

	switch (cIdentifier)
	{
	case ID_NEW_INCOMING_CONNECTION:
		{
			CNetworkPeer* pPeer = CreatePeer();
			pPeer->m_xAddress = pPacket->systemAddress;

			XEN_LIST_FOREACH_R(t_NetworkPeerList, ppPeer, m_lpPeers)
			{
				BitStream xOutStream;

				xOutStream.Write((xuint8)ID_PEER_JOINED);
				xOutStream.Write((xbool)pPeer->m_iID == (*ppPeer)->m_iID);
				xOutStream.Write((xuint16)pPeer->m_iID);
				xOutStream.WriteAlignedBytes(&(*ppPeer)->m_xSystemAddress, sizeof(SystemAddress));

				m_pInterface->Send(&xOutStream, HIGH_PRIORITY, RELIABLE_ORDERED, 1, pPacket->systemAddress, false);
			}
		}
		break;

	case ID_DISCONNECTION_NOTIFICATION:
		{
		}
		break;

	// Received a data packet from a client.
	case ID_DATA_PACKET:
		{
			if (m_xCallbacks.m_fpPacketReceived)
				m_xCallbacks.m_fpPacketReceived(NULL, &xInStream);
		}
	}
}

// =============================================================================
// Nat Ryall                                                         09-Jun-2008
// =============================================================================
void CNetwork::ProcessClientNotifications(xchar* cIdentifier, Packet* pPacket, xchar* pData, xint iDataSize)
{
	BitStream xInStream(pData, iDataSize, false);

	switch (cIdentifier)
	{
	// Client successfully/unsuccessfully connected to the host.
	case ID_CONNECTION_REQUEST_ACCEPTED:
		{
			if (m_xCallbacks.m_fpConnectionEstablished)
				m_xCallbacks.m_fpConnectionEstablished();
		}
		break;

	case ID_CONNECTION_ATTEMPT_FAILED:
		{
		}
		break;

	// The connection to the host has been lost.
	case ID_DISCONNECTION_NOTIFICATION:
	case ID_CONNECTION_LOST:
		{
		}
		break;

	// A new peer has been added to the game.
	case ID_PEER_JOINED:
		{
			CNetworkPeer* pPeer = CreatePeer();

			xbool bLocal = false;
			xuint16 iID = 0;
			SystemAddress xAddress;

			xInStream.Read(&bLocal);
			xInStream.Read(&iID);
			xInStream.ReadAlignedBytes(&xAddress, sizeof(SystemAddress));
			
			pPeer->m_bLocal = bLocal;
			pPeer->m_iID = (xint)iID;
			pPeer->m_xAddress = xAddress;

			if (bLocal)
				m_pLocalPeer = pPeer;

			if (iID == 0)
				m_pHostPeer = pPeer;

			if (m_xCallbacks.m_fpPeerJoined)
				m_xCallbacks.m_fpPeerJoined(pPeer);
		}
		break;

	// A peer has been removed from the game.
	case ID_PEER_LEAVING:
		{

		}
		break;

	// Received a data packet from the host.
	case ID_DATA_PACKET:
		{
			//FindPeer() // Read first byte for peer ID.

			if (m_xCallbacks.m_fpPacketReceived)
				m_xCallbacks.m_fpPacketReceived(NULL, &xInStream);
		}
	}
}

//##############################################################################