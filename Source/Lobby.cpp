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
	m_bPublic(true),
	m_pSession(NULL)
{
}

// =============================================================================
// Nat Ryall                                                         22-May-2008
// =============================================================================
CLobbyScreen::~CLobbyScreen()
{
}

// =============================================================================
// Nat Ryall                                                         04-Aug-2008
// =============================================================================
void CLobbyScreen::OnLoad()
{
	// Load.
	m_pJoinInterface = new CJoinInterface();
	m_pStatusBox = new CStatusBox();

	m_pJoinInterface->m_pJoinButton->SetClickCallback(xbind(this, &CLobbyScreen::OnJoinClicked));

	// Initialise,
	m_pPeerFont = new CFont(_FONT("Lobby-Peer"));

	for (xint iA = 0; iA < MATCH_SESSION_LIMIT; ++iA)
		m_pSessionBoxes[iA] = NULL;

	
}

// =============================================================================
// Nat Ryall                                                         04-Aug-2008
// =============================================================================
void CLobbyScreen::OnUnload()
{
	delete m_pJoinInterface;
	delete m_pStatusBox;

	delete m_pPeerFont;

	DeleteSessionBoxes();
}

// =============================================================================
// Nat Ryall                                                         09-Jun-2008
// =============================================================================
void CLobbyScreen::OnActivate()
{
	m_iState = LobbyState_None;

	m_pJoinInterface->m_pAddressBox->SetText("127.0.0.1");
}

// =============================================================================
// Nat Ryall                                                         09-Jun-2008
// =============================================================================
void CLobbyScreen::OnDeactivate()
{
	m_iState = LobbyState_None;
}

// =============================================================================
// Nat Ryall                                                         30-Jul-2008
// =============================================================================
void CLobbyScreen::OnWake()
{
	switch (m_iState)
	{
	case LobbyState_Game:
		{
			EndGame();
			Stop();
		}
		break;
	}
}

// =============================================================================
// Nat Ryall                                                         30-Jul-2008
// =============================================================================
void CLobbyScreen::OnSleep()
{
}

// =============================================================================
// Nat Ryall                                                         09-Jun-2008
// =============================================================================
void CLobbyScreen::OnUpdate()
{
	if (_HGE->Input_KeyUp(HGEK_ESCAPE))
		Stop();

	UpdateParent();

	switch (m_iState)
	{
	case LobbyState_Lobby:
		{
			UpdateLobby();
		}
		break;

	case LobbyState_Closing:
		{
			if (!Match.IsBusy() && m_pSession)
				Match.CloseSession(m_pSession, xbind(this, &CLobbyScreen::OnCloseSessionCompleted));
		}
		break;
	}
}

// =============================================================================
// Nat Ryall                                                         09-Jun-2008
// =============================================================================
void CLobbyScreen::OnRender()
{
	RenderParent();

	switch (m_iState)
	{
	case LobbyState_Lobby:
		{
			RenderLobby();
		}
		break;
	}
}

// =============================================================================
// Nat Ryall                                                         17-Jun-2008
// =============================================================================
void CLobbyScreen::UpdateLobby()
{
	if (Network.IsHosting())
	{
		// Ping the matchmaking service on set intervals to keep the session alive.
		if (m_pSession && !Match.IsBusy() && m_xPingTimer.IsExpired())
		{
			Match.PingSession(m_pSession, NULL);
			m_xPingTimer.ExpireAfter(MATCH_PING_INTERVAL);
		}

		// Start the game when ENTER is pressed (debug).
		if (_HGE->Input_KeyUp(HGEK_ENTER) && Network.IsEveryoneVerified())
		{
			Network.Broadcast(NULL, NetworkStreamType_StartGame, NULL, HIGH_PRIORITY, RELIABLE_ORDERED);
			StartGame();
		}
	}
}

// =============================================================================
// Nat Ryall                                                         17-Jun-2008
// =============================================================================
void CLobbyScreen::RenderLobby()
{
	xint iPeerOffset = 0;

	XEN_LIST_FOREACH(t_NetworkPeerList, ppPeer, Network.GetVerifiedPeers())
	{
		CNetworkGamerCard* pCard = GetGamerCard(*ppPeer);
		
		m_pPeerFont->Render(XFORMAT("%s, %d", pCard->m_cNickname, pCard->m_iSeed), xpoint(50, 50 + iPeerOffset), HGETEXT_LEFT);
		iPeerOffset += 40;
	}

	Global.m_pGameFont->Render(XFORMAT("%d", Network.GetLastPing()), xpoint(2, 0), HGETEXT_LEFT);
}

// =============================================================================
// Nat Ryall                                                         09-Jun-2008
// =============================================================================
void CLobbyScreen::Start(t_LobbyStartMode iStartMode)
{
	m_xPingTimer.Reset();
	
	m_bPublic = (iStartMode == LobbyStartMode_JoinPublic || iStartMode == LobbyStartMode_CreatePublic);
	m_pSession = NULL;

	switch (iStartMode)
	{
	case LobbyStartMode_JoinPublic:
		{
			SetState(LobbyState_Searching);
			Match.ListSessions(xbind(this, &CLobbyScreen::OnListSessionsCompleted));
		}
		break;

	case LobbyStartMode_JoinPrivate:
		{
			SetState(LobbyState_Specify);
		}
		break;
	
	case LobbyStartMode_CreatePublic:
		{
			SetState(LobbyState_Creating);
			m_pSession = Match.CreateSession(4, "PikPik Beta Server", xbind(this, &CLobbyScreen::OnCreateSessionCompleted));
		}
		break;

	case LobbyStartMode_CreatePrivate:
		{
			SetState(LobbyState_Creating);
			CreateLobby();
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
	if (Network.IsRunning())
	{
		SetState(LobbyState_Leaving);
		Network.RequestStop();
	}
	else
		OnNetworkStop();
}

// =============================================================================
// Nat Ryall                                                         15-Jun-2008
// =============================================================================
void CLobbyScreen::SetState(t_LobbyState iState)
{
	InterfaceScreen->DetachAll();

	switch (iState)
	{
	case LobbyState_List:
		{
			for (int iA = 0; iA < m_iSessionCount; ++iA)
				InterfaceScreen->Attach(m_pSessionBoxes[iA]);
		}
		break;

	case LobbyState_Searching:
		{
			m_pStatusBox->AttachElements();
			m_pStatusBox->m_pLabel->SetText(_LOCALE("Status_Searching"));
		}
		break;

	case LobbyState_Specify:
		{
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

	case LobbyState_Verifying:
		{
			m_pStatusBox->AttachElements();
			m_pStatusBox->m_pLabel->SetText(_LOCALE("Status_Verifying"));
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
// Nat Ryall                                                         02-Aug-2008
// =============================================================================
void CLobbyScreen::InitialiseNetwork()
{
	// Initialise the local player's gamer card.
	static const char* s_pNames[] =
	{
		"PeterParker",
		"Krakken",
		"w0nd3rw0man",
		"<M00MIN>",
		"slygamer123",
		"Middas",
		"kazii",
		"Malus",
		"robspear",
		"KRM-Gaute",
		"|6P|_Pickis",
		"The-Guvna",
		"Firefoot",
		"Shaks",
		"Rlan",
		"cadron_9000",
		"billBo_bacons",
		"STALLONE [AH]",
		"TMWNN",
		"bkt"
	};

	strcpy_s(m_xGamerCard.m_cNickname, _MAXNAMELEN, s_pNames[rand() % 20]);
	m_xGamerCard.m_iSeed = rand() % 4096;

	Network.SetGamerCard(&m_xGamerCard, sizeof(CNetworkGamerCard));

	// Initialise the verification info.
	Network.SetVerificationInfo(_GID, (xint)strlen(_GID) + 1);

	// Bind the stream type callbacks.
	Network.BindReceiveCallback(NetworkStreamType_StartGame, xbind(this, &CLobbyScreen::OnReceiveStartGame));
	Network.BindReceiveCallback(NetworkStreamType_PlayerUpdate, &CPlayer::OnReceivePlayerUpdate);

	// Bind all event callbacks.
	Network.m_xCallbacks.m_fpNetworkStarted = xbind(this, &CLobbyScreen::OnNetworkStart);
	Network.m_xCallbacks.m_fpNetworkStopped = xbind(this, &CLobbyScreen::OnNetworkStop);
	Network.m_xCallbacks.m_fpConnectionCompleted = xbind(this, &CLobbyScreen::OnConnectionCompleted);
	Network.m_xCallbacks.m_fpConnectionLost = xbind(this, &CLobbyScreen::OnConnectionLost);
	Network.m_xCallbacks.m_fpVerifyPeer = xbind(this, &CLobbyScreen::OnVerifyPeer);
	Network.m_xCallbacks.m_fpVerificationCompleted = xbind(this, &CLobbyScreen::OnConnectionVerified);
	Network.m_xCallbacks.m_fpPeerJoined = xbind(this, &CLobbyScreen::OnPeerJoined);
	Network.m_xCallbacks.m_fpPeerLeaving = xbind(this, &CLobbyScreen::OnPeerLeaving);
}

// =============================================================================
// Nat Ryall                                                         10-Jul-2008
// =============================================================================
void CLobbyScreen::CreateLobby()
{
	Network.Reset();

	InitialiseNetwork();

	Network.StartHost(4, _HOSTPORT);

	SetState(LobbyState_Lobby);
}

// =============================================================================
// Nat Ryall                                                         10-Jul-2008
// =============================================================================
void CLobbyScreen::JoinLobby(const xchar* pHostAddress)
{
	Network.Reset();

	InitialiseNetwork();

	Network.StartClient(pHostAddress, _HOSTPORT);

	SetState(LobbyState_Connecting);
}

// =============================================================================
// Nat Ryall                                                         30-Jul-2008
// =============================================================================
void CLobbyScreen::StartGame()
{
	//SetState(LobbyState_Starting);

	// Load the map.
	Global.m_pActiveMap = MapManager.GetMap("M009");
	Global.m_pActiveMap->Load();

	srand(GetGamerCard(Network.GetVerifiedPeers().front())->m_iSeed);

	Global.ResetActivePlayers();
	
	// Initialise all players.
	XEN_LIST_FOREACH(t_PlayerList, ppPlayer, Global.m_lpActivePlayers)
	{
		CPlayer* pPlayer = *ppPlayer;
		pPlayer->SetLogicType(Network.GetLocalPeer()->m_bHost ? PlayerLogicType_AI : PlayerLogicType_Remote);
	}

	// Setup all network players.
	xint iPlayerIndex = 0;

	XEN_LIST_FOREACH(t_NetworkPeerList, ppPeer, Network.GetVerifiedPeers())
	{
		CNetworkPeer* pPeer = *ppPeer;
		CNetworkPeerInfo* pInfo = GetPeerInfo(pPeer);

		pInfo->m_pPlayer = Global.m_lpActivePlayers[iPlayerIndex++];
		pInfo->m_pPlayer->SetLogicType(pPeer->m_bLocal ? PlayerLogicType_Local : PlayerLogicType_Remote);

		if (pPeer->m_bLocal)
			Global.m_pLocalPlayer = pInfo->m_pPlayer;
	}

	SetState(LobbyState_Game);
	ScreenManager.Push(ScreenIndex_GameScreen);
}

// =============================================================================
// Nat Ryall                                                         30-Jul-2008
// =============================================================================
void CLobbyScreen::EndGame()
{
	Global.m_pActiveMap->Unload();
}

// =============================================================================
// Nat Ryall                                                         15-Jun-2008
// =============================================================================
void CLobbyScreen::OnJoinClicked(CButtonComponent* pButton, xpoint xOffset)
{
	JoinLobby(m_pJoinInterface->m_pAddressBox->GetText());
}

// =============================================================================
// Nat Ryall                                                         09-Jul-2008
// =============================================================================
void CLobbyScreen::OnListSessionsCompleted(t_MatchResultError iError, xint iSessionCount, CSession* pSessions)
{
	if (iError == MatchResultError_Success)
	{
		DeleteSessionBoxes();

		m_iSessionCount = iSessionCount;
		m_pSessionList = pSessions;

		for (xint iA = 0; iA < iSessionCount; ++iA)
			m_pSessionBoxes[iA] = new CSessionBox(iA, &m_pSessionList[iA]);

		SetState(LobbyState_List);
	}
	else
		Stop();
}

// =============================================================================
// Nat Ryall                                                         08-Jul-2008
// =============================================================================
void CLobbyScreen::OnCreateSessionCompleted(t_MatchResultError iError, CSession* pSession)
{
	if (iError == MatchResultError_Success)
		CreateLobby();
	else
		Stop();
}

// =============================================================================
// Nat Ryall                                                         09-Jul-2008
// =============================================================================
void CLobbyScreen::OnCloseSessionCompleted(t_MatchResultError iError, CSession* pSession)
{
	delete m_pSession;
	m_pSession = NULL;

	OnNetworkStop();
}

// =============================================================================
// Nat Ryall                                                         31-Jul-2008
// =============================================================================
xbool CLobbyScreen::OnVerifyPeer(CNetworkPeer* pPeer, void* pData, xint iDataLength)
{
	return iDataLength && pData && strcmp(_GID, (const xchar*)pData) == 0 && m_iState == LobbyState_Lobby;
}

// =============================================================================
// Nat Ryall                                                         01-Aug-2008
// =============================================================================
void CLobbyScreen::OnNetworkStart()
{
#if !defined(_RELEASE)
	Network.GetInterface()->ApplyNetworkSimulator(XKB(56), 80, 40);
#endif
}

// =============================================================================
// Nat Ryall                                                         01-Aug-2008
// =============================================================================
void CLobbyScreen::OnNetworkStop()
{
	if (m_iState != LobbyState_None)
	{
		// If we have a session still open, close that and return here later.
		if (Network.IsHosting() && m_pSession)
		{
			if (m_iState != LobbyState_Closing)
				SetState(LobbyState_Closing);
		}
		// Otherwise, we're ready to exit so let's leave.
		else
		{
			SetState(LobbyState_None);
			ScreenManager.Pop();
		}
	}
}

// =============================================================================
// Nat Ryall                                                         17-Jun-2008
// =============================================================================
void CLobbyScreen::OnConnectionCompleted(xbool bSuccess)
{
	if (bSuccess)
		SetState(LobbyState_Verifying);
	else
		Stop();
}

// =============================================================================
// Nat Ryall                                                         31-Jul-2008
// =============================================================================
void CLobbyScreen::OnConnectionVerified(xbool bSuccess)
{
	if (bSuccess)
		SetState(LobbyState_Lobby);
	else
		Stop();
}

// =============================================================================
// Nat Ryall                                                         17-Jun-2008
// =============================================================================
void CLobbyScreen::OnConnectionLost()
{
	OnNetworkStop();
}

// =============================================================================
// Nat Ryall                                                         18-Jun-2008
// =============================================================================
void CLobbyScreen::OnPeerJoined(CNetworkPeer* pPeer)
{
	XLOG("[LobbyScreen] '%s' joined the game with the ID %d.", GetGamerCard(pPeer)->m_cNickname, pPeer->m_iID);

	CNetworkPeerInfo* pInfo = new CNetworkPeerInfo();
	pInfo->m_pPlayer = NULL;

	pPeer->m_pData = pInfo;

	Network.SortPeers();
}

// =============================================================================
// Nat Ryall                                                         18-Jun-2008
// =============================================================================
void CLobbyScreen::OnPeerLeaving(CNetworkPeer* pPeer)
{
	XLOG("[LobbyScreen] '%s' with the ID %d is about to leave the game.", GetGamerCard(pPeer)->m_cNickname, pPeer->m_iID);

	CNetworkPeerInfo* pInfo = GetPeerInfo(pPeer);

	if (m_iState == LobbyState_Game)
		pInfo->m_pPlayer->SetLogicType(Network.IsHosting() ? PlayerLogicType_AI : PlayerLogicType_Remote);

	if (pInfo)
		delete pInfo;
}

// =============================================================================
// Nat Ryall                                                         30-Jul-2008
// =============================================================================
void CLobbyScreen::OnReceiveStartGame(CNetworkPeer* pFrom, BitStream* pStream)
{
	XLOG("[LobbyScreen] Received notification to start the game.");

	StartGame();
}

// =============================================================================
// Nat Ryall                                                         13-Jul-2008
// =============================================================================
void CLobbyScreen::DeleteSessionBoxes()
{
	for (xint iA = 0; iA < MATCH_SESSION_LIMIT; ++iA)
	{
		if (m_pSessionBoxes[iA])
		{
			delete m_pSessionBoxes[iA];
			m_pSessionBoxes[iA] = NULL;
		}
	}
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
	InterfaceScreen->Detach(m_pStatusBox);
	delete m_pStatusBox;

	InterfaceScreen->Detach(m_pLabel);
	delete m_pLabel;
}

//##############################################################################

//##############################################################################
//
//                             MESSAGE INTERFACE
//
//##############################################################################

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
	InterfaceScreen->Detach(m_pAddressBox);
	delete m_pAddressBox;

	InterfaceScreen->Detach(m_pJoinButton);
	delete m_pJoinButton;
}

//##############################################################################

//##############################################################################
//
//                                 SESSION BOX
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         13-Jul-2008
// =============================================================================
CSessionBox::CSessionBox(xint iIndex, CSession* pSession) : CImageComponent(_SPRITE("Menu-Session")),
	m_iIndex(iIndex),
	m_pSession(pSession)
{
	m_iType = ElementType_SessionBox;

	m_pTitleFont = new CFont(_FONT("Menu-Session-Title"));
	m_pInfoFont = new CFont(_FONT("Menu-Session-Info"));

	xpoint xSize = m_pSprite->GetImageSize();
	SetPosition(xpoint(_HSWIDTH - (xSize.iX / 2), 40 + (m_iIndex * (xSize.iY + 10))));
}

// =============================================================================
// Nat Ryall                                                         13-Jul-2008
// =============================================================================
CSessionBox::~CSessionBox()
{
	delete m_pTitleFont;
	delete m_pInfoFont;
}

// =============================================================================
// Nat Ryall                                                         13-Jul-2008
// =============================================================================
void CSessionBox::OnRender()
{
	CImageComponent::OnRender();

	m_pTitleFont->Render
	(
		m_pSession->m_sTitle.c_str(), 
		m_pSprite->GetMetadata()->FindArea("Title")->xRect + GetPosition(), 
		HGETEXT_CENTER | HGETEXT_MIDDLE
	);

	m_pInfoFont->Render
	(
		XFORMAT("%d Playing", m_pSession->m_iUsedSlots), 
		m_pSprite->GetMetadata()->FindArea("Info")->xRect + GetPosition(), 
		HGETEXT_CENTER | HGETEXT_MIDDLE
	);
}

// =============================================================================
// Nat Ryall                                                         13-Jul-2008
// =============================================================================
void CSessionBox::OnMouseUp(xpoint xPosition)
{
	CLobbyScreen* pLobby = (CLobbyScreen*)ScreenManager.FindScreen(ScreenIndex_LobbyScreen);
	pLobby->JoinLobby(m_pSession->m_sIP.c_str());
}

//##############################################################################
