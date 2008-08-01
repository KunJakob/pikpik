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

//##############################################################################
//
//                                     LOGO
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                          6-Apr-2008
// =============================================================================
void CLogoScreen::OnActivate()
{
	m_pImage = new CSprite(_SPRITE("Logo-SAPIAN"));

	m_pImage->SetAnchor(m_pImage->GetAreaCentre());
	m_pImage->SetPosition(xpoint(_HSWIDTH, _HSHEIGHT));
}

// =============================================================================
// Nat Ryall                                                          6-Apr-2008
// =============================================================================
void CLogoScreen::OnDeactivate()
{
	delete m_pImage;
}

// =============================================================================
// Nat Ryall                                                          6-Apr-2008
// =============================================================================
void CLogoScreen::OnWake()
{
	CFadeScreen::Reset();
}

// =============================================================================
// Nat Ryall                                                          6-Apr-2008
// =============================================================================
void CLogoScreen::OnNotify(xuint iEventType, void* pEventInfo)
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
void CLogoScreen::OnUpdate()
{
	CFadeScreen::OnUpdate();

	m_pImage->SetAlpha(m_fAlpha);
}

// =============================================================================
// Nat Ryall                                                          6-Apr-2008
// =============================================================================
void CLogoScreen::OnRender()
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
	//Global.m_iNextScreen = ScreenIndex_WarningScreen;
	ScreenManager.Set(ScreenIndex_WarningScreen);
}

//##############################################################################

//##############################################################################
//
//                                   WARNING
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                          6-Apr-2008
// =============================================================================
void CWarningScreen::OnNotify(xuint iEventType, void* pEventInfo)
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
void CWarningScreen::OnRender()
{
	xfloat fColour = m_fAlpha * .08f;
	_HGE->Gfx_Clear(ARGBF(1.f, fColour, fColour, fColour));

	static xrect s_xRect(_HSWIDTH - 250, 0, _HSWIDTH + 250, _SHEIGHT);

	Global.m_pGameFont->Render
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
	//Global.m_iNextScreen = ScreenIndex_MenuScreen;
	ScreenManager.Set(ScreenIndex_MenuScreen);
}

//##############################################################################