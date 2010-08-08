/**
* @file Menu.cpp
* @author Nat Ryall
* @date 11/04/2008
* @brief Manages menu transitions.
*
* Copyright © SAPIAN
*/

//##############################################################################

// Global.
#include <Global.h>

// Local.
#include <Menu.h>

// Other.
#include <Resource.h>
#include <Font.h>
#include <Component.h>
#include <Lobby.h>

//##############################################################################

// The menu transition values.
#define MENU_TRANSITION_DELAY 50
#define MENU_TRANSITION_TIME 200
#define MENU_TRANSITION_DISTANCE _SWIDTH

//##############################################################################

// =============================================================================
CMenuLink::CMenuLink(xuint iGroupIndex, CFontMetadata* pFont, const xchar* pText, t_OnClickCallback fpCallback) : CHyperlinkComponent(pFont),
	m_iGroupIndex(iGroupIndex),
	m_iElementIndex(0)
{
	m_iType = ElementType_MenuLink;
	m_fpOnClickCallback = fpCallback;

	SetText(pText);
}

// =============================================================================
void CMenuLink::RePosition(xuint iElementIndex, xuint iNumElements)
{
	m_iElementIndex = iElementIndex;
	m_xLinkPosition = xpoint(_HSWIDTH - (GetWidth() / 2), _HSHEIGHT - (iNumElements * (GetHeight() / 2)) + iElementIndex * (GetHeight() + 10));

	SetPosition(m_xLinkPosition);
}

//##############################################################################

// =============================================================================
void CMenuScreen::OnLoad()
{
	// Initialise the render resources.
	m_pBackground = new CBackgroundImage("Menu-Background");

	m_pMenuDefault = _FONT("Menu-Default");
	m_pMenuHighlight = _FONT("Menu-Highlighted");
	m_pMenuDisabled = _FONT("Menu-Disabled");

	// Initialise interface.
	InterfaceManager.SetCursor(_SPRITE("Cursor-Main"));
	InterfaceManager.SetCursor(ElementType_Button, _SPRITE("Cursor-Click"));
	InterfaceManager.SetCursor(ElementType_Input, _SPRITE("Cursor-Write"));

	// Initialise the menu links.
	CMenuLink* pLinkList[] = 
	{
		// Main
		new CMenuLink(MenuGroup_Main,		m_pMenuHighlight,	_LOCALE("Menu_Online"),			xbind(this, &CMenuScreen::Callback_ShowOnlineMenu)),
		new CMenuLink(MenuGroup_Main,		m_pMenuDefault,		_LOCALE("Menu_Offline"),		xbind(this, &CMenuScreen::Callback_ShowLevelsMenu)),
		new CMenuLink(MenuGroup_Main,		m_pMenuDisabled,	_LOCALE("Menu_Tutorial"),		NULL),
		new CMenuLink(MenuGroup_Main,		m_pMenuDisabled,	_LOCALE("Menu_Options"),		NULL),
		new CMenuLink(MenuGroup_Main,		m_pMenuDisabled,	_LOCALE("Menu_Credits"),		NULL),
		new CMenuLink(MenuGroup_Main,		m_pMenuDefault,		_LOCALE("Menu_Exit"),			xbind(this, &CMenuScreen::Callback_QuitGame)),

		// Online
		new CMenuLink(MenuGroup_Online,		m_pMenuDefault,		_LOCALE("Menu_OnlineJoin"),		xbind(this, &CMenuScreen::Callback_ShowJoinMenu)),
		new CMenuLink(MenuGroup_Online,		m_pMenuDefault,		_LOCALE("Menu_OnlineCreate"),	xbind(this, &CMenuScreen::Callback_ShowCreateMenu)),
		new CMenuLink(MenuGroup_Online,		m_pMenuHighlight,	_LOCALE("Menu_Back"),			xbind(this, &CMenuScreen::Callback_ShowMainMenu)),

		// Find
		new CMenuLink(MenuGroup_Join,		m_pMenuDefault,		_LOCALE("Menu_JoinPublic"),		xbind(this, &CMenuScreen::Callback_JoinPublic)),
		new CMenuLink(MenuGroup_Join,		m_pMenuDefault,		_LOCALE("Menu_JoinPrivate"),	xbind(this, &CMenuScreen::Callback_JoinPrivate)),
		new CMenuLink(MenuGroup_Join,		m_pMenuHighlight,	_LOCALE("Menu_Back"),			xbind(this, &CMenuScreen::Callback_ShowOnlineMenu)),

		// Create
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
void CMenuScreen::OnUnload()
{
	delete m_pBackground;
}

// =============================================================================
void CMenuScreen::OnActivate()
{
	// Initialise transition variables.
	m_iState = MenuState_None;
	m_iMenuGroup = MenuGroup_None;
	m_iPendingMenuGroup = MenuGroup_None;
	m_iLastMenuGroup = MenuGroup_Main;
	m_iNextScreen = ScreenIndex_Invalid;

	// Initialise the layers.
	m_xRenderView = new CRenderView(MenuLayerIndex_Max);

	m_xRenderView->GetLayer(MenuLayerIndex_Background)->AttachRenderable(m_pBackground);
	m_xRenderView->GetLayer(MenuLayerIndex_Interface)->SetRenderOverride(xbind(&InterfaceManager, &CInterfaceManager::Render));

	// Initialise the debug windows.
	m_pDebugWindow = (CWindowComponent*)ComponentFactory.CreateWindow(Global.m_pDebugMetadata, "Debug-RenderLayers");
	
	((CCheckComponent*)m_pDebugWindow->FindChild("L-Background"))->SetCheckBinding(xbind(m_xRenderView->GetLayer(MenuLayerIndex_Background), &CRenderLayer::SetEnabled));
	((CCheckComponent*)m_pDebugWindow->FindChild("L-Interface"))->SetCheckBinding(xbind(m_xRenderView->GetLayer(MenuLayerIndex_Interface), &CRenderLayer::SetEnabled));
	((CCheckComponent*)m_pDebugWindow->FindChild("L-InterfaceDebug"))->SetCheckBinding(xbind(&InterfaceManager, &CInterfaceManager::SetDebugRender));

	InterfaceScreen.Attach(m_pDebugWindow);
}

// =============================================================================
void CMenuScreen::OnDeactivate()
{
	InterfaceScreen.Detach(m_pDebugWindow);
	delete m_pDebugWindow;

	delete m_xRenderView;
	m_xRenderView = NULL;
}

// =============================================================================
void CMenuScreen::OnWake()
{
	RenderManager.SetRenderView(m_xRenderView);

	InterfaceManager.SetCursorVisible(true);

	// Show the last state.
	SetMenuGroup(m_iLastMenuGroup);
}

// =============================================================================
void CMenuScreen::OnSleep()
{
	InterfaceManager.SetCursorVisible(false);

	RenderManager.ClearRenderView();
}

// =============================================================================
xbool CMenuScreen::OnEvent(xint iEventType, void* pEventInfo)
{
	hgeInputEvent* pEvent = (hgeInputEvent*)pEventInfo;

	switch (iEventType)
	{
	case INPUT_KEYUP:
		{
			switch (pEvent->key)
			{
			// ESCAPE.
			case HGEK_ESCAPE:
				{
					// Exit the game.
					if (m_iState == MenuState_Idle)
					{
						switch (m_iMenuGroup)
						{
						case MenuGroup_Main:
							{
								_TERMINATE;
								return true;
							}
							break;
						}
					}
				}
				break;

			// F1.
			case HGEK_F1:
				{
					// Show/hide debug window.
					CWindowComponent* pWindow = (CWindowComponent*)InterfaceScreen.FindChild("Debug-RenderLayers");

					if (pWindow)
					{
						pWindow->SetVisible(!pWindow->IsVisible());
						return true;
					}
				}
				break;
			}
		}
		break;
	}

	return false;
}

// =============================================================================
void CMenuScreen::OnUpdate()
{
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
void CMenuScreen::OnPreRender()
{
}

// =============================================================================
void CMenuScreen::OnPostRender()
{
}

// =============================================================================
void CMenuScreen::SetState(t_MenuState iState)
{
	m_iState = iState;

	switch (iState)
	{
	case MenuState_None:
		{
			InterfaceScreen.SetEnabled(true);

			m_iLastMenuGroup = m_iMenuGroup;
			m_iMenuGroup = MenuGroup_None;

			ShowNextScreen();
		}
		break;

	case MenuState_TransitionIn:
		{
			InterfaceScreen.SetEnabled(false);
			InitTransition(true);
		}
		break;

	case MenuState_Idle:
		{
			InterfaceScreen.SetEnabled(true);
		}
		break;

	case MenuState_TransitionOut:
		{
			InterfaceScreen.SetEnabled(false);
			InitTransition(false);
		}
		break;
	}
}

// =============================================================================
void CMenuScreen::SetNextScreen(t_ScreenIndex iScreenIndex)
{
	m_iNextScreen = iScreenIndex;
	SetState(MenuState_TransitionOut);
}

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
void CMenuScreen::AttachMenuGroup(t_MenuGroup iMenuGroup)
{
	m_iMenuGroup = iMenuGroup;

	//InterfaceScreen.Clear();

	if (iMenuGroup != MenuGroup_None)
	{
		XEN_LIST_FOREACH(t_MenuLinkList, ppMenuLink, m_lpMenuLinks[iMenuGroup])
			InterfaceScreen.Attach(*ppMenuLink);
	}
}

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
			pMenuLink->SetPosition(xpoint(pMenuLink->m_xLinkPosition.m_tX, -pMenuLink->GetHeight()));
		else
			pMenuLink->m_bTransitionRight = !pMenuLink->m_bTransitionRight;
	}
}

// =============================================================================
void CMenuScreen::Callback_ShowMainMenu()
{
	SetMenuGroup(MenuGroup_Main);
}

// =============================================================================
void CMenuScreen::Callback_StartGame()
{
	CMenuLink* pLink = (CMenuLink*)InterfaceManager.GetActiveElement();

	Global.m_pActiveMap = MapManager.GetMap(pLink->m_iElementIndex);
	Global.m_pActiveMap->Load();

	SetNextScreen(ScreenIndex_GameScreen);
}

// =============================================================================
void CMenuScreen::Callback_QuitGame()
{
	_TERMINATE;
}

// =============================================================================
void CMenuScreen::Callback_ShowOnlineMenu()
{
	SetMenuGroup(MenuGroup_Online);
}

// =============================================================================
void CMenuScreen::Callback_ShowJoinMenu()
{
	SetMenuGroup(MenuGroup_Join);
}

// =============================================================================
void CMenuScreen::Callback_ShowCreateMenu()
{
	SetMenuGroup(MenuGroup_Create);
}

// =============================================================================
void CMenuScreen::Callback_ShowLevelsMenu()
{
	SetMenuGroup(MenuGroup_Levels);
}

// =============================================================================
void CMenuScreen::Callback_JoinPublic()
{
	SetNextScreen(ScreenIndex_LobbyScreen);
	m_iLobbyMode = LobbyStartMode_JoinPublic;
}

// =============================================================================
void CMenuScreen::Callback_JoinPrivate()
{
	SetNextScreen(ScreenIndex_LobbyScreen);
	m_iLobbyMode = LobbyStartMode_JoinPrivate;
}

// =============================================================================
void CMenuScreen::Callback_CreatePublic()
{
	SetNextScreen(ScreenIndex_LobbyScreen);
	m_iLobbyMode = LobbyStartMode_CreatePublic;
}

// =============================================================================
void CMenuScreen::Callback_CreatePrivate()
{
	SetNextScreen(ScreenIndex_LobbyScreen);
	m_iLobbyMode = LobbyStartMode_CreatePrivate;
}

//##############################################################################