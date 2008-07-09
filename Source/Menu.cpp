/**
* @file Menu.cpp
* @author Nat Ryall
* @date 11/04/2008
* @brief Manages menu transitions.
*
* Copyright © SAPIAN
*/

#pragma region Include
//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Global.
#include <Global.h>

// Local.
#include <Menu.h>

// Other.
#include <Resource.h>
#include <Font.h>
#include <Selection.h>
#include <Component.h>
#include <Lobby.h>

//##############################################################################
#pragma endregion

#pragma region Elements
//##############################################################################
//
//                                  ELEMENTS
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         13-Apr-2008
// =============================================================================
CMenuLink::CMenuLink(XUINT iGroupIndex, CFontMetadata* pFont, const XCHAR* pText, t_fpLinkSelectedCallback fpCallback) : CInterfaceElement(ElementType_MenuLink),
	m_iGroupIndex(iGroupIndex),
	m_pText(NULL),
	m_fpLinkSelectedCallback(fpCallback)
{
	m_pText = new CText(pFont);
	m_pText->SetString(pText);
	m_pText->SetAlignment(HGETEXT_LEFT);
}

// =============================================================================
// Nat Ryall                                                         13-Apr-2008
// =============================================================================
CMenuLink::~CMenuLink()
{
	delete m_pText;
}

// =============================================================================
// Nat Ryall                                                         13-Apr-2008
// =============================================================================
void CMenuLink::Update()
{
}

// =============================================================================
// Nat Ryall                                                         13-Apr-2008
// =============================================================================
void CMenuLink::Render()
{
	m_pText->Render();
}

// =============================================================================
// Nat Ryall                                                         18-Apr-2008
// =============================================================================
void CMenuLink::RePosition(XUINT iElementIndex, XUINT iNumElements)
{
	XPOINT xPosition = XPOINT(_HSWIDTH - (GetWidth() / 2), _HSHEIGHT - (iNumElements * (GetHeight() / 2)) + iElementIndex * (GetHeight() + 5));

	SetPosition(xPosition);
	m_pText->SetPosition(xPosition);
}

//##############################################################################
#pragma endregion

#pragma region Definition
//##############################################################################
//
//                                 DEFINITION
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         11-Apr-2008
// =============================================================================
void CMenuScreen::Load()
{
	_GLOBAL.pMenu = this;

	// Initialise the render resources.
	m_pBackground = new CBackgroundImage("Menu-Background");

	m_pMenuDefault = _FONT("Menu-Default");
	m_pMenuHighlight = _FONT("Menu-Highlighted");

	// Initialise interface.
	InterfaceManager.SetCursor(_SPRITE("Cursor-Main"));
	InterfaceManager.SetCursor(ElementType_Button, _SPRITE("Cursor-Click"));
	InterfaceManager.SetCursor(ElementType_Input, _SPRITE("Cursor-Write"));
	InterfaceManager.SetCursor(ElementType_MenuLink, _SPRITE("Cursor-Click"));

	// Initialise the menu links.
	CMenuLink* pLinkList[] = 
	{
		// Main.
		new CMenuLink(MenuGroupIndex_Main,		m_pMenuHighlight,	_LOCALE("Menu_Online"),			xbind(this, &CMenuScreen::Callback_ShowOnlineMenu)),
		new CMenuLink(MenuGroupIndex_Main,		m_pMenuDefault,		_LOCALE("Menu_Offline"),		xbind(this, &CMenuScreen::Callback_StartGame)),
		new CMenuLink(MenuGroupIndex_Main,		m_pMenuDefault,		_LOCALE("Menu_Tutorial"),		NULL),
		new CMenuLink(MenuGroupIndex_Main,		m_pMenuDefault,		_LOCALE("Menu_Options"),		NULL),
		new CMenuLink(MenuGroupIndex_Main,		m_pMenuDefault,		_LOCALE("Menu_Credits"),		NULL),
		new CMenuLink(MenuGroupIndex_Main,		m_pMenuDefault,		_LOCALE("Menu_Exit"),			xbind(this, &CMenuScreen::Callback_QuitGame)),

		// Play.
		new CMenuLink(MenuGroupIndex_Play,		m_pMenuHighlight,	_LOCALE("Menu_Back"),			xbind(this, &CMenuScreen::Callback_ShowMainMenu)),

		// Online.
		new CMenuLink(MenuGroupIndex_Online,	m_pMenuDefault,		_LOCALE("Menu_OnlineFind"),		xbind(this, &CMenuScreen::Callback_FindLobby)),
		new CMenuLink(MenuGroupIndex_Online,	m_pMenuDefault,		_LOCALE("Menu_OnlineJoin"),		xbind(this, &CMenuScreen::Callback_JoinLobby)),
		new CMenuLink(MenuGroupIndex_Online,	m_pMenuDefault,		_LOCALE("Menu_OnlineCreate"),	xbind(this, &CMenuScreen::Callback_CreateLobby)),
		new CMenuLink(MenuGroupIndex_Online,	m_pMenuHighlight,	_LOCALE("Menu_Back"),			xbind(this, &CMenuScreen::Callback_ShowMainMenu)),
	};

	for (xuint iA = 0; iA < (sizeof(pLinkList) / sizeof(CMenuLink*)); ++iA)
		m_lpMenuLinks[pLinkList[iA]->m_iGroupIndex].push_back(pLinkList[iA]);

	for (xuint iGroup = 0; iGroup < MenuGroupIndex_Max; ++iGroup)
	{
		xuint iElementCount = m_lpMenuLinks[iGroup].size();
		xuint iElement = 0;

		XEN_LIST_FOREACH(t_MenuLinkList, ppMenuLink, m_lpMenuLinks[iGroup])
			(*ppMenuLink)->RePosition(iElement++, iElementCount);
	}

	// Initialise transition variables.
	m_iLastMenuGroup = MenuGroupIndex_Main;
	m_iNextScreen = ScreenIndex_Invalid;
	m_iLobbyMode = LobbyStartMode_Join;
}

// =============================================================================
// Nat Ryall                                                         11-Apr-2008
// =============================================================================
void CMenuScreen::Unload()
{
	//delete m_pMenuDefault;
	//delete m_pMenuHighlight; 

	// TODO: Fix the above.

	delete m_pBackground;
}

// =============================================================================
// Nat Ryall                                                         09-Jun-2008
// =============================================================================
void CMenuScreen::Wake()
{
	SetMenuGroup(m_iLastMenuGroup);
}

// =============================================================================
// Nat Ryall                                                         09-Jun-2008
// =============================================================================
void CMenuScreen::Sleep()
{
	m_iLastMenuGroup = m_iMenuGroup;
	SetMenuGroup(MenuGroupIndex_None);
}

// =============================================================================
// Nat Ryall                                                         13-Apr-2008
// =============================================================================
void CMenuScreen::Update()
{
	// Allow "ESC" to exit the game.
	if (_HGE->Input_KeyUp(HGEK_ESCAPE))
	{
		switch (m_iMenuGroup)
		{
		case MenuGroupIndex_Main:
			_TERMINATE;
			return;

		case MenuGroupIndex_Online:
			SetMenuGroup(MenuGroupIndex_Main);
			break;
		}
	}

	// Transition or update this screen.
	if (m_iNextScreen != ScreenIndex_Invalid)
	{
		ScreenManager::Push(m_iNextScreen);

		switch (m_iNextScreen)
		{
		case ScreenIndex_LobbyScreen:
			_GLOBAL.pLobby->Start(m_iLobbyMode);
			break;
		}

		m_iNextScreen = ScreenIndex_Invalid;
	}

	m_pBackground->Update();
	InterfaceManager.Update();
}

// =============================================================================
// Nat Ryall                                                         13-Apr-2008
// =============================================================================
void CMenuScreen::Render()
{
	m_pBackground->Render();
	InterfaceManager.Render();
}

// =============================================================================
// Nat Ryall                                                         13-Apr-2008
// =============================================================================
void CMenuScreen::SetMenuGroup(t_MenuGroupIndex iMenuGroup)
{
	InterfaceManager.GetScreen()->DetachAll();

	if (iMenuGroup != MenuGroupIndex_None)
	{
		XEN_LIST_FOREACH(t_MenuLinkList, ppMenuLink, m_lpMenuLinks[iMenuGroup])
			InterfaceManager.GetScreen()->Attach(*ppMenuLink);
	}

	m_iMenuGroup = iMenuGroup;
}

// =============================================================================
// Nat Ryall                                                         27-Apr-2008
// =============================================================================
void CMenuScreen::Callback_ShowMainMenu()
{
	SetMenuGroup(MenuGroupIndex_Main);
}

// =============================================================================
// Nat Ryall                                                         27-Apr-2008
// =============================================================================
void CMenuScreen::Callback_ShowPlayMenu()
{
	SetMenuGroup(MenuGroupIndex_Play);
}

// =============================================================================
// Nat Ryall                                                         27-Apr-2008
// =============================================================================
void CMenuScreen::Callback_ShowOnlineMenu()
{
	SetMenuGroup(MenuGroupIndex_Online);
}

// =============================================================================
// Nat Ryall                                                         09-Jul-2008
// =============================================================================
void CMenuScreen::Callback_FindLobby()
{
	m_iNextScreen = ScreenIndex_LobbyScreen;
	m_iLobbyMode = LobbyStartMode_Find;
}

// =============================================================================
// Nat Ryall                                                          5-May-2008
// =============================================================================
void CMenuScreen::Callback_JoinLobby()
{
	m_iNextScreen = ScreenIndex_LobbyScreen;
	m_iLobbyMode = LobbyStartMode_Join;
}

// =============================================================================
// Nat Ryall                                                         15-Jun-2008
// =============================================================================
void CMenuScreen::Callback_CreateLobby()
{
	m_iNextScreen = ScreenIndex_LobbyScreen;
	m_iLobbyMode = LobbyStartMode_Create;
}

// =============================================================================
// Nat Ryall                                                         27-Apr-2008
// =============================================================================
void CMenuScreen::Callback_StartLobby()
{
}

// =============================================================================
// Nat Ryall                                                         13-Apr-2008
// =============================================================================
void CMenuScreen::Callback_StartGame()
{
	m_iNextScreen = ScreenIndex_GameScreen;
}

// =============================================================================
// Nat Ryall                                                         13-Apr-2008
// =============================================================================
void CMenuScreen::Callback_QuitGame()
{
	_TERMINATE;
}

// =============================================================================
// Nat Ryall                                                         16-May-2008
// =============================================================================
void CMenuScreen::Debug_HideWindow(CButtonComponent* pButton, xpoint xOffset)
{
	pButton->GetParent()->SetPosition(xpoint());
}

//##############################################################################
#pragma endregion