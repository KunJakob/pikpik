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
CMenuLink::CMenuLink(XUINT iGroupIndex, CFontMetadata* pFont, const XCHAR* pText, t_fpLinkSelectedCallback fpCallback) :
	m_iGroupIndex(iGroupIndex),
	m_pText(NULL),
	m_fpLinkSelectedCallback(fpCallback)
{
  id = 0;
	
	bStatic = false;
	bVisible = false;
	bEnabled = false;

	rect = hgeRect(0, 0, 0, 0);

	m_pText = new CText(pFont);
	m_pText->SetString(pText);
	m_pText->SetAlignment(HGETEXT_CENTER);
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
void CMenuLink::Update(float fDeltaTime)
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
// Nat Ryall                                                         13-Apr-2008
// =============================================================================
bool CMenuLink::MouseLButton(bool bDown)
{
	if (bDown)
		ExecuteCallback();

	return false;
}

// =============================================================================
// Nat Ryall                                                         18-Apr-2008
// =============================================================================
void CMenuLink::RePosition(XUINT iElementIndex, XUINT iNumElements)
{
	XFLOAT fHalfWidth = (XFLOAT)m_pText->GetStringWidth() / 2.f;
	XFLOAT fHeight = (XFLOAT)m_pText->GetFontHeight();
	XFLOAT fLineHeight = (XFLOAT)(XUINT)(fHeight * 1.25f);
	XFLOAT fHalfLineHeight = fLineHeight / 2.f;

	XFLOAT fX = (XFLOAT)_HSWIDTH;
	XFLOAT fY = (XFLOAT)_HSHEIGHT - (iNumElements * fHalfLineHeight) + ((XFLOAT)iElementIndex * fLineHeight);

	rect = hgeRect(fX - fHalfWidth, fY, fX + fHalfWidth, fY + fHeight);
	
	m_pText->SetPosition(XPOINT((XUINT)fX, (XUINT)fY));
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

  // Initialise the menu links.
  m_iMenuGroup = MenuGroupIndex_None;

  CMenuLink* pLinkList[] = 
  {
    // Main.
    new CMenuLink(MenuGroupIndex_Main,		m_pFont, "Play Online",		&Callback_ShowOnlineMenu),
    new CMenuLink(MenuGroupIndex_Main,		m_pFont, "Tutorial",	NULL),
    new CMenuLink(MenuGroupIndex_Main,		m_pFont, "Options",		NULL),
    new CMenuLink(MenuGroupIndex_Main,		m_pFont, "Credits",		NULL),
    new CMenuLink(MenuGroupIndex_Main,		m_pFont, "Exit",			&Callback_QuitGame),

    // Play.
    new CMenuLink(MenuGroupIndex_Play,		m_pFont, "Back",			&Callback_ShowMainMenu),

    // Online.
    new CMenuLink(MenuGroupIndex_Online,	m_pFont, "Join",			NULL),
    new CMenuLink(MenuGroupIndex_Online,	m_pFont, "Create",		Callback_StartGame),
    new CMenuLink(MenuGroupIndex_Online,	m_pFont, "Back",			&Callback_ShowMainMenu),
  };

  /*for (XUINT iA = 0; iA < (sizeof(pLinkList) / sizeof(CMenuLink*)); ++iA)
  {
    m_lpMenuLinks[pLinkList[iA]->m_iGroupIndex].push_back(pLinkList[iA]);
    m_pGUI->AddCtrl(pLinkList[iA]);
  }*/

  for (XUINT iGroup = 0; iGroup < MenuGroupIndex_Max; ++iGroup)
  {
    XUINT iElementCount = m_lpMenuLinks[iGroup].size();
    XUINT iElement = 0;

    XEN_LIST_FOREACH(t_MenuLinkList, ppMenuLink, m_lpMenuLinks[iGroup])
      (*ppMenuLink)->RePosition(iElement++, iElementCount);
  }

  SetMenuGroup(MenuGroupIndex_Main);

  // Initialise the render resources.
  m_pBackground = new CBackgroundImage("Lobby-Background");

  // Initialise statics.
  s_iNextScreen = ScreenIndex_Invalid;

  // Debug.
  /*CWindow* m_pWindow = new CWindow(_SPRITE("Test-Window"));
  m_pWindow->SetPosition(XPOINT(20, 20));
  m_pWindow->SetInnerSize(200, 200);

  CInputBox* pInputBox = new CInputBox(_SPRITE("Test-InputBox"), _FONT("Test-Input"));
  pInputBox->SetPosition(m_pWindow->GetInnerPosition() + XPOINT(5, 5));
  pInputBox->SetWidth(190);
  pInputBox->SetCharLimit(16);
  m_pWindow->Attach(pInputBox);

  pInputBox = new CInputBox(_SPRITE("Test-InputBox"), _FONT("Test-Input"));
  pInputBox->SetPosition(m_pWindow->GetInnerPosition() + XPOINT(5, 28));
  pInputBox->SetWidth(190);
  pInputBox->SetText("Welcome to PMO.");
  m_pWindow->Attach(pInputBox);

  InterfaceManager.GetContainer()->Attach(m_pWindow);

	m_pWindow = new CWindow(_SPRITE("Test-Window"));
	m_pWindow->SetPosition(XPOINT(_HSWIDTH, _HSHEIGHT));
	m_pWindow->SetInnerSize(200, 200);

	CButton* m_pButton = new CButton(_SPRITE("Test-Button"), _FONT("Test-Input"));
	m_pButton->SetPosition(m_pWindow->GetInnerPosition() + XPOINT(5, 5));
  m_pButton->SetWidth(100);
  m_pButton->SetText("Continue");
	m_pWindow->Attach(m_pButton);

	m_pButton = new CButton(_SPRITE("Test-Button"));
	m_pButton->SetPosition(m_pWindow->GetInnerPosition() + XPOINT(5, 5 + m_pButton->GetHeight()));
	m_pWindow->Attach(m_pButton);

	InterfaceManager.GetContainer()->Attach(m_pWindow);

  InterfaceManager.SetCursor(_SPRITE("Test-Cursor"));*/
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
		{
			(*ppMenuLink)->bVisible = false;
			(*ppMenuLink)->bEnabled = false;
		}
	}

	XEN_LIST_FOREACH(t_MenuLinkList, ppMenuLink, m_lpMenuLinks[iMenuGroup])
	{
		(*ppMenuLink)->bVisible = true;
		(*ppMenuLink)->bEnabled = true;
	}

	m_iMenuGroup = iMenuGroup;
}

//##############################################################################
#pragma endregion