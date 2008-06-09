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

// Other.
#include <RakNetworkFactory.h>
#include <RakPeerInterface.h>

//##############################################################################

//##############################################################################
//
//                                   MACROS
//
//##############################################################################

// Shortcuts.
#define Network CNetwork::Get()

//##############################################################################

//##############################################################################
//
//                                   TYPES
//
//##############################################################################

// Predeclare.
class CNetworkPeer;

// The custom RakNet message identifiers.
enum
{
	ID_DATA_PACKET = ID_USER_PACKET_ENUM,
	ID_PEER_JOINED,
	ID_PEER_LEAVING,
};

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
	xfunction(0)<> m_fpConnectionEstablished;
	xfunction(0)<> m_fpConnectionLost;
	xfunction(1)<CNetworkPeer* /*Peer*/> m_fpPeerJoined;
	xfunction(1)<CNetworkPeer* /*Peer*/> m_fpPeerLeaving;
	xfunction(2)<CNetworkPeer* /*Peer*/, BitStream* /*Stream*/> m_fpPacketReceived;
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

	// Initialise the network system as a host.
	void StartHost();

	// Initialise the network system as a client.
	void StartClient();

	// Deinitialise the network system.
	void Stop();

	// Update the network system.
	void Update();

	// Send a data packet to a remote client.
	//void Send();

	// Check if the system is running.
	inline xbool IsRunning()
	{
		return m_pInterface != NULL;
	}

	// The network callbacks.
	CNetworkCallbacks m_xCallbacks;

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

protected:
	// Reset the system variables.
	void Reset();

	// Process all host notifications.
	void ProcessHostNotifications(xchar cIdentifier, Packet* pPacket, xchar* pData, xint iDataSize);

	// Process all client notifications.
	void ProcessClientNotifications(xchar cIdentifier, Packet* pPacket, xchar* pData, xint iDataSize);

	// Create a new peer object.
	CNetworkPeer* CreatePeer();

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

	// The local socket descriptor.
	SocketDescriptor m_xSocket;

	// The last used peer ID.
	xint m_iLastPeerID;
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
	// Specifies if the peer is local to the machine.
	xbool m_bLocal;

	// The unique network ID.
	xint m_iID;

	// The internal system address managed by RakNet.
	SystemAddress m_xAddress;

	// The network peer custom data.
	void* m_pData;
};

//##############################################################################