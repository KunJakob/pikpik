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
#include <Component.h>

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
CMenuLink::CMenuLink(xuint iGroupIndex, CFontMetadata* pFont, const xchar* pText, t_OnClickCallback fpCallback) : CHyperlinkComponent(pFont),
	m_iGroupIndex(iGroupIndex),
	m_iElementIndex(0)
{
	m_iType = ElementType_MenuLink;
	m_fpOnClickCallback = fpCallback;

	SetText(pText);
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
	m_pMenuDisabled = _FONT("Menu-Disabled");

	// Initialise interface.
	InterfaceManager.SetCursor(_SPRITE("Cursor-Main"));
	InterfaceManager.SetCursor(ElementType_Button, _SPRITE("Cursor-Click"));
	InterfaceManager.SetCursor(ElementType_Input, _SPRITE("Cursor-Write"));

	// Initialise the menu links.
	CMenuLink* pLinkList[] = 
	{
		// Main.
		new CMenuLink(MenuGroup_Main,		m_pMenuHighlight,	_LOCALE("Menu_Offline"),		xbind(this, &CMenuScreen::Callback_StartGame)),
		new CMenuLink(MenuGroup_Main,		m_pMenuDisabled,	_LOCALE("Menu_Online"),			NULL),
		new CMenuLink(MenuGroup_Main,		m_pMenuDefault,		_LOCALE("Menu_Tutorial"),		NULL),
		new CMenuLink(MenuGroup_Main,		m_pMenuDefault,		_LOCALE("Menu_Options"),		NULL),
		new CMenuLink(MenuGroup_Main,		m_pMenuDefault,		_LOCALE("Menu_Credits"),		NULL),
		new CMenuLink(MenuGroup_Main,		m_pMenuDefault,		_LOCALE("Menu_Exit"),			xbind(this, &CMenuScreen::Callback_QuitGame)),
	};

	for (xuint iA = 0; iA < (sizeof(pLinkList) / sizeof(CMenuLink*)); ++iA)
		m_lpMenuLinks[pLinkList[iA]->m_iGroupIndex].push_back(pLinkList[iA]);

	for (xuint iGroup = 0; iGroup < MenuGroup_Max; ++iGroup)
	{
		xuint iElement = 0;

		XEN_LIST_FOREACH(t_MenuLinkList, ppMenuLink, m_lpMenuLinks[iGroup])
			(*ppMenuLink)->RePosition(iElement++, (xuint)m_lpMenuLinks[iGroup].size());
	}
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

	// Initialise the layers.
	RenderManager.ReinitLayers(MenuLayerIndex_Max);

	RenderLayer(MenuLayerIndex_Background)->AttachRenderable(m_pBackground);
	RenderLayer(MenuLayerIndex_Interface)->SetRenderOverride(xbind(&InterfaceManager, &CInterfaceManager::Render));

	// Initialise the debug windows.
	CWindowComponent* pWindow = (CWindowComponent*)ComponentFactory.CreateWindow(Global.m_pDebugMetadata, "Debug-RenderLayers");
	
	InterfaceScreen.Attach(pWindow);

	((CCheckComponent*)pWindow->FindChild("L-Background"))->SetCheckBinding(xbind(RenderLayer(MenuLayerIndex_Background), &CRenderLayer::SetEnabled));
	((CCheckComponent*)pWindow->FindChild("L-Interface"))->SetCheckBinding(xbind(RenderLayer(MenuLayerIndex_Interface), &CRenderLayer::SetEnabled));
	((CCheckComponent*)pWindow->FindChild("L-InterfaceDebug"))->SetCheckBinding(xbind(&InterfaceManager, &CInterfaceManager::SetDebugRender));
}

// =============================================================================
// Nat Ryall                                                         11-Apr-2008
// =============================================================================
void CMenuScreen::OnDeactivate()
{
	RenderManager.ClearLayers();
}

// =============================================================================
// Nat Ryall                                                         09-Jun-2008
// =============================================================================
void CMenuScreen::OnWake()
{
	SetMenuGroup(m_iLastMenuGroup);
}

// =============================================================================
// Nat Ryall                                                         09-Jun-2008
// =============================================================================
void CMenuScreen::OnSleep()
{
}

// =============================================================================
// Nat Ryall                                                         06-Nov-2008
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
// Nat Ryall                                                         13-Apr-2008
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
void CMenuScreen::OnPreRender()
{
}

// =============================================================================
// Nat Ryall                                                         13-Apr-2008
// =============================================================================
void CMenuScreen::OnPostRender()
{
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

	//InterfaceScreen.Clear();

	if (iMenuGroup != MenuGroup_None)
	{
		XEN_LIST_FOREACH(t_MenuLinkList, ppMenuLink, m_lpMenuLinks[iMenuGroup])
			InterfaceScreen.Attach(*ppMenuLink);
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
			pMenuLink->SetPosition(xpoint(pMenuLink->m_xLinkPosition.m_tX, -pMenuLink->GetHeight()));
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
// Nat Ryall                                                         13-Apr-2008
// =============================================================================
void CMenuScreen::Callback_StartGame()
{
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