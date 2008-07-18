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
	xfunction(1)<CNetworkPeer* /*Peer*/> m_fpPeerJoined;
	xfunction(1)<CNetworkPeer* /*Peer*/> m_fpPeerLeaving;
};

//##############################################################################

//##############################################################################
//
//                                   NETWORK
//
//##############################################################################
class CNetwork
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

	// Update the network system.
	void Update();

	// Bind a callback function to a specific packet type.
	void BindReceiveCallback(xuchar cType, t_fpStreamReceived fpCallback);

	// Unbind a callback function from a specific packet type.
	void UnbindReceiveCallback(xuchar cType);

	// Send a data packet to a remote peer. "pTo" is the peer to send to when sending from the host and is ignored otherwise.
	xbool Send(CNetworkPeer* pTo, xuchar cType, BitStream* pStream, PacketPriority iPriority, PacketReliability iReliability, xchar iChannel = 2);

	// Send a data packet to all remote peers (via the host if we are a client).
	xbool Broadcast(CNetworkPeer* pIgnore,  xuchar cType, BitStream* pStream, PacketPriority iPriority, PacketReliability iReliability, xchar iChannel = 2);

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
	void DisconnectPeer(CNetworkPeer* pPeer);

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

	// The network callbacks.
	CNetworkCallbacks m_xCallbacks;

protected:
	// Relay a data packet from the host machine to all other peers on behalf of the sending peer.
	void Relay(CNetworkPeer* pSender, BitStream* pStream);

	// Process all host notifications.
	void ProcessHostNotifications(xchar cIdentifier, Packet* pPacket, xuchar* pData, xint iDataSize);

	// Process all client notifications.
	void ProcessClientNotifications(xchar cIdentifier, Packet* pPacket, xuchar* pData, xint iDataSize);

	// Process an incoming packet and dispatch to any callbacks.
	void ProcessPacket(Packet* pPacket, BitStream* pStream);

	// Create a new peer object.
	CNetworkPeer* CreatePeer();

	// Geterate a new peer ID. Valid only on the host.
	xint GetUniquePeerID();

	// Destroy an existing peer if it exists by system address.
	void DestroyPeer(SystemAddress* pAddress);

	// Destroy a peer if it exists by peer ID.
	void DestroyPeer(xint iPeerID);

	// Find an existing peer by system address.
	CNetworkPeer* FindPeer(SystemAddress* pAddress);

	// Find an exisiting peer by peer ID.
	CNetworkPeer* FindPeer(xint iPeerID);

	// The local interface.
	RakPeerInterface* m_pInterface;

	// The network peer list.
	t_NetworkPeerList m_lpPeers;

	// The local peer.
	CNetworkPeer* m_pLocalPeer;

	// The host peer.
	CNetworkPeer* m_pHostPeer;

	// Determines if the local machine is the host.
	xbool m_bHosting;

	// Determines if we are connected to a remote machine.
	xbool m_bConnected;

	// The array of callback type bindings.
	t_fpStreamReceived m_fpReceiveCallbacks[256];

	// The local socket descriptor.
	SocketDescriptor m_xSocket;

	// The last used peer ID.
	xint m_iLastPeerID;

	// Determines if a stop request is pending.
	xbool m_bStopPending;
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

	// The network peer custom data.
	void* m_pData;
};

//##############################################################################