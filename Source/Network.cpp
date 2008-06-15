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

		m_pHostPeer = CreatePeer();
		m_pLocalPeer = m_pHostPeer;

		m_pLocalPeer->m_bHost = true;
		m_pLocalPeer->m_bLocal = true;
		m_pLocalPeer->m_iID = 0;
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

		m_pHostPeer = NULL;
		m_pLocalPeer = NULL;
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

			xuchar* pData = &pPacket->data[1];
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

		m_xCallbacks.m_fpConnectionCompleted = NULL;
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

	pPeer->m_bHost = false;
	pPeer->m_bLocal = false;
	pPeer->m_iID = (m_bHosting) ? GetUniquePeerID() : -1;
	pPeer->m_pData = NULL;

	pPeer->m_xAddress.binaryAddress = 0;
	pPeer->m_xAddress.port = 0;

	m_lpPeers.push_back(pPeer);

	return pPeer;
}

// =============================================================================
// Nat Ryall                                                         13-Jun-2008
// =============================================================================
xint CNetwork::GetUniquePeerID()
{
	return m_iLastPeerID++ % (1 << 8);
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
CNetworkPeer* CNetwork::FindPeer(SystemAddress* pAddress)
{
	XEN_LIST_FOREACH(t_NetworkPeerList, ppPeer, m_lpPeers)
	{
		if ((*ppPeer)->m_xAddress == *pAddress)
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
void CNetwork::ProcessHostNotifications(xchar cIdentifier, Packet* pPacket, xuchar* pData, xint iDataSize)
{
	BitStream xInStream(pData, iDataSize, false);

	switch (cIdentifier)
	{
	case ID_NEW_INCOMING_CONNECTION:
		{
			CNetworkPeer* pJoiningPeer = CreatePeer();
			pJoiningPeer->m_xAddress = pPacket->systemAddress;

			XEN_LIST_FOREACH_R(t_NetworkPeerList, ppPeer, m_lpPeers)
			{
				if (!(*ppPeer)->m_bLocal)
				{
					BitStream xOutStream;

					xOutStream.Write((xuint8)ID_PEER_JOINED);
					xOutStream.Write((xbool)(pJoiningPeer->m_iID == (*ppPeer)->m_iID));
					xOutStream.Write((xuint16)pJoiningPeer->m_iID);

					m_pInterface->Send(&xOutStream, HIGH_PRIORITY, RELIABLE, 1, pPacket->systemAddress, false);
				}
			}

			BitStream xOutStream;

			xOutStream.Write((xuint8)ID_PEER_JOINED);
			xOutStream.Write(false);
			xOutStream.Write((xuint16)pJoiningPeer->m_iID);

			m_pInterface->Send(&xOutStream, HIGH_PRIORITY, RELIABLE, 1, pPacket->systemAddress, true);

			if (m_xCallbacks.m_fpPeerJoined)
				m_xCallbacks.m_fpPeerJoined(pJoiningPeer);
		}
		break;

	case ID_DISCONNECTION_NOTIFICATION:
		{
			CNetworkPeer* pLeavingPeer = FindPeer(&pPacket->systemAddress);
			
			if (pLeavingPeer)
			{
				BitStream xOutStream;

				xOutStream.Write((xuint8)ID_PEER_LEAVING);
				xOutStream.Write((xuint16)pLeavingPeer->m_iID);

				m_pInterface->Send(&xOutStream, HIGH_PRIORITY, RELIABLE, 1, UNASSIGNED_SYSTEM_ADDRESS, true);
			}

			if (m_xCallbacks.m_fpPeerLeaving)
				m_xCallbacks.m_fpPeerLeaving(pLeavingPeer);

			DestroyPeer(pLeavingPeer->m_iID);
		}
		break;

	// Received a data packet from a client.
	case ID_DATA_PACKET:
		{
			ProcessPacket(&xInStream);
		}
		break;
	}
}

// =============================================================================
// Nat Ryall                                                         09-Jun-2008
// =============================================================================
void CNetwork::ProcessClientNotifications(xchar cIdentifier, Packet* pPacket, xuchar* pData, xint iDataSize)
{
	BitStream xInStream(pData, iDataSize, false);

	switch (cIdentifier)
	{
	// Client successfully/unsuccessfully connected to the host.
	case ID_CONNECTION_REQUEST_ACCEPTED:
		{
			m_pHostPeer = CreatePeer();

			m_pHostPeer->m_bHost = true;
			m_pHostPeer->m_bLocal = false;
			m_pHostPeer->m_iID = 0;
			m_pHostPeer->m_xAddress = pPacket->systemAddress;

			if (m_xCallbacks.m_fpPeerJoined)
				m_xCallbacks.m_fpPeerJoined(m_pHostPeer);

			if (m_xCallbacks.m_fpConnectionCompleted)
				m_xCallbacks.m_fpConnectionCompleted(true);
		}
		break;

	case ID_CONNECTION_ATTEMPT_FAILED:
		{
			if (m_xCallbacks.m_fpConnectionCompleted)
				m_xCallbacks.m_fpConnectionCompleted(false);
		}
		break;

	// The connection to the host has been lost.
	case ID_DISCONNECTION_NOTIFICATION:
	case ID_CONNECTION_LOST:
		{
			if (m_xCallbacks.m_fpConnectionLost)
				m_xCallbacks.m_fpConnectionLost();

			Reset();
		}
		break;

	// A new peer has been added to the game.
	case ID_PEER_JOINED:
		{
			CNetworkPeer* pPeer = CreatePeer();

			xbool bLocal = false;
			xuint16 iID = 0;

			xInStream.Read(bLocal);
			xInStream.Read(iID);
			
			pPeer->m_bHost = false;
			pPeer->m_bLocal = bLocal;
			pPeer->m_iID = (xint)iID;

			if (bLocal)
				m_pLocalPeer = pPeer;

			if (m_xCallbacks.m_fpPeerJoined)
				m_xCallbacks.m_fpPeerJoined(pPeer);
		}
		break;

	// A peer has been removed from the game.
	case ID_PEER_LEAVING:
		{
			xuint16 iID = 0;
			xInStream.Read(iID);

			CNetworkPeer* pPeer = FindPeer(iID);

			if (pPeer && m_xCallbacks.m_fpPeerLeaving)
				m_xCallbacks.m_fpPeerLeaving(pPeer);

			DestroyPeer(iID);
		}
		break;

	// Received a data packet from the host.
	case ID_DATA_PACKET:
		{
			ProcessPacket(&xInStream);
		}
		break;
	}
}

// =============================================================================
// Nat Ryall                                                         13-Jun-2008
// =============================================================================
void CNetwork::ProcessPacket(BitStream* pStream)
{
	xuint16 iID = 0;
	pStream->Read(iID);

	CNetworkPeer* pPeer = FindPeer(iID);

	if (pPeer && m_xCallbacks.m_fpPacketReceived)
		m_xCallbacks.m_fpPacketReceived(pPeer, pStream);
}

// =============================================================================
// Nat Ryall                                                         13-Jun-2008
// =============================================================================
void CNetwork::DisconnectPeer(CNetworkPeer* pPeer)
{
	XMASSERT(false, "DisconnectPeer() is not implemented yet.");
}

//##############################################################################