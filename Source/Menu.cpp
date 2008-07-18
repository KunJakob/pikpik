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
CMenuLink::CMenuLink(xuint iGroupIndex, CFontMetadata* pFont, const xchar* pText, t_fpLinkSelectedCallback fpCallback) : CTextLink(pFont, pText, fpCallback),
	m_iGroupIndex(iGroupIndex)
{
	m_iType = ElementType_MenuLink;
}

// =============================================================================
// Nat Ryall                                                         18-Apr-2008
// =============================================================================
void CMenuLink::RePosition(xuint iElementIndex, xuint iNumElements)
{
	xpoint xPosition = xpoint(_HSWIDTH - (GetWidth() / 2), _HSHEIGHT - (iNumElements * (GetHeight() / 2)) + iElementIndex * (GetHeight() + 5));
	SetPosition(xPosition);
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
	Interface.SetCursor(_SPRITE("Cursor-Main"));
	Interface.SetCursor(ElementType_Button, _SPRITE("Cursor-Click"));
	Interface.SetCursor(ElementType_Input, _SPRITE("Cursor-Write"));
	Interface.SetCursor(ElementType_MenuLink, _SPRITE("Cursor-Click"));
	Interface.SetCursor(ElementType_SessionBox, _SPRITE("Cursor-Click"));

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

		// Online.
		new CMenuLink(MenuGroupIndex_Online,	m_pMenuDefault,		_LOCALE("Menu_OnlineJoin"),		xbind(this, &CMenuScreen::Callback_ShowJoinMenu)),
		new CMenuLink(MenuGroupIndex_Online,	m_pMenuDefault,		_LOCALE("Menu_OnlineCreate"),	xbind(this, &CMenuScreen::Callback_ShowCreateMenu)),
		new CMenuLink(MenuGroupIndex_Online,	m_pMenuHighlight,	_LOCALE("Menu_Back"),			xbind(this, &CMenuScreen::Callback_ShowMainMenu)),

		// Find.
		new CMenuLink(MenuGroupIndex_Join,		m_pMenuDefault,		_LOCALE("Menu_JoinPublic"),		xbind(this, &CMenuScreen::Callback_JoinPublic)),
		new CMenuLink(MenuGroupIndex_Join,		m_pMenuDefault,		_LOCALE("Menu_JoinPrivate"),	xbind(this, &CMenuScreen::Callback_JoinPrivate)),
		new CMenuLink(MenuGroupIndex_Join,		m_pMenuHighlight,	_LOCALE("Menu_Back"),			xbind(this, &CMenuScreen::Callback_ShowOnlineMenu)),

		// Create.
		new CMenuLink(MenuGroupIndex_Create,	m_pMenuDefault,		_LOCALE("Menu_CreatePublic"),	xbind(this, &CMenuScreen::Callback_CreatePublic)),
		new CMenuLink(MenuGroupIndex_Create,	m_pMenuDefault,		_LOCALE("Menu_CreatePrivate"),	xbind(this, &CMenuScreen::Callback_CreatePrivate)),
		new CMenuLink(MenuGroupIndex_Create,	m_pMenuHighlight,	_LOCALE("Menu_Back"),			xbind(this, &CMenuScreen::Callback_ShowOnlineMenu)),
	};

	for (xuint iA = 0; iA < (sizeof(pLinkList) / sizeof(CMenuLink*)); ++iA)
		m_lpMenuLinks[pLinkList[iA]->m_iGroupIndex].push_back(pLinkList[iA]);

	for (xuint iGroup = 0; iGroup < MenuGroupIndex_Max; ++iGroup)
	{
		xuint iElementCount = (xuint)m_lpMenuLinks[iGroup].size();
		xuint iElement = 0;

		XEN_LIST_FOREACH(t_MenuLinkList, ppMenuLink, m_lpMenuLinks[iGroup])
			(*ppMenuLink)->RePosition(iElement++, iElementCount);
	}

	// Initialise transition variables.
	m_iLastMenuGroup = MenuGroupIndex_Main;

	m_iNextScreen = ScreenIndex_Invalid;
	m_iLobbyMode = LobbyStartMode_CreatePrivate;
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

		case MenuGroupIndex_Join:
		case MenuGroupIndex_Create:
			SetMenuGroup(MenuGroupIndex_Online);
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
}

// =============================================================================
// Nat Ryall                                                         13-Apr-2008
// =============================================================================
void CMenuScreen::Render()
{
	m_pBackground->Render();
}

// =============================================================================
// Nat Ryall                                                         13-Apr-2008
// =============================================================================
void CMenuScreen::SetMenuGroup(t_MenuGroupIndex iMenuGroup)
{
	Interface.GetScreen()->DetachAll();

	if (iMenuGroup != MenuGroupIndex_None)
	{
		XEN_LIST_FOREACH(t_MenuLinkList, ppMenuLink, m_lpMenuLinks[iMenuGroup])
			Interface.GetScreen()->Attach(*ppMenuLink);
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
void CMenuScreen::Callback_ShowOnlineMenu()
{
	SetMenuGroup(MenuGroupIndex_Online);
}

// =============================================================================
// Nat Ryall                                                         09-Jul-2008
// =============================================================================
void CMenuScreen::Callback_ShowJoinMenu()
{
	SetMenuGroup(MenuGroupIndex_Join);
}

// =============================================================================
// Nat Ryall                                                         10-Jul-2008
// =============================================================================
void CMenuScreen::Callback_ShowCreateMenu()
{
	SetMenuGroup(MenuGroupIndex_Create);
}

// =============================================================================
// Nat Ryall                                                         09-Jul-2008
// =============================================================================
void CMenuScreen::Callback_JoinPublic()
{
	m_iNextScreen = ScreenIndex_LobbyScreen;
	m_iLobbyMode = LobbyStartMode_JoinPublic;
}

// =============================================================================
// Nat Ryall                                                          5-May-2008
// =============================================================================
void CMenuScreen::Callback_JoinPrivate()
{
	m_iNextScreen = ScreenIndex_LobbyScreen;
	m_iLobbyMode = LobbyStartMode_JoinPrivate;
}

// =============================================================================
// Nat Ryall                                                         15-Jun-2008
// =============================================================================
void CMenuScreen::Callback_CreatePublic()
{
	m_iNextScreen = ScreenIndex_LobbyScreen;
	m_iLobbyMode = LobbyStartMode_CreatePublic;
}

// =============================================================================
// Nat Ryall                                                         27-Apr-2008
// =============================================================================
void CMenuScreen::Callback_CreatePrivate()
{
	m_iNextScreen = ScreenIndex_LobbyScreen;
	m_iLobbyMode = LobbyStartMode_CreatePrivate;
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

//##############################################################################
#pragma endregion