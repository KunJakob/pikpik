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

		m_xCallbacks.m_fpNetworkStarted = NULL;
		m_xCallbacks.m_fpNetworkStopped = NULL;
		m_xCallbacks.m_fpConnectionCompleted = NULL;
		m_xCallbacks.m_fpConnectionLost = NULL;
		m_xCallbacks.m_fpPeerJoined = NULL;
		m_xCallbacks.m_fpPeerLeaving = NULL;
		m_xCallbacks.m_fpPacketReceived = NULL;

		m_bStopPending = false;
	}
}

// =============================================================================
// Nat Ryall                                                         08-Jun-2008
// =============================================================================
void CNetwork::Update()
{
	if (m_bStopPending)
		Stop();

	if (m_pInterface)
	{
		while (Packet* pPacket = m_pInterface->Receive())
		{
			xchar cIdentifier = pPacket->data[0];

			xuchar* pData = &pPacket->data[1];
			xint iDataSize = pPacket->length;

			static const char* s_pNetworkID[] =
			{
				"ID_INTERNAL_PING",
				"ID_PING",
				"ID_PING_OPEN_CONNECTIONS",
				"ID_CONNECTED_PONG",
				"ID_CONNECTION_REQUEST",
				"ID_SECURED_CONNECTION_RESPONSE",
				"ID_SECURED_CONNECTION_CONFIRMATION",
				"ID_RPC_MAPPING",
				"ID_DETECT_LOST_CONNECTIONS",
				"ID_OPEN_CONNECTION_REQUEST",
				"ID_OPEN_CONNECTION_REPLY",
				"ID_RPC",
				"ID_RPC_REPLY",
				"ID_OUT_OF_BAND_INTERNAL",
				"ID_CONNECTION_REQUEST_ACCEPTED",
				"ID_CONNECTION_ATTEMPT_FAILED",
				"ID_ALREADY_CONNECTED",
				"ID_NEW_INCOMING_CONNECTION",
				"ID_NO_FREE_INCOMING_CONNECTIONS",
				"ID_DISCONNECTION_NOTIFICATION",
				"ID_CONNECTION_LOST",
				"ID_RSA_PUBLIC_KEY_MISMATCH",
				"ID_CONNECTION_BANNED",
				"ID_INVALID_PASSWORD",
				"ID_MODIFIED_PACKET",
				"ID_TIMESTAMP",
				"ID_PONG",
				"ID_ADVERTISE_SYSTEM",
				"ID_REMOTE_DISCONNECTION_NOTIFICATION",
				"ID_REMOTE_CONNECTION_LOST",
				"ID_REMOTE_NEW_INCOMING_CONNECTION",
				"ID_DOWNLOAD_PROGRESS",
				"ID_FILE_LIST_TRANSFER_HEADER",
				"ID_FILE_LIST_TRANSFER_FILE",
				"ID_DDT_DOWNLOAD_REQUEST",
				"ID_TRANSPORT_STRING",
				"ID_REPLICA_MANAGER_CONSTRUCTION",
				"ID_REPLICA_MANAGER_DESTRUCTION",
				"ID_REPLICA_MANAGER_SCOPE_CHANGE",
				"ID_REPLICA_MANAGER_SERIALIZE",
				"ID_REPLICA_MANAGER_DOWNLOAD_COMPLETE",
				"ID_CONNECTION_GRAPH_REQUEST",
				"ID_CONNECTION_GRAPH_REPLY",
				"ID_CONNECTION_GRAPH_UPDATE",
				"ID_CONNECTION_GRAPH_NEW_CONNECTION",
				"ID_CONNECTION_GRAPH_CONNECTION_LOST",
				"ID_CONNECTION_GRAPH_DISCONNECTION_NOTIFICATION",
				"ID_ROUTE_AND_MULTICAST",
				"ID_RAKVOICE_OPEN_CHANNEL_REQUEST",
				"ID_RAKVOICE_OPEN_CHANNEL_REPLY",
				"ID_RAKVOICE_CLOSE_CHANNEL",
				"ID_RAKVOICE_DATA",
				"ID_AUTOPATCHER_GET_CHANGELIST_SINCE_DATE",
				"ID_AUTOPATCHER_CREATION_LIST",
				"ID_AUTOPATCHER_DELETION_LIST",
				"ID_AUTOPATCHER_GET_PATCH",
				"ID_AUTOPATCHER_PATCH_LIST",
				"ID_AUTOPATCHER_REPOSITORY_FATAL_ERROR",
				"ID_AUTOPATCHER_FINISHED_INTERNAL",
				"ID_AUTOPATCHER_FINISHED",
				"ID_AUTOPATCHER_RESTART_APPLICATION",
				"ID_NAT_PUNCHTHROUGH_REQUEST",
				"ID_NAT_TARGET_NOT_CONNECTED",
				"ID_NAT_TARGET_CONNECTION_LOST",
				"ID_NAT_CONNECT_AT_TIME",
				"ID_NAT_SEND_OFFLINE_MESSAGE_AT_TIME",
				"ID_DATABASE_QUERY_REQUEST",
				"ID_DATABASE_UPDATE_ROW",
				"ID_DATABASE_REMOVE_ROW",
				"ID_DATABASE_QUERY_REPLY",
				"ID_DATABASE_UNKNOWN_TABLE",
				"ID_DATABASE_INCORRECT_PASSWORD",
				"ID_READY_EVENT_SET",
				"ID_READY_EVENT_UNSET",
				"ID_READY_EVENT_ALL_SET",
				"ID_READY_EVENT_QUERY",
				"ID_LOBBY_GENERAL",
				"ID_DATA_PACKET",
				"ID_PEER_JOINED",
				"ID_PEER_LEAVING",
			};

			XLOG("[Network] Packet: %d (%s), %d", cIdentifier, s_pNetworkID[cIdentifier], iDataSize);

			if (m_bHosting)
				ProcessHostNotifications(cIdentifier, pPacket, pData, iDataSize);
			else
				ProcessClientNotifications(cIdentifier, pPacket, pData, iDataSize);

			m_pInterface->DeallocatePacket(pPacket);

			if (m_bStopPending)
				return;
		}
	}
}

// =============================================================================
// Nat Ryall                                                         08-Jun-2008
// =============================================================================
void CNetwork::StartHost(xint iMaxPeers, xint iPort, void* pData, xint iDataSize)
{
	XASSERT(!m_pInterface);

	if (!m_pInterface)
	{
		XLOG("[Network] Starting network as a host.");

		m_bHosting = true;
		m_pInterface = RakNetworkFactory::GetRakPeerInterface();

		m_xSocket.hostAddress[0] = NULL;
		m_xSocket.port = iPort;

		m_pInterface->Startup(iMaxPeers, 0, &m_xSocket, 1);
		m_pInterface->SetMaximumIncomingConnections(iMaxPeers);
		m_pInterface->SetOccasionalPing(true);

		m_pHostPeer = CreatePeer();
		m_pLocalPeer = m_pHostPeer;

		m_pLocalPeer->m_bHost = true;
		m_pLocalPeer->m_bLocal = true;
		m_pLocalPeer->m_iID = 0;

		if (m_xCallbacks.m_fpNetworkStarted)
			m_xCallbacks.m_fpNetworkStarted();
	}
}

// =============================================================================
// Nat Ryall                                                         08-Jun-2008
// =============================================================================
void CNetwork::StartClient(const xchar* pHostAddress, xint iHostPort, void* pData, xint iDataSize)
{
	XASSERT(!m_pInterface);

	if (!m_pInterface)
	{
		XLOG("[Network] Starting network as a client.");

		m_bHosting = false;
		m_pInterface = RakNetworkFactory::GetRakPeerInterface();

		m_xSocket.hostAddress[0] = NULL;
		m_xSocket.port = 0;

		m_pInterface->Startup(1, 0, &m_xSocket, 1);
		m_pInterface->Connect(pHostAddress, iHostPort, NULL, 0, 0);

		m_pHostPeer = NULL;
		m_pLocalPeer = NULL;

		if (m_xCallbacks.m_fpNetworkStarted)
			m_xCallbacks.m_fpNetworkStarted();
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
		XLOG("[Network] Stopping network.");

		if (m_xCallbacks.m_fpNetworkStopped)
			m_xCallbacks.m_fpNetworkStopped();

		m_pInterface->Shutdown(1);

		RakNetworkFactory::DestroyRakPeerInterface(m_pInterface);
		m_pInterface = NULL;

		Reset();
	}
}

// =============================================================================
// Nat Ryall                                                         13-Jun-2008
// =============================================================================
void CNetwork::DisconnectPeer(CNetworkPeer* pPeer)
{
	XMASSERT(false, "DisconnectPeer() is not implemented yet.");
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
					xOutStream.Write((xuint16)(*ppPeer)->m_iID);

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
	case ID_CONNECTION_LOST:
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

			RequestStop();
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

//##############################################################################