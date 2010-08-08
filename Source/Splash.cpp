//##############################################################################

// Global.
#include <Global.h>

// Local.
#include <Splash.h>

// Other.
#include <Renderer.h>
#include <Font.h>
#include <Sound.h>

//##############################################################################

// =============================================================================
void CLogoScreen::OnActivate()
{
	m_pImage = new CSprite(_SPRITE("Logo-SAPIAN"));

	m_pImage->SetAnchor(m_pImage->GetAreaCentre());
	m_pImage->SetPosition(xpoint(_HSWIDTH, _HSHEIGHT));

	m_pSound = new CSound(_SOUND("Logo-Intro"));
}

// =============================================================================
void CLogoScreen::OnDeactivate()
{
	delete m_pImage;
	delete m_pSound;
}

// =============================================================================
void CLogoScreen::OnWake()
{
	CFadeScreen::Reset();

	m_pSound->Play();

	RenderManager.SetRenderOverride(xbind(this, &CLogoScreen::Render));
}

// =============================================================================
void CLogoScreen::OnSleep()
{
	RenderManager.SetRenderOverride(NULL);
}

// =============================================================================
xbool CLogoScreen::OnEvent(xint iEventType, void* pEventInfo)
{
	switch (iEventType)
	{
	case INPUT_MBUTTONUP:
	case INPUT_KEYUP:
		{
			OnFadeComplete();
			return true;
		}
		break;
	}

	return false;
}

// =============================================================================
void CLogoScreen::OnUpdate()
{
	CFadeScreen::OnUpdate();

	static xuint s_iHideTime	= m_pSound->GetMarkerTime("Display");
	static xuint s_iFadeStart	= m_pSound->GetMarkerTime("FadeStart");
	static xuint s_iFadePeak	= m_pSound->GetMarkerTime("FadePeak");
	static xuint s_iFadeEnd		= m_pSound->GetMarkerTime("FadeEnd");
	static xuint s_iFadePower	= 40;

	xuint iTrackTime = m_pSound->GetTime();
	xuint iColour = 0;

	m_pImage->SetAlpha((iTrackTime > s_iHideTime) ? m_fAlpha : 0.f);

	if (iTrackTime > s_iFadeStart)
	{
		if (iTrackTime < s_iFadeEnd)
		{
			if (iTrackTime < s_iFadePeak)
			{
				xuint iTimeOffset = iTrackTime - s_iFadeStart;
				iColour = XINTPERCENT(iTimeOffset, s_iFadePeak - s_iFadeStart, s_iFadePower);
			}
			else
			{
				xuint iTimeOffset = iTrackTime - s_iFadePeak;
				iColour = s_iFadePower - XINTPERCENT(iTimeOffset, s_iFadeEnd - s_iFadePeak, s_iFadePower);
			}
		}
	}

	m_pImage->GetMetadata()->GetSprite()->SetBlendMode(BLEND_COLORADD);
	m_pImage->GetMetadata()->GetSprite()->SetColor(ARGB(255, iColour, iColour, iColour));
}

// =============================================================================
void CLogoScreen::Render()
{
	xfloat fColour = m_fAlpha * .08f;
	_HGE->Gfx_Clear(_ARGBF(1.f, fColour, fColour, fColour));

	m_pImage->Render();
}

// =============================================================================
void CLogoScreen::OnFadeComplete()
{
	ScreenManager.Set(ScreenIndex_WarningScreen);
}

//##############################################################################

// =============================================================================
void CWarningScreen::OnWake()
{
	RenderManager.SetRenderOverride(xbind(this, &CWarningScreen::Render));
}

// =============================================================================
void CWarningScreen::OnSleep()
{
	RenderManager.SetRenderOverride(NULL);
}

// =============================================================================
xbool CWarningScreen::OnEvent(xint iEventType, void* pEventInfo)
{
	switch (iEventType)
	{
	case INPUT_MBUTTONUP:
	case INPUT_KEYUP:
		{
			OnFadeComplete();
			return true;
		}
		break;
	}

	return false;
}

// =============================================================================
void CWarningScreen::Render()
{
	xfloat fColour = m_fAlpha * .08f;
	_HGE->Gfx_Clear(_ARGBF(1.f, fColour, fColour, fColour));

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
void CWarningScreen::OnFadeComplete()
{
	ScreenManager.Set(ScreenIndex_MenuScreen);
}

//##############################################################################