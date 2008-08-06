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
		m_lpPeers.clear();
		m_lpVerifiedPeers.clear();

		m_bHosting = false;
		m_bConnected = false;
		m_bVerified = false;

		m_iLastPeerID = 0;

		m_pLocalPeer = NULL;
		m_pHostPeer = NULL;

		m_xCallbacks.m_fpNetworkStarted = NULL;
		m_xCallbacks.m_fpNetworkStopped = NULL;
		m_xCallbacks.m_fpConnectionCompleted = NULL;
		m_xCallbacks.m_fpConnectionLost = NULL;
		m_xCallbacks.m_fpVerifyPeer = NULL;
		m_xCallbacks.m_fpVerificationCompleted = NULL;
		m_xCallbacks.m_fpPeerJoined = NULL;
		m_xCallbacks.m_fpPeerLeaving = NULL;

		m_bStopPending = false;

		for (xint iA = 0; iA < 256; ++iA)
			m_fpReceiveCallbacks[iA] = NULL;

		m_pGamerCard = NULL;
		m_iGamerCardSize = 0;

		m_pVerificationInfo = NULL;
		m_iVerificationInfoSize = 0;
	}
}

// =============================================================================
// Nat Ryall                                                         04-Aug-2008
// =============================================================================
void CNetwork::OnInitialise()
{
	Reset();
}

// =============================================================================
// Nat Ryall                                                         08-Jun-2008
// =============================================================================
void CNetwork::OnUpdate()
{
	if (m_bStopPending)
		Stop();

	if (m_pInterface)
	{
		while (Packet* pPacket = m_pInterface->Receive())
		{
			xchar cIdentifier = pPacket->data[0];

			xuchar* pData = &pPacket->data[1];
			xint iDataSize = pPacket->length - 1;

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
				"ID_REPLICA_MANAGER_DOWNLOAD_STARTED",
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
				"ID_NAT_IN_PROGRESS",
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
				"ID_AUTO_RPC_CALL",
				"ID_AUTO_RPC_REMOTE_INDEX",
				"ID_AUTO_RPC_UNKNOWN_REMOTE_INDEX",
				"ID_RPC_REMOTE_ERROR",
				"ID_STREAM",
				"ID_ROUTED_STREAM",
				"ID_VERIFICATION_REQUEST",
				"ID_VERIFICATION_SUCCEEDED",
				"ID_PEER_JOINED",
				"ID_PEER_LEAVING",
			};

			XLOG("[Network] Packet: %d (%s), %d", cIdentifier, s_pNetworkID[cIdentifier], iDataSize);

			if (m_bHosting)
				OnProcessHostNotification(cIdentifier, pPacket, pData, iDataSize);
			else
				OnProcessClientNotification(cIdentifier, pPacket, pData, iDataSize);

			m_pInterface->DeallocatePacket(pPacket);

			if (m_bStopPending)
				return;
		}
	}
}

// =============================================================================
// Nat Ryall                                                         18-Jun-2008
// =============================================================================
void CNetwork::BindReceiveCallback(xuchar cType, t_fpStreamReceived fpCallback)
{
	XMASSERT(cType < 256, "The type index cannot exceed 256.");

	if (cType < 256)
		m_fpReceiveCallbacks[cType] = fpCallback;
}

// =============================================================================
// Nat Ryall                                                         18-Jun-2008
// =============================================================================
void CNetwork::UnbindReceiveCallback(xuchar cType)
{
	BindReceiveCallback(cType, NULL);
}

// =============================================================================
// Nat Ryall                                                         18-Jun-2008
// =============================================================================
xbool CNetwork::Send(CNetworkPeer* pTo, xint iStreamType, BitStream* pStream, PacketPriority iPriority, PacketReliability iReliability, xchar iChannel)
{
	XMASSERT(iChannel >= 2 && iChannel <= 31, "Channel index out of bounds.");
	XMASSERT(m_pLocalPeer && m_pLocalPeer->m_bVerified, "The local peer is invalid or not yet initialised.");

	// Initialise tracking vars.
	BitStream* pFinalStream = NULL;
	xbool bSuccess = false;

	// If we are the host.
	if (m_bHosting)
	{
		XMASSERT(pTo, "You must specify a recepient when sending a packet from the host.");

		pFinalStream = CreateStream(iStreamType, m_pLocalPeer->m_iID, pStream);
		bSuccess = m_pInterface->Send(pFinalStream, iPriority, iReliability, iChannel, pTo->m_xAddress, false);
	}
	// If we are the client.
	else
	{
		XMASSERT(m_pHostPeer, "Cannot send from the client until the host peer is validated.");

		if (!pTo || pTo == m_pHostPeer)
			pFinalStream = CreateStream(iStreamType, m_pLocalPeer->m_iID, pStream);
		else
			pFinalStream = CreateRoutedStream(iStreamType, m_pLocalPeer->m_iID, pTo->m_iID, iPriority, iReliability, iChannel, false, pStream);
		
		bSuccess = m_pInterface->Send(pFinalStream, iPriority, iReliability, iChannel, m_pHostPeer->m_xAddress, false);
	}

	// If a stream exists, free it now.
	if (pFinalStream)
		delete pFinalStream;

	return bSuccess;
}

// =============================================================================
// Nat Ryall                                                         18-Jun-2008
// =============================================================================
xbool CNetwork::Broadcast(CNetworkPeer* pIgnore, xint iStreamType, BitStream* pStream, PacketPriority iPriority, PacketReliability iReliability, xchar iChannel)
{
	XMASSERT(iChannel >= 2 && iChannel <= 31, "Channel index out of bounds.");
	XMASSERT(m_pLocalPeer && m_pLocalPeer->m_bVerified, "The local peer is invalid or not yet initialised.");

	// Initialise tracking vars.
	BitStream* pFinalStream = NULL;
	xbool bSuccess = false;

	// If we are the host.
	if (m_bHosting)
	{
		pFinalStream = CreateStream(iStreamType, m_pLocalPeer->m_iID, pStream);
		bSuccess = m_pInterface->Send(pFinalStream, iPriority, iReliability, iChannel, pIgnore ? pIgnore->m_xAddress : UNASSIGNED_SYSTEM_ADDRESS, true);
	}
	// If we are the client.
	else
	{
		XMASSERT(m_pHostPeer, "Cannot send from the client until the host peer is validated.");

		pFinalStream = CreateRoutedStream(iStreamType, m_pLocalPeer->m_iID, pIgnore ? pIgnore->m_iID : NETWORK_PEER_INVALID_ID, iPriority, iReliability, iChannel, true, pStream);
		bSuccess = m_pInterface->Send(pFinalStream, iPriority, iReliability, iChannel, m_pHostPeer->m_xAddress, false);
	}

	// If a stream exists, free it now.
	if (pFinalStream)
		delete pFinalStream;

	return bSuccess;
}

// =============================================================================
// Nat Ryall                                                         05-Aug-2008
// =============================================================================
BitStream* CNetwork::CreateStream(xint iStreamType, xint iFrom, BitStream* pStream)
{
	BitStream* pFinalStream = new BitStream();

	pFinalStream->Write((xuint8)ID_STREAM);
	pFinalStream->Write((xuint8)iStreamType);
	pFinalStream->Write((xuint8)iFrom);

	if (pStream)
		pFinalStream->Write(pStream);

	return pFinalStream;
}

// =============================================================================
// Nat Ryall                                                         05-Aug-2008
// =============================================================================
BitStream* CNetwork::CreateRoutedStream(xint iStreamType, xint iFrom, xint iTo, xint iPriority, xint iReliability, xint iChannel, xbool bBroadcast, BitStream* pStream)
{
	BitStream* pFinalStream = new BitStream();

	pFinalStream->Write((xuint8)ID_ROUTED_STREAM);
	pFinalStream->Write((xuint8)iStreamType);
	pFinalStream->Write((xuint8)iFrom);
	pFinalStream->Write((xuint8)iTo);
	pFinalStream->Write((xuint8)iPriority);
	pFinalStream->Write((xuint8)iReliability);
	pFinalStream->Write((xuint8)iChannel);
	pFinalStream->Write((xuint8)bBroadcast);

	if (pStream)
		pFinalStream->Write(pStream);

	return pFinalStream;
}

// =============================================================================
// Nat Ryall                                                         08-Jun-2008
// =============================================================================
void CNetwork::StartHost(xint iMaxPeers, xint iPort, void* pData, xint iDataSize)
{
	XASSERT(!m_pInterface);
	XASSERT(iMaxPeers < NETWORK_PEER_INVALID_ID);

	if (!m_pInterface)
	{
		XLOG("[Network] Starting network as a host.");

		m_bHosting = true;
		m_bVerified = true;

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
		m_pLocalPeer->m_bVerified = true;

		if (m_iGamerCardSize)
		{
			m_pLocalPeer->m_pGamerCard = new xchar[m_iGamerCardSize];				
			memcpy_s(m_pLocalPeer->m_pGamerCard, m_iGamerCardSize, m_pGamerCard, m_iGamerCardSize);
		}

		m_lpVerifiedPeers.push_back(m_pLocalPeer);

		if (m_xCallbacks.m_fpNetworkStarted)
			m_xCallbacks.m_fpNetworkStarted();

		if (m_xCallbacks.m_fpPeerJoined)
			m_xCallbacks.m_fpPeerJoined(m_pLocalPeer);
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

		DestroyPeers();

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
void CNetwork::DisconnectPeer(SystemAddress& xAddress)
{
	if (m_pInterface)
		m_pInterface->CloseConnection(xAddress, true, 1);
}

// =============================================================================
// Nat Ryall                                                         18-Jul-2008
// =============================================================================
xint CNetwork::GetLastPing()
{
	if (m_bHosting || !m_pHostPeer)
		return -1;
	else
		return m_pInterface->GetLastPing(m_pHostPeer->m_xAddress);
}

// =============================================================================
// Nat Ryall                                                         13-Jun-2008
// =============================================================================
xint CNetwork::GetUniquePeerID()
{
	while (FindPeer(m_iLastPeerID) != NULL) 
	{
		m_iLastPeerID++;
		m_iLastPeerID %= NETWORK_PEER_INVALID_ID - 1;
	} 

	return m_iLastPeerID;
}

// =============================================================================
// Nat Ryall                                                         09-Jun-2008
// =============================================================================
CNetworkPeer* CNetwork::CreatePeer()
{
	CNetworkPeer* pPeer = new CNetworkPeer();

	pPeer->m_bHost = false;
	pPeer->m_bLocal = false;
	pPeer->m_iID = (m_bHosting) ? GetUniquePeerID() : NETWORK_PEER_INVALID_ID;
	pPeer->m_bVerified = false;
	pPeer->m_pGamerCard = NULL;

	pPeer->m_xAddress.binaryAddress = 0;
	pPeer->m_xAddress.port = 0;

	m_lpPeers.push_back(pPeer);

	XLOG("[Network] Created a new peer.");

	return pPeer;
}

// =============================================================================
// Nat Ryall                                                         19-Jul-2008
// =============================================================================
void CNetwork::DestroyPeer(CNetworkPeer* pPeer)
{
	if (pPeer)
	{	
		XLOG("[Network] Destroying peer %d.", pPeer->m_iID);

		if (pPeer->m_pGamerCard)
		{
			delete pPeer->m_pGamerCard;
			pPeer->m_pGamerCard = NULL;
		}

		XEN_LIST_REMOVE(t_NetworkPeerList, m_lpPeers, pPeer);
		XEN_LIST_REMOVE(t_NetworkPeerList, m_lpVerifiedPeers, pPeer);
	
		delete pPeer;
	}
}

// =============================================================================
// Nat Ryall                                                         19-Jul-2008
// =============================================================================
void CNetwork::DestroyPeers()
{
	while (m_lpPeers.size()) 
		DestroyPeer(m_lpPeers.front());
}

// =============================================================================
// Nat Ryall                                                         30-Jul-2008
// =============================================================================
void CNetwork::SortPeers()
{
	m_lpPeers.sort(&OnComparePeers);
	m_lpVerifiedPeers.sort(&OnComparePeers);
}

// =============================================================================
// Nat Ryall                                                         31-Jul-2008
// =============================================================================
xbool CNetwork::OnComparePeers(const CNetworkPeer* pA, const CNetworkPeer* pB)
{
	return pA->m_iID < pB->m_iID;
}

// =============================================================================
// Nat Ryall                                                         09-Jun-2008
// =============================================================================
CNetworkPeer* CNetwork::FindPeer(SystemAddress& xAddress)
{
	XEN_LIST_FOREACH(t_NetworkPeerList, ppPeer, m_lpPeers)
	{
		if ((*ppPeer)->m_xAddress == xAddress)
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
void CNetwork::OnProcessHostNotification(xchar cIdentifier, Packet* pPacket, xuchar* pData, xint iDataSize)
{
	BitStream xInStream(pData, iDataSize, false);

	switch (cIdentifier)
	{
	// A new client connection was established to this machine.
	case ID_NEW_INCOMING_CONNECTION:
		{
			CNetworkPeer* pPeer = CreatePeer();
			pPeer->m_xAddress = pPacket->systemAddress;
		}
		break;

	// A client is requesting verification.
	case ID_VERIFICATION_REQUEST:
		{
			// Find the peer we're working with.
			CNetworkPeer* pPeer = FindPeer(pPacket->systemAddress);

			if (pPeer)
			{
				// Read the data sizes before doing anything else.
				xuint16 iGamerCardSize = 0;
				xuint16 iVerificationInfoSize = 0;

				xInStream.Read(iGamerCardSize);
				xInStream.Read(iVerificationInfoSize);

				// Read the gamercard data for this peer.
				if (iGamerCardSize)
				{
					pPeer->m_pGamerCard = new xchar[iGamerCardSize];				
					xInStream.Read((xchar*)pPeer->m_pGamerCard, iGamerCardSize);
				}

				// Read the verification information for this peer and execute the verification routine.
				void* pVerificationInfo = NULL;

				if (iVerificationInfoSize)
				{
					pVerificationInfo = new xchar[iVerificationInfoSize];				
					xInStream.Read((xchar*)pVerificationInfo, iVerificationInfoSize);
				}

				if (m_xCallbacks.m_fpVerifyPeer)
					pPeer->m_bVerified = m_xCallbacks.m_fpVerifyPeer(pPeer, pVerificationInfo, iVerificationInfoSize);
				else
					pPeer->m_bVerified = true;

				delete pVerificationInfo;

				// Process based on our verification result.
				if (pPeer->m_bVerified)
				{
					// Notify that we have successfully been verified.
					{
						BitStream xOutStream;
						xOutStream.Write((xuint8)ID_VERIFICATION_SUCCEEDED);

						m_pInterface->Send(&xOutStream, HIGH_PRIORITY, RELIABLE, 1, pPacket->systemAddress, false);
					}

					// Notify our new client about all existing verified clients. 
					XEN_LIST_FOREACH_R(t_NetworkPeerList, ppPeer, m_lpPeers)
					{
						if ((*ppPeer)->m_bVerified)
						{
							BitStream xOutStream;

							xOutStream.Write((xuint8)ID_PEER_JOINED);
							xOutStream.Write((xbool)(pPeer->m_iID == (*ppPeer)->m_iID));
							xOutStream.Write((xuint16)(*ppPeer)->m_iID);
							xOutStream.Write((xuint16)iGamerCardSize);

							if (iGamerCardSize)
								xOutStream.Write((xchar*)(*ppPeer)->m_pGamerCard, iGamerCardSize);

							m_pInterface->Send(&xOutStream, HIGH_PRIORITY, RELIABLE, 1, pPacket->systemAddress, false);
						}
					}

					// Notify all existing verified clients about the new client.
					XEN_LIST_FOREACH(t_NetworkPeerList, ppPeer, m_lpPeers)
					{
						if (!(*ppPeer)->m_bLocal && *ppPeer != pPeer && (*ppPeer)->m_bVerified)
						{
							BitStream xOutStream;

							xOutStream.Write((xuint8)ID_PEER_JOINED);
							xOutStream.Write(false);
							xOutStream.Write((xuint16)pPeer->m_iID);
							xOutStream.Write((xuint16)iGamerCardSize);

							if (iGamerCardSize)
								xOutStream.Write((xchar*)pPeer->m_pGamerCard, iGamerCardSize);

							m_pInterface->Send(&xOutStream, HIGH_PRIORITY, RELIABLE, 1, (*ppPeer)->m_xAddress, false);
						}
					}

					// Add the peer to the verified list.
					m_lpVerifiedPeers.push_back(pPeer);

					// Fire the join notification.
					if (m_xCallbacks.m_fpPeerJoined)
						m_xCallbacks.m_fpPeerJoined(pPeer);
				}
				else
					DisconnectPeer(pPacket->systemAddress);
			}
			else
				DisconnectPeer(pPacket->systemAddress);
		}
		break;

	// A client has lost connection to us.
	case ID_DISCONNECTION_NOTIFICATION:
	case ID_CONNECTION_LOST:
		{
			// If we have a record of them, notify all other clients of their disconnection.
			CNetworkPeer* pPeer = FindPeer(pPacket->systemAddress);
			
			if (pPeer)
			{
				// Notify all other peers of the disconnection.
				BitStream xOutStream;

				xOutStream.Write((xuint8)ID_PEER_LEAVING);
				xOutStream.Write((xuint16)pPeer->m_iID);

				m_pInterface->Send(&xOutStream, HIGH_PRIORITY, RELIABLE, 1, UNASSIGNED_SYSTEM_ADDRESS, true);
			}

			// Fire the leaving notification.
			if (m_xCallbacks.m_fpPeerLeaving)
				m_xCallbacks.m_fpPeerLeaving(pPeer);

			DestroyPeer(pPeer);
		}
		break;

	// Received a data packet from a client.
	case ID_STREAM:
		{
			OnProcessPacket(pPacket, &xInStream);
		}
		break;

	// Received a data packet from a client to broadcast to other peers.
	case ID_ROUTED_STREAM:
		{
			xuint8 iStreamType;
			xuint8 iFrom;
			xuint8 iTo;
			xuint8 iPriority;
			xuint8 iReliability;
			xuint8 iChannel;
			xuint8 bBroadcast;

			xInStream.Read(iStreamType);
			xInStream.Read(iFrom);
			xInStream.Read(iTo);
			xInStream.Read(iPriority);
			xInStream.Read(iReliability);
			xInStream.Read(iChannel);
			xInStream.Read(bBroadcast);

			BitStream xAppendStream;

			if (xInStream.GetNumberOfUnreadBits())
				xInStream.Read(xAppendStream);

			// Check if the host should process this packet.
			if (bBroadcast && iTo != m_pLocalPeer->m_iID)
			{
				BitStream xLocalStream;

				xLocalStream.Write(iStreamType);
				xLocalStream.Write(iFrom);

				if (xAppendStream.GetNumberOfBitsUsed())
				{
					xLocalStream.Write(xAppendStream);
					xAppendStream.SetReadOffset(0);
				}

				OnProcessPacket(pPacket, &xLocalStream);
			}

			// Forward or broadcast the stream.
			CNetworkPeer* pToPeer = FindPeer(iTo);

			BitStream* pFinalStream = NULL;
			BitStream* pAppendStream = xAppendStream.GetNumberOfBitsUsed() ? &xAppendStream : NULL; 

			if (bBroadcast)
			{
				pFinalStream = CreateStream(iStreamType, iFrom, pAppendStream);

				XEN_LIST_FOREACH(t_NetworkPeerList, ppPeer, m_lpVerifiedPeers)
				{
					CNetworkPeer* pWorkingPeer = *ppPeer;

					if (pWorkingPeer != pToPeer && pWorkingPeer->m_iID != iFrom && !pWorkingPeer->m_bLocal)
					{
						XLOG("[Network] Forwarding broadcasted packet from peer %d to peer %d", iFrom, pWorkingPeer->m_iID);
						m_pInterface->Send(pFinalStream, (PacketPriority)iPriority, (PacketReliability)iReliability, iChannel, pWorkingPeer->m_xAddress, false);
					}
				}
			}
			else
			{
				if (pToPeer)
				{
					pFinalStream = CreateStream(iStreamType, iFrom, pAppendStream);

					XLOG("[Network] Forwarding packet from peer %d to peer %d", iFrom, pToPeer->m_iID);
					m_pInterface->Send(pFinalStream, (PacketPriority)iPriority, (PacketReliability)iReliability, iChannel, pToPeer->m_xAddress, false);
				}
			}

			if (pFinalStream)
				delete pFinalStream;
		}
		break;
	}
}

// =============================================================================
// Nat Ryall                                                         09-Jun-2008
// =============================================================================
void CNetwork::OnProcessClientNotification(xchar cIdentifier, Packet* pPacket, xuchar* pData, xint iDataSize)
{
	BitStream xInStream(pData, iDataSize, false);

	switch (cIdentifier)
	{
	// Client successfully connected to the host.
	case ID_CONNECTION_REQUEST_ACCEPTED:
		{
			m_bConnected = true;
			m_bVerified = false;

			// We're connected but not verified yet. Execute early so that verification data can be set in the callback.
			if (m_xCallbacks.m_fpConnectionCompleted)
				m_xCallbacks.m_fpConnectionCompleted(true);

			// Send out our gamer card and verification info to the host.
			BitStream xOutStream;

			xOutStream.Write((xuint8)ID_VERIFICATION_REQUEST);

			xOutStream.Write((xuint16)m_iGamerCardSize);
			xOutStream.Write((xuint16)m_iVerificationInfoSize);

			if (m_iGamerCardSize)
				xOutStream.Write((xchar*)m_pGamerCard, m_iGamerCardSize);

			if (m_iVerificationInfoSize)
				xOutStream.Write((xchar*)m_pVerificationInfo, m_iVerificationInfoSize);

			m_pInterface->Send(&xOutStream, HIGH_PRIORITY, RELIABLE, 1, pPacket->systemAddress, false);
		}
		break;

	// Client could not connect to the host.
	case ID_CONNECTION_ATTEMPT_FAILED:
		{
			m_bConnected = false;
			m_bVerified = false;

			if (m_xCallbacks.m_fpConnectionCompleted)
				m_xCallbacks.m_fpConnectionCompleted(false);
		}
		break;

	// The verification process has completed and was a success.
	case ID_VERIFICATION_SUCCEEDED:
		{
			m_bVerified = true;

			if (m_xCallbacks.m_fpVerificationCompleted)
				m_xCallbacks.m_fpVerificationCompleted(true);
		}
		break;

	// The connection to the host has been lost.
	case ID_DISCONNECTION_NOTIFICATION:
	case ID_CONNECTION_LOST:
		{
			// Remove all existing peers and execute the leaving callbacks.
			DestroyPeers();

			// If we were verifying, we should execute the verification completed callback with a failure.
			if (m_bConnected && !m_bVerified)
			{
				if (m_xCallbacks.m_fpVerificationCompleted)
					m_xCallbacks.m_fpVerificationCompleted(false);
			}

			// We're no longer connected so notify and shutdown.
			m_bConnected = false;
			m_bVerified = false;

			if (m_xCallbacks.m_fpConnectionLost)
				m_xCallbacks.m_fpConnectionLost();

			RequestStop();
		}
		break;

	// A new peer has been added to the game.
	case ID_PEER_JOINED:
		{
			// Create the new peer structure.
			CNetworkPeer* pPeer = CreatePeer();

			// Read the stream header information.
			xbool bLocal = false;
			xuint16 iID = 0;
			xuint16 iGamerCardSize = 0;

			xInStream.Read(bLocal);
			xInStream.Read(iID);
			xInStream.Read(iGamerCardSize);

			// Read the gamercard data for this peer.
			if (iGamerCardSize)
			{
				pPeer->m_pGamerCard = new xchar[iGamerCardSize];				
				xInStream.Read((xchar*)pPeer->m_pGamerCard, iGamerCardSize);
			}

			// Initalise the peer.
			pPeer->m_bHost = (iID == 0);
			pPeer->m_bLocal = bLocal;
			pPeer->m_iID = (xint)iID;
			pPeer->m_bVerified = true;

			if (pPeer->m_bHost)
			{
				m_pHostPeer = pPeer;
				m_pHostPeer->m_xAddress = pPacket->systemAddress;
			}

			if (bLocal)
				m_pLocalPeer = pPeer;

			// Add the peer to the verified list.
			m_lpVerifiedPeers.push_back(pPeer);

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

			DestroyPeer(pPeer);
		}
		break;

	// Received a data packet from the host.
	case ID_STREAM:
		{
			OnProcessPacket(pPacket, &xInStream);
		}
		break;
	}
}

// =============================================================================
// Nat Ryall                                                         13-Jun-2008
// =============================================================================
void CNetwork::OnProcessPacket(Packet* pPacket, BitStream* pStream)
{
	xuint8 iStreamType;
	xuint8 iFrom;

	pStream->Read(iStreamType);
	pStream->Read(iFrom);

	CNetworkPeer* pPeer = FindPeer((xint)iFrom);

	XLOG("[Network] Received a packet from peer %d.", iFrom);

	if (pPeer)
	{
		if (m_fpReceiveCallbacks[iStreamType])
			m_fpReceiveCallbacks[iStreamType](pPeer, pStream);
	}
}

//##############################################################################