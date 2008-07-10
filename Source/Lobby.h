#pragma once

/**
* @file Lobby.h
* @author Nat Ryall
* @date 22/05/2008
* @brief The online lobby screen implementation.
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

// Other.
#include <Component.h>
#include <Network.h>
#include <Match.h>
#include <Menu.h>

//##############################################################################

//##############################################################################
//
//                                   TYPES
//
//##############################################################################

// Predeclare.
class CStatusBox;
class CJoinInterface;

// The lobby state.
enum t_LobbyState
{
	LobbyState_None,
	LobbyState_Searching,
	LobbyState_List,
	LobbyState_Join,
	LobbyState_Creating,
	LobbyState_Connecting,
	LobbyState_Joining,
	LobbyState_Lobby,
	LobbyState_Leaving,
	LobbyState_Closing,
};

// The message display mode.
enum t_MessageDisplayMode
{
	MessageDisplayMode_None,
	MessageDisplayMode_Error,
	MessageDisplayMode_Confirm,
};

//##############################################################################

//##############################################################################
//
//                             NETWORK PLAYER INFO
//
//##############################################################################
class CNetworkPlayerInfo
{
public:
	// The player's nickname/gamertag with a terminating NULL.
	char cNickname[_MAXNICKLEN];
};

//##############################################################################

//##############################################################################
//
//                                LOBBY SCREEN
//
//##############################################################################
class CLobbyScreen : public CScreen
{
public:
	// Initialise.
	CLobbyScreen();

	// Deinitialise
	virtual ~CLobbyScreen();

	// Called when the screen is first added to the screen stack.
	virtual void Load();

	// Called when the screen is removed from the screen stack.
	virtual void Unload();

	// Called each frame to update the screen when active.
	virtual void Update();

	// Called each frame to render the screen when active.
	virtual void Render();

	// Start and initialise the lobby using the specified mode.
	void Start(t_LobbyStartMode iStartMode);

	// Stop and close the lobby.
	void Stop();

	// Check for an excape keypress and handle accordingly.
	void QuitCheck();

protected:
	// Set the internal state.
	void SetState(t_LobbyState iState);

	// Render the session list.
	void RenderList();

	// Create a new lobby and act as the host.
	void CreateLobby();

	// Join an existing lobby and act as a client.
	void JoinLobby(const xchar* pHostAddress);

	// Update the main lobby screen.
	void UpdateLobby();

	// Render the main lobby screen.
	void RenderLobby();

	// Close the lobby down and disconnect from the network.
	void CloseLobby();

	// Callback for when the listing of available sessions has completed.
	void OnListSessionsCompleted(t_MatchResultError iError, xint iSessionCount, CSession* pSessions);

	// Callback for when the session has created.
	void OnCreateSessionCompleted(t_MatchResultError iError, CSession* pSession);

	// Callback for when the session is closed.
	void OnCloseSessionCompleted(t_MatchResultError iError, CSession* pSession);

	// Callback for when the join button is clicked in the join interface.
	void OnJoinClicked(CButtonComponent* pButton, xpoint xOffset);

	// Callback for when the network is started.
	//void OnNetworkStart();

	// Callback for when the network is stopped.
	//void OnNetworkStop();

	// Callback for when the network has successfully started.
	void OnConnectionCompleted(xbool bSuccess);

	// Callback for when the network connection has been lost.
	void OnConnectionLost();

	// Callback for when a remote peer joins the game.
	void OnPeerJoined(CNetworkPeer* pPeer);

	// Callback for when a remote peer is just about to leave the game.
	void OnPeerLeaving(CNetworkPeer* pPeer);

	// Bind all the packet callbacks to the network system.
	void BindPacketCallbacks();

	// Callback for when a lobby packet is received.
	void OnReceivePlayerInfo(CNetworkPeer* pFrom, BitStream* pStream);

	// The lobby startup mode.
	t_LobbyStartMode m_iStartMode;

	// The current lobby state.
	t_LobbyState m_iState;

	// Specifies if the lobby is public or private.
	xbool m_bPublic;

	// The local active session.
	CSession* m_pSession;

	// The join interface.
	CJoinInterface* m_pJoinInterface;

	// The status box.
	CStatusBox* m_pStatusBox;

	// The lobby peer font.
	CFont* m_pPeerFont;

	// The session list count.
	xint m_iSessionCount;

	// The session list.
	CSession* m_pSessionList;
};

//##############################################################################

//##############################################################################
//
//                               STATUS SCREEN
//
//##############################################################################
class CStatusBox
{
public:
	// Friends.
	friend class CLobbyScreen;

	// Constructor.
	CStatusBox();

	// Destructor.
	virtual ~CStatusBox();

	// Attach all elements to the interface.
	inline void AttachElements()
	{
		Interface.GetScreen()->Attach(m_pStatusBox);
		Interface.GetScreen()->Attach(m_pLabel);
	}

protected:
	// The status screen display box.
	CProgressComponent* m_pStatusBox;

	// The status text.
	CLabelComponent* m_pLabel;
};

//##############################################################################

//##############################################################################
//
//                             MESSAGE INTERFACE
//
//##############################################################################
class CMessageInterface
{
public:
	// Friends.
	friend class CLobbyScreen;

	// Constructor.
	CMessageInterface();

	// Destructor.
	virtual ~CMessageInterface();

	// Attach all elements to the interface.
	inline void AttachElements()
	{
		Interface.GetScreen()->Attach(m_pAcceptLink);
		Interface.GetScreen()->Attach(m_pCancelLink);
	}

	// Render the message interface.
	void Render();

protected:
	// The display mode.
	t_MessageDisplayMode m_iMode;

	// The "accept" text link.
	CTextLink* m_pAcceptLink;

	// The "cancel" text link.
	CTextLink* m_pCancelLink;
};

//##############################################################################

//##############################################################################
//
//                               JOIN INTERFACE
//
//##############################################################################
class CJoinInterface
{
public:
	// Friends.
	friend class CLobbyScreen;

	// Constructor.
	CJoinInterface();

	// Destructor.
	virtual ~CJoinInterface();

	// Attach all elements to the interface.
	inline void AttachElements()
	{
		Interface.GetScreen()->Attach(m_pAddressBox);
		Interface.GetScreen()->Attach(m_pJoinButton);
	}

protected:
	// The join screen address box.
	CInputComponent* m_pAddressBox;

	// The join screen join button.
	CButtonComponent* m_pJoinButton;
};

//##############################################################################