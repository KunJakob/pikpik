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
		new CMenuLink(MenuGroupIndex_Online,	m_pMenuDefault,		_LOCALE("Menu_OnlineJoin"),		NULL),
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

	SetMenuGroup(MenuGroupIndex_Main);

	// Initialise statics.
	m_iNextScreen = ScreenIndex_Invalid;

	// Interface.
	/*CWindowComponent* pWindow = new CWindowComponent(_SPRITE("Test-Window"), _FONT("Test-WindowFont"));
	pWindow->SetInnerSize(200, 300);
	pWindow->SetPosition(xpoint(10, 10));
	pWindow->SetMoveable(true);
	pWindow->SetTitle("Window #1");

	CButtonComponent* pButton = new CButtonComponent(_SPRITE("Test-Button"), _FONT("Test-Font"));
	pButton->SetWidth(190);
	pButton->SetPosition(pWindow->GetInnerPosition() + xpoint(5, 5));
	pButton->SetText("Reset Position");
	pButton->SetClickCallback(xbind(this, &CMenuScreen::Debug_HideWindow));

	CInputComponent* pInput = new CInputComponent(_SPRITE("Test-Input"), _FONT("Test-Font"));
	pInput->SetWidth(190);
	pInput->SetPosition(pWindow->GetInnerPosition() + xpoint(5, 35));
	pInput->SetMasked(false);

	CCheckComponent* pCheck = new CCheckComponent(_SPRITE("Test-Check"), _FONT("Test-Font"));
	pCheck->SetPosition(pWindow->GetInnerPosition() + xpoint(5, 65));
	pCheck->SetText("Mask Input");

	CRadioComponent* pRadio[7] = 
	{
		new CRadioComponent(0, _SPRITE("Test-Radio"), _FONT("Test-Font")),
		new CRadioComponent(0, _SPRITE("Test-Radio"), _FONT("Test-Font")),
		new CRadioComponent(0, _SPRITE("Test-Radio"), _FONT("Test-Font")),
		new CRadioComponent(0, _SPRITE("Test-Radio"), _FONT("Test-Font")),
		new CRadioComponent(1, _SPRITE("Test-Radio"), _FONT("Test-Font")),
		new CRadioComponent(1, _SPRITE("Test-Radio"), _FONT("Test-Font")),
		new CRadioComponent(1, _SPRITE("Test-Radio"), _FONT("Test-Font")),
	};

	pRadio[0]->SetPosition(pWindow->GetInnerPosition() + xpoint(5, 95));
	pRadio[0]->SetText("A1");

	pRadio[1]->SetPosition(pWindow->GetInnerPosition() + xpoint(55, 95));
	pRadio[1]->SetText("A2");

	pRadio[2]->SetPosition(pWindow->GetInnerPosition() + xpoint(105, 95));
	pRadio[2]->SetText("A3");

	pRadio[3]->SetPosition(pWindow->GetInnerPosition() + xpoint(155, 95));
	pRadio[3]->SetText("A4");

	pRadio[4]->SetPosition(pWindow->GetInnerPosition() + xpoint(5, 125));
	pRadio[4]->SetText("B1");

	pRadio[5]->SetPosition(pWindow->GetInnerPosition() + xpoint(55, 125));
	pRadio[5]->SetText("B2");

	pRadio[6]->SetPosition(pWindow->GetInnerPosition() + xpoint(105, 125));
	pRadio[6]->SetText("B3");
	pRadio[6]->m_pExtendedData = (void*)"SECRET";

	CProgressComponent* pProgress = new CProgressComponent(_SPRITE("Test-Progress"));
	pProgress->SetWidth(190);
	pProgress->SetPosition(pWindow->GetInnerPosition() + xpoint(5, 155));

	CGroupComponent* pGroup = new CGroupComponent(_SPRITE("Test-Group"), _FONT("Test-GroupFont"));
	pGroup->SetSize(190, 100);
	pGroup->SetPosition(pWindow->GetInnerPosition() + xpoint(5, 185));
	pGroup->SetTitle("Connection Info");

	pWindow->Attach(pButton);
	pWindow->Attach(pInput);
	pWindow->Attach(pCheck);
	pWindow->Attach(pRadio[0]);
	pWindow->Attach(pRadio[1]);
	pWindow->Attach(pRadio[2]);
	pWindow->Attach(pRadio[3]);
	pWindow->Attach(pRadio[4]);
	pWindow->Attach(pRadio[5]);
	pWindow->Attach(pRadio[6]);
	pWindow->Attach(pProgress);
	pWindow->Attach(pGroup);*/

	//InterfaceManager.GetScreen()->Attach(pWindow);

	/*m_pInput = pInput;
	m_pCheck = pCheck;
	m_pProgress = pProgress;*/
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
// Nat Ryall                                                         13-Apr-2008
// =============================================================================
void CMenuScreen::Update()
{
	// Allow "ESC" to exit the game.
	if (_HGE->Input_KeyDown(HGEK_ESCAPE))
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

	// DEBUG.
	/*if (CRadioComponent* pRadio = CRadioComponent::GetChecked(1))
	{
	if (pRadio->m_pExtendedData && stricmp((const char*)pRadio->m_pExtendedData, "SECRET") == 0)
	{
	m_pInput->SetText("You found the SECRET!");
	m_pCheck->SetChecked(false);
	}
	}*/

	//m_pInput->SetMasked(m_pCheck->IsChecked());
	//m_pProgress->SetProgress(m_pProgress->GetProgress() + 0.001f);
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
void CMenuScreen::Callback_ShowJoinInterface()
{
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