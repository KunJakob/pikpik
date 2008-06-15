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
	m_pJoinInterface = new CJoinInterface();
	m_pStatusBox = new CStatusBox();

	m_pJoinInterface->m_pJoinButton->SetClickCallback(xbind(this, &CLobbyScreen::OnJoinClicked));
}

// =============================================================================
// Nat Ryall                                                         09-Jun-2008
// =============================================================================
void CLobbyScreen::Unload()
{
	delete m_pJoinInterface;
	delete m_pStatusBox;
}

// =============================================================================
// Nat Ryall                                                         09-Jun-2008
// =============================================================================
void CLobbyScreen::Update()
{
	// Allow "ESC" to exit the screen.
	if (_HGE->Input_KeyUp(HGEK_ESCAPE))
	{
		switch (m_iState)
		{
		case LobbyState_Join:
			ScreenManager::Pop();
			break;
		}
	}

	UpdateParent();
}

// =============================================================================
// Nat Ryall                                                         09-Jun-2008
// =============================================================================
void CLobbyScreen::Render()
{
	RenderParent();
}

// =============================================================================
// Nat Ryall                                                         09-Jun-2008
// =============================================================================
void CLobbyScreen::Start(t_LobbyStartMode iStartMode)
{
	switch (iStartMode)
	{
	case LobbyStartMode_Join:
		SetState(LobbyState_Join);
		break;
	
	case LobbyStartMode_Create:
		SetState(LobbyState_Connecting);
		break;
	}
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
		m_pJoinInterface->AttachElements();
		break;

	case LobbyState_Creating:
		m_pStatusBox->AttachElements();
		m_pStatusBox->SetText(_LOCALE("Status_Creating"));
		break;

	case LobbyState_Connecting:
		m_pStatusBox->AttachElements();
		m_pStatusBox->SetText(_LOCALE("Status_Connecting"));
		break;

	case LobbyState_Joining:
		m_pStatusBox->AttachElements();
		m_pStatusBox->SetText(_LOCALE("Status_Joining"));
		break;
	}

	m_iState = iState;
}

// =============================================================================
// Nat Ryall                                                         15-Jun-2008
// =============================================================================
void CLobbyScreen::OnJoinClicked(CButtonComponent* pButton, xpoint xOffset)
{
	SetState(LobbyState_Connecting);
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

// =============================================================================
// Nat Ryall                                                         09-Jun-2008
// =============================================================================
void CStatusBox::SetText(const xchar* pStatus)
{
	m_pLabel->SetText(pStatus);
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
	m_pAddressBox->SetText("127.0.0.1");

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