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

//##############################################################################
#pragma endregion

#pragma region Static
//##############################################################################
//
//                                   STATIC
//
//##############################################################################

// The font to render the links.
static hgeFont* s_pFont;

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
CMenuLink::CMenuLink(XUINT iGroupIndex, XUINT iElementIndex, XUINT iOrderIndex, const XCHAR* pText, t_fpLinkSelectedCallback fpCallback) 
:
	m_iGroupIndex(iGroupIndex),
	m_iOrderIndex(iOrderIndex),
	m_pText(pText),
	m_fpLinkSelectedCallback(fpCallback)
{
	id = iElementIndex;
	
	bStatic = false;
	bVisible = false;
	bEnabled = false;

	XFLOAT fX = (XFLOAT)_HSWIDTH;
	XFLOAT fY = (XFLOAT)_HSHEIGHT - 80.f + ((XFLOAT)iOrderIndex * 40.f);
	
	XFLOAT fHalfWidth = s_pFont->GetStringWidth(pText, false) / 2.f;
	XFLOAT fHeight = s_pFont->GetHeight();

	rect = hgeRect(fX - fHalfWidth, fY, fX + fHalfWidth, fY + fHeight);
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
	s_pFont->Render(rect.x1, rect.y1, HGETEXT_LEFT, m_pText);
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
	s_pFont = new hgeFont(".\\Font\\OneBlade.fnt");

	// Initialise the menu links.
	CMenuLink* pLinkList[] = 
	{
		// Main.
		new CMenuLink(MenuGroupIndex_Main, MenuElementIndex_Link_Play,		0, "Play Online",	&Callback_StartGame),
		new CMenuLink(MenuGroupIndex_Main, MenuElementIndex_Link_Options, 1, "Options",			NULL),
		new CMenuLink(MenuGroupIndex_Main, MenuElementIndex_Link_Credits, 2, "Credits",			NULL),
		new CMenuLink(MenuGroupIndex_Main, MenuElementIndex_Link_Exit,		3, "Exit",				&Callback_QuitGame),

		// Play.
		new CMenuLink(MenuGroupIndex_Play, MenuElementIndex_Link_Find,		0, "Find",				NULL),
		new CMenuLink(MenuGroupIndex_Play, MenuElementIndex_Link_Direct,	1, "Join",				NULL),
		new CMenuLink(MenuGroupIndex_Play, MenuElementIndex_Link_Create,	2, "Create",			NULL),
	};

	for (XUINT iA = 0; iA < (sizeof(pLinkList) / sizeof(CMenuLink*)); ++iA)
	{
		m_lpMenuLinks.push_back(pLinkList[iA]);
		m_pGUI->AddCtrl(pLinkList[iA]);
	}

	SetMenuGroup(MenuGroupIndex_Main);

	// Initialise the render resources.
	m_pBackground = ResourceManager::CreateSprite("Menu-Background");
	m_pBackground->SetArea("Screen");
	m_pBackground->SetPosition(XPOINT(0, 0), false);

	m_pCursor = ResourceManager::CreateSprite("Cursor");
	m_pGUI->SetCursor(m_pCursor->GetSurfaceTemplate()->pSurface);

	// Initialise statics.
	s_iNextScreen = ScreenIndex_Invalid;
}

// =============================================================================
// Nat Ryall                                                         11-Apr-2008
// =============================================================================
void CMenuScreen::Unload()
{
	delete s_pFont;
	delete m_pGUI;

	ResourceManager::FreeResource(m_pCursor);
	ResourceManager::FreeResource(m_pBackground);
}

// =============================================================================
// Nat Ryall                                                         13-Apr-2008
// =============================================================================
void CMenuScreen::Update()
{
	m_pGUI->Update(_TIMEDELTAF);

	if (s_iNextScreen != ScreenIndex_Invalid)
	{
		ScreenManager::Push(s_iNextScreen);
		s_iNextScreen = ScreenIndex_Invalid;
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
	XEN_LIST_FOREACH(t_MenuLinkList, ppMenuLink, m_lpMenuLinks)
	{
		XBOOL bEnable = (*ppMenuLink)->m_iGroupIndex == iMenuGroup;

		(*ppMenuLink)->bVisible = bEnable;
		(*ppMenuLink)->bEnabled = bEnable;
	}
}

//##############################################################################
#pragma endregion