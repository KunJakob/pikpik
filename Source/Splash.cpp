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

	_FMOD->createStream("Audio\\Clips\\Logo-SAPIAN.mp3", FMOD_SOFTWARE, NULL, &m_pSound);
}

// =============================================================================
// Nat Ryall                                                          6-Apr-2008
// =============================================================================
void CLogoScreen::OnDeactivate()
{
	delete m_pImage;

	m_pChannel->stop();
	m_pSound->release();
}

// =============================================================================
// Nat Ryall                                                          6-Apr-2008
// =============================================================================
void CLogoScreen::OnWake()
{
	CFadeScreen::Reset();

	_FMOD->playSound(FMOD_CHANNEL_FREE, m_pSound, false, &m_pChannel);
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

#define FLASH_START 3200
#define FLASH_MID 3500
#define FLASH_END 4200
#define FLASH_MAX 50

	xuint iTrackTime;
	m_pChannel->getPosition(&iTrackTime, FMOD_TIMEUNIT_MS);

	if (iTrackTime > FLASH_START)
	{
		xuint iColour = 0;

		if (iTrackTime < FLASH_END)
		{
			if (iTrackTime < FLASH_MID)
			{
				xuint iTimeOffset = iTrackTime - FLASH_START;
				iColour = XINTPERCENT(iTimeOffset, FLASH_MAX, FLASH_MID - FLASH_START);
			}
			else
			{
				xuint iTimeOffset = iTrackTime - FLASH_MID;
				iColour = FLASH_MAX - XINTPERCENT(iTimeOffset, FLASH_MAX, FLASH_END - FLASH_MID);
			}
		}

		m_pImage->GetMetadata()->GetSprite()->SetBlendMode(BLEND_COLORADD);
		m_pImage->GetMetadata()->GetSprite()->SetColor(ARGB(255, iColour, iColour, iColour));
	}
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