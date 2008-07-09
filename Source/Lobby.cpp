/**
* @file Lobby.cpp
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

// Local.
#include <Lobby.h>

//##############################################################################

//##############################################################################
//
//                                LOBBY SCREEN
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         22-May-2008
// =============================================================================
CLobbyScreen::CLobbyScreen() : CScreen(ScreenIndex_LobbyScreen),
	m_iState(LobbyState_None),
	m_pSession(NULL)
{
	_GLOBAL.pLobby = this;
}

// =============================================================================
// Nat Ryall                                                         22-May-2008
// =============================================================================
CLobbyScreen::~CLobbyScreen()
{
	_GLOBAL.pLobby = NULL;
}

// =============================================================================
// Nat Ryall                                                         09-Jun-2008
// =============================================================================
void CLobbyScreen::Load()
{
	// Load interfaces.
	m_pJoinInterface = new CJoinInterface();
	m_pStatusBox = new CStatusBox();

	m_pJoinInterface->m_pJoinButton->SetClickCallback(xbind(this, &CLobbyScreen::OnJoinClicked));

	// Load fonts.
	m_pPeerFont = new CFont(_FONT("Lobby-Peer"));

	// Initialise components.
	m_pJoinInterface->m_pAddressBox->SetText("127.0.0.1");
}

// =============================================================================
// Nat Ryall                                                         09-Jun-2008
// =============================================================================
void CLobbyScreen::Unload()
{
	delete m_pJoinInterface;
	delete m_pStatusBox;

	delete m_pPeerFont;
}

// =============================================================================
// Nat Ryall                                                         09-Jun-2008
// =============================================================================
void CLobbyScreen::Update()
{
	QuitCheck();
	UpdateParent();

	switch (m_iState)
	{
	case LobbyState_Lobby:
		UpdateLobby();
		break;
	}
}

// =============================================================================
// Nat Ryall                                                         09-Jun-2008
// =============================================================================
void CLobbyScreen::Render()
{
	RenderParent();

	switch (m_iState)
	{
	case LobbyState_List:
		RenderList();
		break;

	case LobbyState_Lobby:
		RenderLobby();
		break;
	}
}

// =============================================================================
// Nat Ryall                                                         09-Jun-2008
// =============================================================================
void CLobbyScreen::Start(t_LobbyStartMode iStartMode)
{
	switch (iStartMode)
	{
	case LobbyStartMode_Find:
		{
			Match.ListSessions(xbind(this, &CLobbyScreen::OnListSessionsCompleted));
			SetState(LobbyState_Searching);
		}
		break;

	case LobbyStartMode_Join:
		{
			SetState(LobbyState_Join);
		}
		break;
	
	case LobbyStartMode_Create:
		{
			m_pSession = Match.CreateSession(4, xbind(this, &CLobbyScreen::OnCreateSessionCompleted));
			SetState(LobbyState_Creating);
		}
		break;
	}

	m_iStartMode = iStartMode;
}

// =============================================================================
// Nat Ryall                                                         18-Jun-2008
// =============================================================================
void CLobbyScreen::Stop()
{
	Network.RequestStop();
}

// =============================================================================
// Nat Ryall                                                         15-Jun-2008
// =============================================================================
void CLobbyScreen::SetState(t_LobbyState iState)
{
	InterfaceManager.GetScreen()->DetachAll();

	switch (iState)
	{
	case LobbyState_Searching:
		{
			m_pStatusBox->AttachElements();
			m_pStatusBox->m_pLabel->SetText(_LOCALE("Status_Searching"));
		}
		break;

	case LobbyState_Join:
		{
			if (Network.IsRunning())
				Network.RequestStop();

			m_pJoinInterface->AttachElements();
		}
		break;

	case LobbyState_Creating:
		{
			m_pStatusBox->AttachElements();
			m_pStatusBox->m_pLabel->SetText(_LOCALE("Status_Creating"));
		}
		break;

	case LobbyState_Connecting:
		{
			m_pStatusBox->AttachElements();
			m_pStatusBox->m_pLabel->SetText(_LOCALE("Status_Connecting"));
		}
		break;

	case LobbyState_Joining:
		{
			m_pStatusBox->AttachElements();
			m_pStatusBox->m_pLabel->SetText(_LOCALE("Status_Joining"));
		}
		break;

	case LobbyState_Leaving:
		{
			m_pStatusBox->AttachElements();
			m_pStatusBox->m_pLabel->SetText(_LOCALE("Status_Leaving"));
		}
		break;

	case LobbyState_Closing:
		{
			m_pStatusBox->AttachElements();
			m_pStatusBox->m_pLabel->SetText(_LOCALE("Status_Closing"));
		}
		break;
	}

	m_iState = iState;
}

// =============================================================================
// Nat Ryall                                                         17-Jun-2008
// =============================================================================
void CLobbyScreen::QuitCheck()
{
	if (_HGE->Input_KeyUp(HGEK_ESCAPE))
	{
		switch (m_iState)
		{
		case LobbyState_List:
		case LobbyState_Join:
			{
				SetState(LobbyState_None);
				ScreenManager::Pop();
			}
			break;
		
		case LobbyState_Lobby:
			{
				if (Network.m_bHosting)
				{
					Match.CloseSession(m_pSession, xbind(this, &CLobbyScreen::OnCloseSessionCompleted));
					SetState(LobbyState_Closing);
				}
				else
					SetState(LobbyState_Join);

				Network.Stop();
			}
			break;
		}
	}
}

// =============================================================================
// Nat Ryall                                                         09-Jul-2008
// =============================================================================
void CLobbyScreen::RenderList()
{
	for (xint iA = 0; iA < m_iSessionCount; ++iA)
	{
		CSession* pSession = &m_pSessionList[iA];

		_GLOBAL.pGameFont->Render(pSession->m_sSessionID.c_str(), xpoint(10, 10 + (iA * 20)), HGETEXT_LEFT);
		_GLOBAL.pGameFont->Render(XFORMAT("%d/%d Slots", pSession->m_iUsedSlots, pSession->m_iTotalSlots), xpoint(350, 10 + (iA * 20)), HGETEXT_LEFT);
	}
}

// =============================================================================
// Nat Ryall                                                         17-Jun-2008
// =============================================================================
void CLobbyScreen::UpdateLobby()
{
}

// =============================================================================
// Nat Ryall                                                         17-Jun-2008
// =============================================================================
void CLobbyScreen::RenderLobby()
{
	// Render the peer list.
	xint iPeerOffset = 0;

	XEN_LIST_FOREACH(t_NetworkPeerList, ppPeer, Network.m_lpPeers)
	{
		m_pPeerFont->Render(XFORMAT("Peer_%02d", (*ppPeer)->m_iID), xpoint(50, 50 + iPeerOffset), HGETEXT_LEFT);
		iPeerOffset += 40;
	}
}

// =============================================================================
// Nat Ryall                                                         09-Jul-2008
// =============================================================================
void CLobbyScreen::OnListSessionsCompleted(t_MatchResultError iError, xint iSessionCount, CSession* pSessions)
{
	if (iError == MatchResultError_Success)
	{
		m_iSessionCount = iSessionCount;
		m_pSessionList = pSessions;

		SetState(LobbyState_List);
	}
	else
	{
		SetState(LobbyState_None);
		ScreenManager::Pop();
	}
}

// =============================================================================
// Nat Ryall                                                         08-Jul-2008
// =============================================================================
void CLobbyScreen::OnCreateSessionCompleted(t_MatchResultError iError, CSession* pSession)
{
	if (iError == MatchResultError_Success)
	{
		Network.Reset();

		Network.m_xCallbacks.m_fpPeerJoined = xbind(this, &CLobbyScreen::OnPeerJoined);
		Network.m_xCallbacks.m_fpPeerLeaving = xbind(this, &CLobbyScreen::OnPeerLeaving);

		BindPacketCallbacks();

		Network.StartHost(16, HOST_INCOMING_PORT);

		SetState(LobbyState_Lobby);
	}
	else
	{
		SetState(LobbyState_None);
		ScreenManager::Pop();
	}
}

// =============================================================================
// Nat Ryall                                                         09-Jul-2008
// =============================================================================
void CLobbyScreen::OnCloseSessionCompleted(t_MatchResultError iError, CSession* pSession)
{
	delete m_pSession;
	m_pSession = NULL;

	SetState(LobbyState_None);
	ScreenManager::Pop();
}

// =============================================================================
// Nat Ryall                                                         15-Jun-2008
// =============================================================================
void CLobbyScreen::OnJoinClicked(CButtonComponent* pButton, xpoint xOffset)
{
	Network.Reset();

	Network.m_xCallbacks.m_fpConnectionCompleted = xbind(this, &CLobbyScreen::OnConnectionCompleted);
	Network.m_xCallbacks.m_fpConnectionLost = xbind(this, &CLobbyScreen::OnConnectionLost);
	Network.m_xCallbacks.m_fpPeerJoined = xbind(this, &CLobbyScreen::OnPeerJoined);
	Network.m_xCallbacks.m_fpPeerLeaving = xbind(this, &CLobbyScreen::OnPeerLeaving);

	Network.StartClient(m_pJoinInterface->m_pAddressBox->GetText(), HOST_INCOMING_PORT);

	BindPacketCallbacks();

	SetState(LobbyState_Connecting);
}

// =============================================================================
// Nat Ryall                                                         17-Jun-2008
// =============================================================================
void CLobbyScreen::OnConnectionCompleted(xbool bSuccess)
{
	if (bSuccess)
		SetState(LobbyState_Lobby);
	else
		SetState(LobbyState_Join);
}

// =============================================================================
// Nat Ryall                                                         17-Jun-2008
// =============================================================================
void CLobbyScreen::OnConnectionLost()
{
	SetState(LobbyState_Join);
}

// =============================================================================
// Nat Ryall                                                         18-Jun-2008
// =============================================================================
void CLobbyScreen::OnPeerJoined(CNetworkPeer* pPeer)
{
	XLOG("[LobbyScreen] Peer #%d joined the game.", pPeer->m_iID);
}

// =============================================================================
// Nat Ryall                                                         18-Jun-2008
// =============================================================================
void CLobbyScreen::OnPeerLeaving(CNetworkPeer* pPeer)
{
	XLOG("[LobbyScreen] Peer #%d is about to leave the game.", pPeer->m_iID);
}

// =============================================================================
// Nat Ryall                                                         18-Jun-2008
// =============================================================================
void CLobbyScreen::BindPacketCallbacks()
{
	Network.BindReceiveCallback(NetworkStreamType_PlayerInfo, xbind(this, &CLobbyScreen::OnReceivePlayerInfo));
}

// =============================================================================
// Nat Ryall                                                         18-Jun-2008
// =============================================================================
void CLobbyScreen::OnReceivePlayerInfo(CNetworkPeer* pFrom, BitStream* pStream)
{
	XLOG("[LobbyScreen] Received player info from peer #%d.", pFrom->m_iID);
}

//##############################################################################

//##############################################################################
//
//                               STATUS SCREEN
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         09-Jun-2008
// =============================================================================
CStatusBox::CStatusBox()
{
	m_pStatusBox = new CProgressComponent(_SPRITE("Menu-Status"));
	m_pStatusBox->SetInnerWidth(500);
	m_pStatusBox->SetPosition(xpoint(_HSWIDTH - (m_pStatusBox->GetWidth() / 2), _HSHEIGHT - (m_pStatusBox->GetHeight() / 2)));
	m_pStatusBox->SetEnabled(false);

	CFontMetadata* pFont = _FONT("Menu-Status");

	m_pLabel = new CLabelComponent(pFont);
	m_pLabel->SetAlignment(HGETEXT_CENTER);
	m_pLabel->SetPosition(xpoint(_HSWIDTH, _HSHEIGHT - ((xint)pFont->GetFont()->GetHeight() / 2)));
}

// =============================================================================
// Nat Ryall                                                         09-Jun-2008
// =============================================================================
CStatusBox::~CStatusBox()
{
	InterfaceManager.GetScreen()->Detach(m_pStatusBox);
	delete m_pStatusBox;

	InterfaceManager.GetScreen()->Detach(m_pLabel);
	delete m_pLabel;
}

//##############################################################################

//##############################################################################
//
//                                 JOIN SCREEN
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         15-Jun-2008
// =============================================================================
CJoinInterface::CJoinInterface()
{
	m_pAddressBox = new CInputComponent(_SPRITE("Menu-Input"), _FONT("Menu-Input"));
	m_pAddressBox->SetInnerWidth(300);

	m_pJoinButton = new CButtonComponent(_SPRITE("Menu-Button"), _FONT("Menu-Button"));
	m_pJoinButton->SetInnerWidth(100);
	m_pJoinButton->SetText(_LOCALE("Button_Join"));

	m_pAddressBox->SetPosition(xpoint(_HSWIDTH - ((m_pAddressBox->GetWidth() + m_pJoinButton->GetWidth() + 10) / 2), _HSHEIGHT - (m_pAddressBox->GetHeight() / 2)));
	m_pJoinButton->SetPosition(m_pAddressBox->GetPosition() + xpoint(m_pAddressBox->GetWidth() + 5, 0));
}

// =============================================================================
// Nat Ryall                                                         15-Jun-2008
// =============================================================================
CJoinInterface::~CJoinInterface()
{
	InterfaceManager.GetScreen()->Detach(m_pAddressBox);
	delete m_pAddressBox;

	InterfaceManager.GetScreen()->Detach(m_pJoinButton);
	delete m_pJoinButton;
}

//##############################################################################