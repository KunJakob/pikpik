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
	m_iState(LobbyState_None)
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
	case LobbyStartMode_Join:
		{
			SetState(LobbyState_Join);
		}
		break;
	
	case LobbyStartMode_Create:
		{
			Network.Reset();

			//CNetworkPlayerInfo* pPlayerInfo = new CNetworkPlayerInfo();
			//strcpy_s(pPlayerInfo->cNickname, MAX_NICKNAME_LENGTH, "Krakken");

			Network.StartHost(16, HOST_INCOMING_PORT);

			SetState(LobbyState_Lobby);
		}
		break;
	}

	m_iStartMode = iStartMode;
}

// =============================================================================
// Nat Ryall                                                         15-Jun-2008
// =============================================================================
void CLobbyScreen::SetState(t_LobbyState iState)
{
	InterfaceManager.GetScreen()->DetachAll();

	switch (iState)
	{
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
		case LobbyState_Join:
			{
				ScreenManager::Pop();
			}
			break;
		
		case LobbyState_Lobby:
			{
				if (Network.m_bHosting)
					ScreenManager::Pop();
				else
					SetState(LobbyState_Join);

				Network.Stop();
			}
			break;
		}
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
	//Network.m_pInterface->GetLocalIP(0);

	// Render the peer list.
	xint iPeerOffset = 0;

	XEN_LIST_FOREACH(t_NetworkPeerList, ppPeer, Network.m_lpPeers)
	{
		m_pPeerFont->Render(XFORMAT("Peer_%02d", (*ppPeer)->m_iID), xpoint(50, 50 + iPeerOffset), HGETEXT_LEFT);
		iPeerOffset += 40;
	}
}

// =============================================================================
// Nat Ryall                                                         15-Jun-2008
// =============================================================================
void CLobbyScreen::OnJoinClicked(CButtonComponent* pButton, xpoint xOffset)
{
	Network.Reset();

	Network.m_xCallbacks.m_fpConnectionCompleted = xbind(this, &CLobbyScreen::OnConnectionCompleted);
	Network.m_xCallbacks.m_fpConnectionLost = xbind(this, &CLobbyScreen::OnConnectionLost);

	Network.StartClient(m_pJoinInterface->m_pAddressBox->GetText(), HOST_INCOMING_PORT);

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