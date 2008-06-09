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
	m_iMenuGroup = MenuGroupIndex_None;

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
		new CMenuLink(MenuGroupIndex_Online,	m_pMenuDefault,		_LOCALE("Menu_OnlineJoin"),		xbind(this, &CMenuScreen::Callback_JoinLobby)),
		new CMenuLink(MenuGroupIndex_Online,	m_pMenuDefault,		_LOCALE("Menu_OnlineCreate"),	NULL),
		new CMenuLink(MenuGroupIndex_Online,	m_pMenuHighlight,	_LOCALE("Menu_Back"),			xbind(this, &CMenuScreen::Callback_ShowMainMenu)),
	};

	for (XUINT iA = 0; iA < (sizeof(pLinkList) / sizeof(CMenuLink*)); ++iA)
	{
		m_lpMenuLinks[pLinkList[iA]->m_iGroupIndex].push_back(pLinkList[iA]);
		InterfaceManager.GetScreen()->Attach(pLinkList[iA]);
	}

	for (XUINT iGroup = 0; iGroup < MenuGroupIndex_Max; ++iGroup)
	{
		XUINT iElementCount = m_lpMenuLinks[iGroup].size();
		XUINT iElement = 0;

		XEN_LIST_FOREACH(t_MenuLinkList, ppMenuLink, m_lpMenuLinks[iGroup])
		{
			(*ppMenuLink)->SetVisible(false);
			(*ppMenuLink)->RePosition(iElement++, iElementCount);
		}
	}

	m_iLastMenuGroup = MenuGroupIndex_Main;

	// Initialise statics.
	m_iNextScreen = ScreenIndex_Invalid;
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
	if (_HGE->Input_KeyDown(HGEK_ESCAPE) && m_iMenuGroup == MenuGroupIndex_Main)
	{
		_TERMINATE;
		return;
	}

	// Transition or update this screen.
	if (m_iNextScreen != ScreenIndex_Invalid)
	{
		ScreenManager::Push(m_iNextScreen);
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
	if (m_iMenuGroup != MenuGroupIndex_None)
	{
		XEN_LIST_FOREACH(t_MenuLinkList, ppMenuLink, m_lpMenuLinks[m_iMenuGroup])
			(*ppMenuLink)->SetVisible(false);
	}

	if (iMenuGroup != MenuGroupIndex_None)
	{
		XEN_LIST_FOREACH(t_MenuLinkList, ppMenuLink, m_lpMenuLinks[iMenuGroup])
			(*ppMenuLink)->SetVisible(true);
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
// Nat Ryall                                                          5-May-2008
// =============================================================================
void CMenuScreen::Callback_JoinLobby()
{
	m_iNextScreen = ScreenIndex_LobbyScreen;
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