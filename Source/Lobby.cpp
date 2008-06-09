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
CLobbyScreen::CLobbyScreen() : CScreen(ScreenIndex_LobbyScreen)
{
}

// =============================================================================
// Nat Ryall                                                         22-May-2008
// =============================================================================
CLobbyScreen::~CLobbyScreen()
{
}

// =============================================================================
// Nat Ryall                                                         09-Jun-2008
// =============================================================================
void CLobbyScreen::Load()
{
	m_pAddressBox = new CInputComponent(_SPRITE("Menu-Input"), _FONT("Menu-Input"));
	m_pAddressBox->SetInnerWidth(300);
	m_pAddressBox->SetText("127.0.0.1");

	m_pJoinButton = new CButtonComponent(_SPRITE("Menu-Button"), _FONT("Menu-Button"));
	m_pJoinButton->SetInnerWidth(100);
	m_pJoinButton->SetText("Join");

	m_pAddressBox->SetPosition(xpoint(_HSWIDTH - ((m_pAddressBox->GetWidth() + m_pJoinButton->GetWidth() + 10) / 2), _HSHEIGHT - (m_pAddressBox->GetHeight() / 2)));
	m_pJoinButton->SetPosition(m_pAddressBox->GetPosition() + xpoint(m_pAddressBox->GetWidth() + 5, 0));

	InterfaceManager.GetScreen()->Attach(m_pAddressBox);
	InterfaceManager.GetScreen()->Attach(m_pJoinButton);
}

// =============================================================================
// Nat Ryall                                                         09-Jun-2008
// =============================================================================
void CLobbyScreen::Unload()
{
	InterfaceManager.GetScreen()->Detach(m_pAddressBox);
	delete m_pAddressBox;

	InterfaceManager.GetScreen()->Detach(m_pJoinButton);
	delete m_pJoinButton;
}

// =============================================================================
// Nat Ryall                                                         09-Jun-2008
// =============================================================================
void CLobbyScreen::Update()
{
	// Allow "ESC" to exit the screen.
	if (_HGE->Input_KeyDown(HGEK_ESCAPE))
	{
		ScreenManager::Pop();	
		return;
	}

	ExecuteParentUpdate();
}

// =============================================================================
// Nat Ryall                                                         09-Jun-2008
// =============================================================================
void CLobbyScreen::Render()
{
	ExecuteParentRender();
}

//##############################################################################