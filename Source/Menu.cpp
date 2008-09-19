/**
* @file Menu.cpp
* @author Nat Ryall
* @date 11/04/2008
* @brief Manages menu transitions.
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
#include <Menu.h>

// Other.
#include <Resource.h>
#include <Font.h>
#include <Selection.h>
#include <Component.h>
#include <Lobby.h>

//##############################################################################

//##############################################################################
//
//                                   MACROS
//
//##############################################################################

// The menu transition values.
#define MENU_TRANSITION_DELAY 50
#define MENU_TRANSITION_TIME 200
#define MENU_TRANSITION_DISTANCE _SWIDTH

//##############################################################################

//##############################################################################
//
//                                  ELEMENTS
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         13-Apr-2008
// =============================================================================
CMenuLink::CMenuLink(xuint iGroupIndex, CFontMetadata* pFont, const xchar* pText, t_fpLinkSelectedCallback fpCallback) : CTextLink(pFont, pText, fpCallback),
	m_iGroupIndex(iGroupIndex),
	m_iElementIndex(0)
{
	m_iType = ElementType_MenuLink;
}

// =============================================================================
// Nat Ryall                                                         18-Apr-2008
// =============================================================================
void CMenuLink::RePosition(xuint iElementIndex, xuint iNumElements)
{
	m_iElementIndex = iElementIndex;
	m_xLinkPosition = xpoint(_HSWIDTH - (GetWidth() / 2), _HSHEIGHT - (iNumElements * (GetHeight() / 2)) + iElementIndex * (GetHeight() + 10));

	SetPosition(m_xLinkPosition);
}

//##############################################################################

//##############################################################################
//
//                                 DEFINITION
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         01-Aug-2008
// =============================================================================
void CMenuScreen::OnLoad()
{
	// Initialise the render resources.
	m_pBackground = new CBackgroundImage("Menu-Background");

	m_pMenuDefault = _FONT("Menu-Default");
	m_pMenuHighlight = _FONT("Menu-Highlighted");

	// Initialise interface.
	InterfaceManager.SetCursor(_SPRITE("Cursor-Main"));
	InterfaceManager.SetCursor(ElementType_Button, _SPRITE("Cursor-Click"));
	InterfaceManager.SetCursor(ElementType_Input, _SPRITE("Cursor-Write"));
	InterfaceManager.SetCursor(ElementType_MenuLink, _SPRITE("Cursor-Click"));
	InterfaceManager.SetCursor(ElementType_SessionBox, _SPRITE("Cursor-Click"));

	// Initialise the menu links.
	CMenuLink* pLinkList[] = 
	{
		// Main.
		new CMenuLink(MenuGroup_Main,		m_pMenuDefault,		_LOCALE("Menu_Offline"),		xbind(this, &CMenuScreen::Callback_ShowLevelsMenu)),
		new CMenuLink(MenuGroup_Main,		m_pMenuDefault,		_LOCALE("Menu_Online"),			xbind(this, &CMenuScreen::Callback_ShowOnlineMenu)),
		new CMenuLink(MenuGroup_Main,		m_pMenuDefault,		_LOCALE("Menu_Tutorial"),		NULL),
		new CMenuLink(MenuGroup_Main,		m_pMenuDefault,		_LOCALE("Menu_Options"),		NULL),
		new CMenuLink(MenuGroup_Main,		m_pMenuDefault,		_LOCALE("Menu_Credits"),		NULL),
		new CMenuLink(MenuGroup_Main,		m_pMenuHighlight,	_LOCALE("Menu_Exit"),			xbind(this, &CMenuScreen::Callback_QuitGame)),

		// Online.
		new CMenuLink(MenuGroup_Online,		m_pMenuDefault,		_LOCALE("Menu_OnlineJoin"),		xbind(this, &CMenuScreen::Callback_ShowJoinMenu)),
		new CMenuLink(MenuGroup_Online,		m_pMenuDefault,		_LOCALE("Menu_OnlineCreate"),	xbind(this, &CMenuScreen::Callback_ShowCreateMenu)),
		new CMenuLink(MenuGroup_Online,		m_pMenuHighlight,	_LOCALE("Menu_Back"),			xbind(this, &CMenuScreen::Callback_ShowMainMenu)),

		// Find.
		new CMenuLink(MenuGroup_Join,		m_pMenuDefault,		_LOCALE("Menu_JoinPublic"),		xbind(this, &CMenuScreen::Callback_JoinPublic)),
		new CMenuLink(MenuGroup_Join,		m_pMenuDefault,		_LOCALE("Menu_JoinPrivate"),	xbind(this, &CMenuScreen::Callback_JoinPrivate)),
		new CMenuLink(MenuGroup_Join,		m_pMenuHighlight,	_LOCALE("Menu_Back"),			xbind(this, &CMenuScreen::Callback_ShowOnlineMenu)),

		// Create.
		new CMenuLink(MenuGroup_Create,		m_pMenuDefault,		_LOCALE("Menu_CreatePublic"),	xbind(this, &CMenuScreen::Callback_CreatePublic)),
		new CMenuLink(MenuGroup_Create,		m_pMenuDefault,		_LOCALE("Menu_CreatePrivate"),	xbind(this, &CMenuScreen::Callback_CreatePrivate)),
		new CMenuLink(MenuGroup_Create,		m_pMenuHighlight,	_LOCALE("Menu_Back"),			xbind(this, &CMenuScreen::Callback_ShowOnlineMenu)),
	};

	for (xuint iA = 0; iA < (sizeof(pLinkList) / sizeof(CMenuLink*)); ++iA)
		m_lpMenuLinks[pLinkList[iA]->m_iGroupIndex].push_back(pLinkList[iA]);

	for (xuint iGroup = 0; iGroup < MenuGroup_Max; ++iGroup)
	{
		xuint iElement = 0;

		XEN_LIST_FOREACH(t_MenuLinkList, ppMenuLink, m_lpMenuLinks[iGroup])
			(*ppMenuLink)->RePosition(iElement++, (xuint)m_lpMenuLinks[iGroup].size());
	}

	// Create a list of maps.
	for (int iA = 0; iA < MapManager.GetMapCount(); ++iA)
	{
		CMap* pMap = MapManager.GetMap(iA);
		CMenuLink* pMenuLink = new CMenuLink(MenuGroup_Levels, m_pMenuDefault, pMap->GetName(), xbind(this, &CMenuScreen::Callback_StartGame));

		pMenuLink->RePosition(iA, MapManager.GetMapCount() + 1);
		m_lpMenuLinks[MenuGroup_Levels].push_back(pMenuLink);
	}

	CMenuLink* pMenuLink = new CMenuLink(MenuGroup_Levels, m_pMenuHighlight, _LOCALE("Menu_Back"), xbind(this, &CMenuScreen::Callback_ShowMainMenu));

	pMenuLink->RePosition(MapManager.GetMapCount(), MapManager.GetMapCount() + 1);
	m_lpMenuLinks[MenuGroup_Levels].push_back(pMenuLink);
}

// =============================================================================
// Nat Ryall                                                         01-Aug-2008
// =============================================================================
void CMenuScreen::OnUnload()
{
	delete m_pBackground;
}

// =============================================================================
// Nat Ryall                                                         11-Apr-2008
// =============================================================================
void CMenuScreen::OnActivate()
{
	// Initialise transition variables.
	m_iState = MenuState_None;
	m_iMenuGroup = MenuGroup_None;
	m_iPendingMenuGroup = MenuGroup_None;
	m_iLastMenuGroup = MenuGroup_Main;
	m_iNextScreen = ScreenIndex_Invalid;
	m_iLobbyMode = LobbyStartMode_CreatePrivate;
}

// =============================================================================
// Nat Ryall                                                         11-Apr-2008
// =============================================================================
void CMenuScreen::OnDeactivate()
{
}

// =============================================================================
// Nat Ryall                                                         09-Jun-2008
// =============================================================================
void CMenuScreen::OnWake()
{
	if (Global.m_pActiveMap && Global.m_pActiveMap->IsLoaded())
		Global.m_pActiveMap->Unload();

	SetMenuGroup(m_iLastMenuGroup);
}

// =============================================================================
// Nat Ryall                                                         09-Jun-2008
// =============================================================================
void CMenuScreen::OnSleep()
{
	int i = 0;
}

// =============================================================================
// Nat Ryall                                                         13-Apr-2008
// =============================================================================
void CMenuScreen::OnUpdate()
{
	// Allow "ESC" to exit the game.
	if (m_iState == MenuState_Idle && _HGE->Input_KeyUp(HGEK_ESCAPE))
	{
		switch (m_iMenuGroup)
		{
		case MenuGroup_Main:
			_TERMINATE;
			return;

		case MenuGroup_Levels:
		case MenuGroup_Online:
			SetMenuGroup(MenuGroup_Main);
			break;

		case MenuGroup_Join:
		case MenuGroup_Create:
			SetMenuGroup(MenuGroup_Online);
			break;
		}
	}

	// Update based on the state.
	switch (m_iState)
	{
	case MenuState_TransitionIn:
		{
			UpdateTransition(true);
		}
		break;

	case MenuState_TransitionOut:
		{
			UpdateTransition(false);
		}
		break;
	}

	m_pBackground->Update();
}

// =============================================================================
// Nat Ryall                                                         20-Jul-2008
// =============================================================================
void CMenuScreen::UpdateTransition(xbool bTransitionIn)
{
	XEN_LIST_FOREACH(t_MenuLinkList, ppMenuLink, m_lpMenuLinks[m_iMenuGroup])
	{
		CMenuLink* pMenuLink = *ppMenuLink;

		if (pMenuLink->m_xStartTimer.IsExpired())
		{
			// Perform the transition in for each link as it expires.
			xint iOffset = XINTPERCENT(pMenuLink->m_iTransitionTime, MENU_TRANSITION_TIME, MENU_TRANSITION_DISTANCE);
			xint iDirectionalOffset = bTransitionIn ? MENU_TRANSITION_DISTANCE - iOffset : iOffset;

			pMenuLink->m_iTransitionTime = Math::Clamp<xuint>(pMenuLink->m_iTransitionTime + _TIMEDELTA, 0, MENU_TRANSITION_TIME);
			pMenuLink->SetPosition(pMenuLink->m_xLinkPosition + xpoint(pMenuLink->m_bTransitionRight ? iDirectionalOffset : -iDirectionalOffset, 0));

			// If we're the last one to finish the transition, go to 'idle'.
			if (pMenuLink->m_iElementIndex == ((xint)m_lpMenuLinks[m_iMenuGroup].size() - 1) && iOffset == MENU_TRANSITION_DISTANCE)
			{
				SetState(bTransitionIn ? MenuState_Idle : MenuState_None);
				return;
			}
		}
	}
}

// =============================================================================
// Nat Ryall                                                         13-Apr-2008
// =============================================================================
void CMenuScreen::OnRender()
{
	m_pBackground->Render();
}

// =============================================================================
// Nat Ryall                                                         20-Jul-2008
// =============================================================================
void CMenuScreen::SetState(t_MenuState iState)
{
	m_iState = iState;

	switch (iState)
	{
	case MenuState_None:
		{
			InterfaceScreen->SetEnabled(true);

			m_iLastMenuGroup = m_iMenuGroup;
			m_iMenuGroup = MenuGroup_None;

			ShowNextScreen();
		}
		break;

	case MenuState_TransitionIn:
		{
			InterfaceScreen->SetEnabled(false);
			InitTransition(true);
		}
		break;

	case MenuState_Idle:
		{
			InterfaceScreen->SetEnabled(true);
		}
		break;

	case MenuState_TransitionOut:
		{
			InterfaceScreen->SetEnabled(false);
			InitTransition(false);
		}
		break;
	}
}

// =============================================================================
// Nat Ryall                                                         20-Jul-2008
// =============================================================================
void CMenuScreen::SetNextScreen(t_ScreenIndex iScreenIndex)
{
	m_iNextScreen = iScreenIndex;
	SetState(MenuState_TransitionOut);
}

// =============================================================================
// Nat Ryall                                                         17-Jul-2008
// =============================================================================
void CMenuScreen::ShowNextScreen()
{
	if (m_iNextScreen != ScreenIndex_Invalid)
	{
		ScreenManager.Push(m_iNextScreen, true);

		switch (m_iNextScreen)
		{
		case ScreenIndex_LobbyScreen:
			{
				CLobbyScreen* pLobby = (CLobbyScreen*)ScreenManager.FindScreen(ScreenIndex_LobbyScreen);
				pLobby->Start(m_iLobbyMode);
			}
			break;
		}

		m_iNextScreen = ScreenIndex_Invalid;

		return;
	}

	if (m_iPendingMenuGroup != MenuGroup_None)
	{
		SetMenuGroup(m_iPendingMenuGroup);
		m_iPendingMenuGroup = MenuGroup_None;
	}
}

// =============================================================================
// Nat Ryall                                                         13-Apr-2008
// =============================================================================
void CMenuScreen::SetMenuGroup(t_MenuGroup iMenuGroup)
{
	if (iMenuGroup != MenuGroup_None)
	{
		if (m_iMenuGroup == MenuGroup_None)
		{
			AttachMenuGroup(iMenuGroup);
			SetState(MenuState_TransitionIn);
		}
		else
		{
			m_iPendingMenuGroup = iMenuGroup;
			SetState(MenuState_TransitionOut);
		}
	}
	else
	{
		if (m_iMenuGroup != MenuGroup_None)
			SetState(MenuState_TransitionOut);
	}
}

// =============================================================================
// Nat Ryall                                                         17-Jul-2008
// =============================================================================
void CMenuScreen::AttachMenuGroup(t_MenuGroup iMenuGroup)
{
	m_iMenuGroup = iMenuGroup;

	InterfaceScreen->DetachAll();

	if (iMenuGroup != MenuGroup_None)
	{
		XEN_LIST_FOREACH(t_MenuLinkList, ppMenuLink, m_lpMenuLinks[iMenuGroup])
			InterfaceScreen->Attach(*ppMenuLink);
	}
}

// =============================================================================
// Nat Ryall                                                         20-Jul-2008
// =============================================================================
void CMenuScreen::InitTransition(xbool bTransitionIn)
{
	XEN_LIST_FOREACH(t_MenuLinkList, ppMenuLink, m_lpMenuLinks[m_iMenuGroup])
	{
		CMenuLink* pMenuLink = *ppMenuLink;

		pMenuLink->m_bTransitionRight = (pMenuLink->m_iElementIndex % 2 == 0);
		pMenuLink->m_iTransitionTime = 0;
		pMenuLink->m_xStartTimer.ExpireAfter(MENU_TRANSITION_DELAY * pMenuLink->m_iElementIndex);

		if (bTransitionIn)
			pMenuLink->SetPosition(xpoint(pMenuLink->m_xLinkPosition.iX, -pMenuLink->GetHeight()));
		else
			pMenuLink->m_bTransitionRight = !pMenuLink->m_bTransitionRight;
	}
}

// =============================================================================
// Nat Ryall                                                         27-Apr-2008
// =============================================================================
void CMenuScreen::Callback_ShowMainMenu()
{
	SetMenuGroup(MenuGroup_Main);
}

// =============================================================================
// Nat Ryall                                                         27-Apr-2008
// =============================================================================
void CMenuScreen::Callback_ShowOnlineMenu()
{
	SetMenuGroup(MenuGroup_Online);
}

// =============================================================================
// Nat Ryall                                                         09-Jul-2008
// =============================================================================
void CMenuScreen::Callback_ShowJoinMenu()
{
	SetMenuGroup(MenuGroup_Join);
}

// =============================================================================
// Nat Ryall                                                         10-Jul-2008
// =============================================================================
void CMenuScreen::Callback_ShowCreateMenu()
{
	SetMenuGroup(MenuGroup_Create);
}

// =============================================================================
// Nat Ryall                                                         29-Jul-2008
// =============================================================================
void CMenuScreen::Callback_ShowLevelsMenu()
{
	SetMenuGroup(MenuGroup_Levels);
}

// =============================================================================
// Nat Ryall                                                         09-Jul-2008
// =============================================================================
void CMenuScreen::Callback_JoinPublic()
{
	SetNextScreen(ScreenIndex_LobbyScreen);
	m_iLobbyMode = LobbyStartMode_JoinPublic;
}

// =============================================================================
// Nat Ryall                                                          5-May-2008
// =============================================================================
void CMenuScreen::Callback_JoinPrivate()
{
	SetNextScreen(ScreenIndex_LobbyScreen);
	m_iLobbyMode = LobbyStartMode_JoinPrivate;
}

// =============================================================================
// Nat Ryall                                                         15-Jun-2008
// =============================================================================
void CMenuScreen::Callback_CreatePublic()
{
	SetNextScreen(ScreenIndex_LobbyScreen);
	m_iLobbyMode = LobbyStartMode_CreatePublic;
}

// =============================================================================
// Nat Ryall                                                         27-Apr-2008
// =============================================================================
void CMenuScreen::Callback_CreatePrivate()
{
	SetNextScreen(ScreenIndex_LobbyScreen);
	m_iLobbyMode = LobbyStartMode_CreatePrivate;
}

// =============================================================================
// Nat Ryall                                                         13-Apr-2008
// =============================================================================
void CMenuScreen::Callback_StartGame()
{
	CMenuLink* pLink = (CMenuLink*)InterfaceManager.GetActiveElement();

	Global.m_pActiveMap = MapManager.GetMap(pLink->m_iElementIndex);
	Global.m_pActiveMap->Load();

	SetNextScreen(ScreenIndex_GameScreen);
}

// =============================================================================
// Nat Ryall                                                         13-Apr-2008
// =============================================================================
void CMenuScreen::Callback_QuitGame()
{
	_TERMINATE;
}

//##############################################################################