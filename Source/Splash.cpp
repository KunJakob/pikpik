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

//##############################################################################
#pragma endregion

#pragma region Macros
//##############################################################################
//
//                                   MACROS
//
//##############################################################################
#define SPLASH_FADETIME 500
#define SPLASH_DISPLAYTIME 1000

//##############################################################################
#pragma endregion

#pragma region Definition
//##############################################################################
//
//                                 DEFINITION
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                          6-Apr-2008
// =============================================================================
void CSplashScreen::Load()
{
	m_pImage = new CSprite(_SPRITE("Logo-SAPIAN"));

	m_pImage->SetAnchor(m_pImage->GetAreaCentre());
  m_pImage->SetPosition(XPOINT(_HSWIDTH, _HSHEIGHT));
}

// =============================================================================
// Nat Ryall                                                          6-Apr-2008
// =============================================================================
void CSplashScreen::Unload()
{
	delete m_pImage;
}

// =============================================================================
// Nat Ryall                                                          6-Apr-2008
// =============================================================================
void CSplashScreen::Wake()
{
	m_iStage = SplashScreenStage_FadeIn;
	m_iElapsedTime = 0;
	m_fAlpha = 0.0f;
}

// =============================================================================
// Nat Ryall                                                          6-Apr-2008
// =============================================================================
void CSplashScreen::Event(xuint iEventType, void* pEventInfo)
{
	switch (iEventType)
	{
	case INPUT_MBUTTONDOWN:
	case INPUT_KEYDOWN:
		{
			ScreenManager::Set(ScreenIndex_MenuScreen);
		}
		break;
	}
}

// =============================================================================
// Nat Ryall                                                          6-Apr-2008
// =============================================================================
void CSplashScreen::Update()
{
	m_iElapsedTime += _TIMEDELTA;

	switch (m_iStage)
	{
	case SplashScreenStage_FadeIn:
		{
			if (m_iElapsedTime > SPLASH_FADETIME)
			{
				m_iElapsedTime -= SPLASH_FADETIME;

				m_fAlpha = 1.0f;
				m_iStage = SplashScreenStage_Display;
			}
			else
				m_fAlpha = (float)m_iElapsedTime / (float)SPLASH_FADETIME;
		}
		break;

	case SplashScreenStage_Display:
		{
			if (m_iElapsedTime > SPLASH_DISPLAYTIME)
			{
				m_iElapsedTime -= SPLASH_DISPLAYTIME;
				m_iStage = SplashScreenStage_FadeOut;
			}
		}
		break;

	case SplashScreenStage_FadeOut:
		if (m_iElapsedTime > SPLASH_FADETIME)
		{
			m_iElapsedTime = 0;

			m_fAlpha = 0.0f;
			m_iStage = SplashScreenStage_None;

			ScreenManager::Set(ScreenIndex_MenuScreen);
		}
		else
			m_fAlpha = 1.0f - ((float)m_iElapsedTime / (float)SPLASH_FADETIME);
		break;
	}

	m_pImage->SetAlpha(m_fAlpha);
}

// =============================================================================
// Nat Ryall                                                          6-Apr-2008
// =============================================================================
void CSplashScreen::Render()
{
	XUCHAR cBackgroundColour = (XUCHAR)(m_fAlpha * 20.0f);
	_HGE->Gfx_Clear(ARGB(0, cBackgroundColour, cBackgroundColour, cBackgroundColour));

	m_pImage->Render();
}

//##############################################################################
#pragma endregion