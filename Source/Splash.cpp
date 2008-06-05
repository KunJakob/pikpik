#pragma region Include
//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Global.
#include <Global.h>

// Local.
#include <Splash.h>

// Other.
#include <Renderer.h>
#include <Font.h>

//##############################################################################
#pragma endregion

#pragma region Logo
//##############################################################################
//
//                                     LOGO
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                          6-Apr-2008
// =============================================================================
void CLogoScreen::Load()
{
	m_pImage = new CSprite(_SPRITE("Logo-SAPIAN"));

	m_pImage->SetAnchor(m_pImage->GetAreaCentre());
	m_pImage->SetPosition(XPOINT(_HSWIDTH, _HSHEIGHT));
}

// =============================================================================
// Nat Ryall                                                          6-Apr-2008
// =============================================================================
void CLogoScreen::Unload()
{
	delete m_pImage;
}

// =============================================================================
// Nat Ryall                                                          6-Apr-2008
// =============================================================================
void CLogoScreen::Wake()
{
	CFadeScreen::Reset();
}

// =============================================================================
// Nat Ryall                                                          6-Apr-2008
// =============================================================================
void CLogoScreen::Event(xuint iEventType, void* pEventInfo)
{
	switch (iEventType)
	{
	case INPUT_MBUTTONUP:
	case INPUT_KEYUP:
		{
			OnFadeComplete();
		}
		break;
	}
}

// =============================================================================
// Nat Ryall                                                          6-Apr-2008
// =============================================================================
void CLogoScreen::Update()
{
	CFadeScreen::Update();

	m_pImage->SetAlpha(m_fAlpha);
}

// =============================================================================
// Nat Ryall                                                          6-Apr-2008
// =============================================================================
void CLogoScreen::Render()
{
	xfloat fColour = m_fAlpha * .08f;
	_HGE->Gfx_Clear(ARGBF(1.f, fColour, fColour, fColour));

	m_pImage->Render();
}

// =============================================================================
// Nat Ryall                                                          4-Jun-2008
// =============================================================================
void CLogoScreen::OnFadeComplete()
{
	_GLOBAL.iNextScreen = ScreenIndex_WarningScreen;
}

//##############################################################################
#pragma endregion

#pragma region Warning
//##############################################################################
//
//                                   WARNING
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                          6-Apr-2008
// =============================================================================
void CWarningScreen::Event(xuint iEventType, void* pEventInfo)
{
	switch (iEventType)
	{
	case INPUT_MBUTTONUP:
	case INPUT_KEYUP:
		{
			OnFadeComplete();
		}
		break;
	}
}

// =============================================================================
// Nat Ryall                                                          4-Jun-2008
// =============================================================================
void CWarningScreen::Render()
{
	xfloat fColour = m_fAlpha * .08f;
	_HGE->Gfx_Clear(ARGBF(1.f, fColour, fColour, fColour));

	static xrect s_xRect(_HSWIDTH - 250, 0, _HSWIDTH + 250, _SHEIGHT);

	_GLOBAL.pGameFont->Render
	(
		_LOCALE("Splash_PhotoWarning"),
		s_xRect,
		HGETEXT_CENTER | HGETEXT_MIDDLE,
		m_fAlpha
	);
}

// =============================================================================
// Nat Ryall                                                          4-Jun-2008
// =============================================================================
void CWarningScreen::OnFadeComplete()
{
	_GLOBAL.iNextScreen = ScreenIndex_MenuScreen;
}

//##############################################################################
#pragma endregion