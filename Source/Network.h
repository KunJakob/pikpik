#pragma once

/** 
	INSERT HEADER 
**/

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

// Shortcuts.
#define Network CNetwork::Get()

// The maximum ID for network peers is invalid.
#define NETWORK_PEER_INVALID_ID 0xFFFF

//##############################################################################

//##############################################################################
//
//                                   TYPES
//
//##############################################################################

// Namespaces.
using namespace RakNet;

// Predeclare.
class CNetworkPeer;

// The custom RakNet message identifiers.
enum
{
	ID_DATA_PACKET = ID_USER_PACKET_ENUM,
	ID_VERIFICATION_REQUEST,
	ID_VERIFICATION_SUCCEEDED,
	ID_PEER_JOINED,
	ID_PEER_LEAVING,
};

// Callbacks.
typedef xfunction(2)<CNetworkPeer* /*From*/, BitStream* /*Stream*/> t_fpStreamReceived;

// Lists.
typedef xlist<CNetworkPeer*> t_NetworkPeerList;

//##############################################################################

//##############################################################################
//
//                                 CALLBACKS
//
//##############################################################################
class CNetworkCallbacks
{
public:
	xfunction(0)<> m_fpNetworkStarted;
	xfunction(0)<> m_fpNetworkStopped;
	xfunction(1)<xbool /*Succeeded*/> m_fpConnectionCompleted;
	xfunction(0)<> m_fpConnectionLost;
	xfunction(3)<CNetworkPeer* /*Peer*/, void* /*Data*/, xint /*DataLength*/, xbool /*_RETURN_*/> m_fpVerifyPeer;
	xfunction(1)<xbool /*Succeeded*/> m_fpVerificationCompleted;
	xfunction(1)<CNetworkPeer* /*Peer*/> m_fpPeerJoined;
	xfunction(1)<CNetworkPeer* /*Peer*/> m_fpPeerLeaving;
};

//##############################################################################

//##############################################################################
//
//                                   NETWORK
//
//##############################################################################
class CNetwork : public CModule
{
public:
	// Singleton instance.
	static inline CNetwork& Get() 
	{
		static CNetwork s_Instance;
		return s_Instance;
	}

	// Constructor.
	CNetwork();

	// Reset the system variables.
	void Reset();

	// Initialise the network system.
	virtual void OnInitialise();

	// Update the network system.
	virtual void OnUpdate();

	// Bind a callback function to a specific packet type.
	void BindReceiveCallback(xuchar cType, t_fpStreamReceived fpCallback);

	// Unbind a callback function from a specific packet type.
	void UnbindReceiveCallback(xuchar cType);

	// Send a data packet to a remote peer. "pTo" is the peer to send to when sending from the host and is ignored otherwise.
	xbool Send(CNetworkPeer* pTo, xuchar cType, BitStream* pStream, PacketPriority iPriority, PacketReliability iReliability, xchar iChannel = 2);

	// Send a data packet to all remote peers (via the host if we are a client).
	xbool Broadcast(CNetworkPeer* pIgnore, xuchar cType, BitStream* pStream, PacketPriority iPriority, PacketReliability iReliability, xchar iChannel = 2);

	// Initialise the network system as a host.
	void StartHost(xint iMaxPeers, xint iPort, void* pCustomInfo = NULL, xint iCustomInfoSize = 0);

	// Initialise the network system as a client.
	void StartClient(const xchar* pHostAddress, xint iHostPort, void* pCustomInfo = NULL, xint iCustomInfoSize = 0);

	// Deinitialise the network system.
	void Stop();

	// Request that the network system be deinitialised safely at the next available opportunity.
	inline void RequestStop()
	{
		m_bStopPending = true;
	}

	// Check if the system is running.
	inline xbool IsRunning()
	{
		return m_pInterface != NULL;
	}

	// Forcefully disconnect a peer from the local machine. Valid only on the host.
	void DisconnectPeer(SystemAddress& xAddress);

	// Get the last ping time to the host or -1 if we are the host or disconnected.
	xint GetLastPing();

	// Get a pointer to the internal raknet interface.
	inline RakPeerInterface* GetInterface() 
	{ 
		return m_pInterface; 
	}

	// Get the current peer list.
	inline t_NetworkPeerList& GetPeers() 
	{ 
		return m_lpPeers; 
	}

	// Get the current verified peer list.
	inline t_NetworkPeerList& GetVerifiedPeers() 
	{ 
		return m_lpVerifiedPeers; 
	}

	// Get the local peer or NULL if there is no local peer.
	inline CNetworkPeer* GetLocalPeer() 
	{ 
		return m_pLocalPeer; 
	}

	// Get the current host peer or NULL if not known.
	inline CNetworkPeer* GetHostPeer() 
	{ 
		return m_pHostPeer; 
	}

	// Sort the peers by network ID.
	void SortPeers();

	// Find an exisiting peer by peer ID.
	CNetworkPeer* FindPeer(xint iPeerID);

	// Determine if the local machine is the host.
	inline xbool IsHosting() 
	{ 
		return m_bHosting; 
	}

	// Determine if the local machine is connected to a host.
	inline xbool IsConnected() 
	{ 
		return m_bConnected; 
	}

	// Determine if the local machine is verified with the host.
	inline xbool IsVerified() 
	{ 
		return m_bVerified; 
	}

	// Set the data for the local gamer card to be sent to all other peers.
	inline void SetGamerCard(void* pData, xuint iDataSize) 
	{ 
		m_pGamerCard = pData;
		m_iGamerCardSize = iDataSize;
	}

	// Get the data for the local gamer card to be sent to all other peers.
	inline void* GetGamerCard() 
	{ 
		return m_pGamerCard; 
	}

	// Set the verification info to be sent to the host when connecting.
	inline void SetVerificationInfo(void* pData, xuint iDataSize) 
	{ 
		m_pVerificationInfo = pData;
		m_iVerificationInfoSize = iDataSize;
	}

	// Get the verification info for the local machine.
	inline void* GetVerificationInfo() 
	{ 
		return m_pVerificationInfo; 
	}

	// Determine if there are any peers currently being verified.
	inline xbool IsEveryoneVerified()
	{
		return m_lpPeers.size() == m_lpVerifiedPeers.size();
	}

	// The network callbacks.
	CNetworkCallbacks m_xCallbacks;

protected:
	// Comparison routine for sorting peers.
	static xbool OnComparePeers(const CNetworkPeer* pA, const CNetworkPeer* pB);

	// Relay a data packet from the host machine to all other peers on behalf of the sending peer.
	//void Relay(CNetworkPeer* pSender, BitStream* pStream);

	// Process all host notifications.
	void ProcessHostNotifications(xchar cIdentifier, Packet* pPacket, xuchar* pData, xint iDataSize);

	// Process all client notifications.
	void ProcessClientNotifications(xchar cIdentifier, Packet* pPacket, xuchar* pData, xint iDataSize);

	// Process an incoming packet and dispatch to any callbacks.
	void ProcessPacket(Packet* pPacket, BitStream* pStream);

	// Geterate a new peer ID. Valid only on the host.
	xint GetUniquePeerID();

	// Create a new peer object.
	CNetworkPeer* CreatePeer();

	// Destroy an existing peer.
	void DestroyPeer(CNetworkPeer* pPeer);

	// Find an existing peer by system address.
	CNetworkPeer* FindPeer(SystemAddress* pAddress);

	// Free all existing peers in the system and fire any leaving notifications.
	void DestroyPeers();
	
	// The local interface.
	RakPeerInterface* m_pInterface;

	// The network peer list.
	t_NetworkPeerList m_lpPeers;

	// The list of network verified peers.
	t_NetworkPeerList m_lpVerifiedPeers;

	// The local peer.
	CNetworkPeer* m_pLocalPeer;

	// The host peer.
	CNetworkPeer* m_pHostPeer;

	// Determines if the local machine is the host.
	xbool m_bHosting;

	// Determines if we are connected to a remote machine.
	xbool m_bConnected;

	// Determines if we were verified by the host.
	xbool m_bVerified;

	// The array of callback type bindings.
	t_fpStreamReceived m_fpReceiveCallbacks[256];

	// The local socket descriptor.
	SocketDescriptor m_xSocket;

	// The last used peer ID.
	xint m_iLastPeerID;

	// Determines if a stop request is pending.
	xbool m_bStopPending;

	// The local gamer card data.
	void* m_pGamerCard;

	// The size, in bytes, of the gamer card data.
	xuint m_iGamerCardSize;

	// The verification data.
	void* m_pVerificationInfo;

	// The size, in bytes, of the verification info.
	xuint m_iVerificationInfoSize;
};

//##############################################################################

//##############################################################################
//
//                                NETWORK PEER
//
//##############################################################################
class CNetworkPeer
{
public:
	// Specifies if the peer is hosting.
	xbool m_bHost;

	// Specifies if the peer is local to the machine.
	xbool m_bLocal;

	// The unique network ID.
	xint m_iID;

	// The internal system address managed by RakNet. This is not valid, client to client.
	SystemAddress m_xAddress;

	// Specifies if the peer was successfully verified by the host.
	xbool m_bVerified;

	// The network peer gamer card.
	void* m_pGamerCard;

	// The local peer data.
	void* m_pData;
};

//##############################################################################