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

//##############################################################################
#pragma endregion

#pragma region Static
//##############################################################################
//
//                                   STATIC
//
//##############################################################################

// The font to render the links.
static CShadowFont* s_pFont;

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
CMenuLink::CMenuLink(XUINT iGroupIndex, XUINT iElementIndex, const XCHAR* pText, t_fpLinkSelectedCallback fpCallback) 
:
	m_iGroupIndex(iGroupIndex),
	m_pText(pText),
	m_fpLinkSelectedCallback(fpCallback)
{
	id = iElementIndex;
	
	bStatic = false;
	bVisible = false;
	bEnabled = false;

	rect = hgeRect(0, 0, 0, 0);
}

// =============================================================================
// Nat Ryall                                                         13-Apr-2008
// =============================================================================
CMenuLink::~CMenuLink()
{
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
	s_pFont->SetShadowColor(0xFF000000);
	s_pFont->SetShadowOffset(XPOINT(3, 3));

	if (m_fpLinkSelectedCallback)
		s_pFont->SetColor(0xFFFFFFFF);
	else
		s_pFont->SetColor(0xFF808080);

	s_pFont->RenderShadowed(rect.x1, rect.y1, HGETEXT_LEFT, m_pText);
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
	XFLOAT fHalfWidth = s_pFont->GetStringWidth(m_pText, false) / 2.f;
	XFLOAT fHeight = s_pFont->GetHeight();
	XFLOAT fLineHeight = (XFLOAT)(XUINT)(fHeight * 1.25f);
	XFLOAT fHalfLineHeight = fLineHeight / 2.f;

	XFLOAT fX = (XFLOAT)_HSWIDTH;
	XFLOAT fY = (XFLOAT)_HSHEIGHT - (iNumElements * fHalfLineHeight) + ((XFLOAT)iElementIndex * fLineHeight);

	rect = hgeRect(fX - fHalfWidth, fY, fX + fHalfWidth, fY + fHeight);
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

// =============================================================================
// Nat Ryall                                                         18-Apr-2008
// =============================================================================
void Callback_ShowCharacterSelect()
{
	s_iNextScreen = ScreenIndex_SelectionScreen;
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
	// Initialise prerequisites.
	m_pGUI = new hgeGUI;
	s_pFont = new CShadowFont(".\\Font\\ShowcardGothic_36.fnt");

	// Initialise the menu links.
	m_iMenuGroup = MenuGroupIndex_None;

	CMenuLink* pLinkList[] = 
	{
		// Main.
		new CMenuLink(MenuGroupIndex_Main, MenuElementIndex_Link_Play,		"Play Online",	&Callback_StartGame),
		new CMenuLink(MenuGroupIndex_Main, MenuElementIndex_Link_Credits, "Tutorial",			NULL),
		new CMenuLink(MenuGroupIndex_Main, MenuElementIndex_Link_Options, "Options",			NULL),
		new CMenuLink(MenuGroupIndex_Main, MenuElementIndex_Link_Credits, "Credits",			NULL),
		new CMenuLink(MenuGroupIndex_Main, MenuElementIndex_Link_Exit,		"Exit",					&Callback_QuitGame),
		new CMenuLink(MenuGroupIndex_Main, MenuElementIndex_Link_Debug,		"Debug",				&Callback_ShowCharacterSelect),

		// Play.
		new CMenuLink(MenuGroupIndex_Play, MenuElementIndex_Link_Find,		"Find",					NULL),
		new CMenuLink(MenuGroupIndex_Play, MenuElementIndex_Link_Direct,	"Join",					NULL),
		new CMenuLink(MenuGroupIndex_Play, MenuElementIndex_Link_Create,	"Create",				NULL),
	};

	for (XUINT iA = 0; iA < (sizeof(pLinkList) / sizeof(CMenuLink*)); ++iA)
	{
		m_lpMenuLinks[pLinkList[iA]->m_iGroupIndex].push_back(pLinkList[iA]);
		m_pGUI->AddCtrl(pLinkList[iA]);
	}

	for (XUINT iGroup = 0; iGroup < MenuGroupIndex_Max; ++iGroup)
	{
		XUINT iElementCount = m_lpMenuLinks[iGroup].size();
		XUINT iElement = 0;

		XEN_LIST_FOREACH(t_MenuLinkList, ppMenuLink, m_lpMenuLinks[iGroup])
			(*ppMenuLink)->RePosition(iElement++, iElementCount);
	}

	SetMenuGroup(MenuGroupIndex_Main);

	// Initialise the render resources.
	m_pBackground = ResourceManager::CreateSprite("Menu-Background");
	m_pBackground->SetArea("Screen");
	m_pBackground->SetPosition(XPOINT(0, 0), false);

	m_pCursor = ResourceManager::CreateSprite("Cursor");
	m_pGUI->SetCursor(m_pCursor->GetSurfaceTemplate()->pSurface);

	// Initialise the background scroll.
	m_iTimer = 0;
	m_xOffset.Reset();

	// Initialise statics.
	s_iNextScreen = ScreenIndex_Invalid;
}

// =============================================================================
// Nat Ryall                                                         11-Apr-2008
// =============================================================================
void CMenuScreen::Unload()
{
	delete s_pFont;
	delete m_pGUI; // Also erases the GUI elements.

	ResourceManager::FreeResource(m_pCursor);
	ResourceManager::FreeResource(m_pBackground);
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

	// Update the GUI.
	m_pGUI->Update(_TIMEDELTAF);

	if (s_iNextScreen != ScreenIndex_Invalid)
	{
		ScreenManager::Push(s_iNextScreen);
		s_iNextScreen = ScreenIndex_Invalid;
	}

	// Scroll the background.
	m_iTimer += _TIMEDELTA;

	if (m_iTimer > 25)
	{
		m_iTimer %= 25;

		m_xOffset += XPOINT(1, 1);
		m_xOffset %= 64;
		
		m_pBackground->SetPosition(m_xOffset * -1, false);
	}
}

// =============================================================================
// Nat Ryall                                                         13-Apr-2008
// =============================================================================
void CMenuScreen::Render()
{
	m_pBackground->Render();
	m_pGUI->Render();
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