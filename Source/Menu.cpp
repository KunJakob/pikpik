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

//##############################################################################
#pragma endregion

#pragma region Static
//##############################################################################
//
//                                   STATIC
//
//##############################################################################

// The next screen to process.
static t_ScreenIndex s_iNextScreen = ScreenIndex_Invalid;

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

#pragma region Callbacks
//##############################################################################
//
//                                 CALLBACKS
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         27-Apr-2008
// =============================================================================
void Callback_ShowMainMenu()
{
  _GLOBAL.pMenu->SetMenuGroup(MenuGroupIndex_Main);
}

// =============================================================================
// Nat Ryall                                                         27-Apr-2008
// =============================================================================
void Callback_ShowPlayMenu()
{
	_GLOBAL.pMenu->SetMenuGroup(MenuGroupIndex_Play);
}

// =============================================================================
// Nat Ryall                                                         27-Apr-2008
// =============================================================================
void Callback_ShowOnlineMenu()
{
	_GLOBAL.pMenu->SetMenuGroup(MenuGroupIndex_Online);
}

// =============================================================================
// Nat Ryall                                                          5-May-2008
// =============================================================================
void Callback_ShowJoinInterface()
{
  _GLOBAL.pMenu->SetMenuGroup(MenuGroupIndex_None);
  //_GLOBAL.pMenu->m_pJoinWindow->SetVisible(true);
}

// =============================================================================
// Nat Ryall                                                         27-Apr-2008
// =============================================================================
void Callback_StartLobby()
{
	s_iNextScreen = ScreenIndex_SplashScreen;
}

// =============================================================================
// Nat Ryall                                                         13-Apr-2008
// =============================================================================
void Callback_StartGame()
{
	s_iNextScreen = ScreenIndex_GameScreen;
}

// =============================================================================
// Nat Ryall                                                         13-Apr-2008
// =============================================================================
void Callback_QuitGame()
{
	_TERMINATE;
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

  // Initialise prerequisites.
  m_pFont = _FONT("Menu-Item");

  // Initialise interface.
  InterfaceManager.SetCursor(_SPRITE("Cursor-Main"));
  InterfaceManager.SetCursor(ElementType_MenuLink, _SPRITE("Cursor-Click"));

  // Initialise the menu links.
  m_iMenuGroup = MenuGroupIndex_None;

  CMenuLink* pLinkList[] = 
  {
    // Main.
    new CMenuLink(MenuGroupIndex_Main,		m_pFont, "Play Online",		&Callback_ShowOnlineMenu),
    new CMenuLink(MenuGroupIndex_Main,		m_pFont, "Tutorial",	    NULL),
    new CMenuLink(MenuGroupIndex_Main,		m_pFont, "Options",		    NULL),
    new CMenuLink(MenuGroupIndex_Main,		m_pFont, "Credits",		    NULL),
    new CMenuLink(MenuGroupIndex_Main,		m_pFont, "Exit",			    &Callback_QuitGame),

    // Play.
    new CMenuLink(MenuGroupIndex_Play,		m_pFont, "Back",			    &Callback_ShowMainMenu),

    // Online.
    new CMenuLink(MenuGroupIndex_Online,	m_pFont, "Join",			    &Callback_ShowJoinInterface),
    new CMenuLink(MenuGroupIndex_Online,	m_pFont, "Create",		    &Callback_StartGame),
    new CMenuLink(MenuGroupIndex_Online,	m_pFont, "Back",			    &Callback_ShowMainMenu),
  };

  for (XUINT iA = 0; iA < (sizeof(pLinkList) / sizeof(CMenuLink*)); ++iA)
  {
    m_lpMenuLinks[pLinkList[iA]->m_iGroupIndex].push_back(pLinkList[iA]);
    InterfaceManager.GetScreenElement()->Attach(pLinkList[iA]);
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

  // Initialise the render resources.
  m_pBackground = new CBackgroundImage("Lobby-Background");

  // Initialise statics.
  s_iNextScreen = ScreenIndex_Invalid;

  // Create windows.
  /*CFontMetadata* pFont = _FONT("Test-Input");

  m_pJoinWindow = new CWindow(_SPRITE("Test-Window"));
  m_pJoinWindow->SetInnerSize(280, 90);
  m_pJoinWindow->SetPosition(XPOINT(_HSWIDTH - (m_pJoinWindow->GetWidth() / 2), _HSHEIGHT - (m_pJoinWindow->GetHeight() / 2)));

  XPOINT xWinPos = m_pJoinWindow->GetInnerPosition();

  CLabel* pLabel = NULL;
  CButton* pButton = NULL;
  CInputBox* pInputBox = NULL;

  // Address Line
  pLabel = new CLabel(pFont);
  pLabel->SetPosition(xWinPos + XPOINT(13, 13));
  pLabel->SetText("Address:");
  m_pJoinWindow->Attach(pLabel);

  pInputBox = new CInputBox(_SPRITE("Test-InputBox"), pFont);
  pInputBox->SetPosition(xWinPos + XPOINT(83, 10));
  pInputBox->SetWidth(30);
  pInputBox->SetCharLimit(3);
  m_pJoinWindow->Attach(pInputBox);

  pInputBox = new CInputBox(_SPRITE("Test-InputBox"), pFont);
  pInputBox->SetPosition(xWinPos + XPOINT(133, 10));
  pInputBox->SetWidth(30);
  pInputBox->SetCharLimit(3);
  m_pJoinWindow->Attach(pInputBox);

  pInputBox = new CInputBox(_SPRITE("Test-InputBox"), pFont);
  pInputBox->SetPosition(xWinPos + XPOINT(183, 10));
  pInputBox->SetWidth(30);
  pInputBox->SetCharLimit(3);
  m_pJoinWindow->Attach(pInputBox);

  pInputBox = new CInputBox(_SPRITE("Test-InputBox"), pFont);
  pInputBox->SetPosition(xWinPos + XPOINT(233, 10));
  pInputBox->SetWidth(30);
  pInputBox->SetCharLimit(3);
  m_pJoinWindow->Attach(pInputBox);

  pLabel = new CLabel(pFont);
  pLabel->SetPosition(xWinPos + XPOINT(121, 13));
  pLabel->SetText(".");
  m_pJoinWindow->Attach(pLabel);

  pLabel = new CLabel(pFont);
  pLabel->SetPosition(xWinPos + XPOINT(171, 13));
  pLabel->SetText(".");
  m_pJoinWindow->Attach(pLabel);

  pLabel = new CLabel(pFont);
  pLabel->SetPosition(xWinPos + XPOINT(221, 13));
  pLabel->SetText(".");
  m_pJoinWindow->Attach(pLabel);

  // Port Line.
  pLabel = new CLabel(pFont);
  pLabel->SetPosition(xWinPos + XPOINT(13, 39));
  pLabel->SetText("Port:");
  m_pJoinWindow->Attach(pLabel);

  pInputBox = new CInputBox(_SPRITE("Test-InputBox"), pFont);
  pInputBox->SetPosition(xWinPos + XPOINT(83, 36));
  pInputBox->SetWidth(48);
  pInputBox->SetCharLimit(5);
  m_pJoinWindow->Attach(pInputBox);

  // Button Line.
  pButton = new CButton(_SPRITE("Test-Button"), pFont);
  pButton->SetPosition(xWinPos + XPOINT(82, 62));
  pButton->SetWidth(105);
  pButton->SetText("Join");
  m_pJoinWindow->Attach(pButton);

  pButton = new CButton(_SPRITE("Test-Button"), pFont);
  pButton->SetPosition(xWinPos + XPOINT(194, 62));
  pButton->SetWidth(75);
  pButton->SetText("Quit");
  m_pJoinWindow->Attach(pButton);

  InterfaceManager.GetContainer()->Attach(m_pJoinWindow);
  m_pJoinWindow->SetVisible(false);*/
}

// =============================================================================
// Nat Ryall                                                         11-Apr-2008
// =============================================================================
void CMenuScreen::Unload()
{
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

	if (s_iNextScreen != ScreenIndex_Invalid)
	{
		ScreenManager::Push(s_iNextScreen);
		s_iNextScreen = ScreenIndex_Invalid;
	}

	// Update the background.
	m_pBackground->Update();

  // Update the interface.
  InterfaceManager.Update();
}

// =============================================================================
// Nat Ryall                                                         13-Apr-2008
// =============================================================================
void CMenuScreen::Render()
{
	m_pBackground->Render();

  // Render the interface.
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

//##############################################################################
#pragma endregion