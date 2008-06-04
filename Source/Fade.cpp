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
// Nat Ryall                                                          4-Jun-2008
// =============================================================================
CFadeScreen::CFadeScreen(t_ScreenIndex iIndex, xuint iFadeTime, xuint iDisplayTime) : CScreen(iIndex),
	m_iFadeTime(iFadeTime),
	m_iDisplayTime(iDisplayTime)
{
	Reset();
}

// =============================================================================
// Nat Ryall                                                          4-Jun-2008
// =============================================================================
void CFadeScreen::Reset()
{
	m_iStage = FadeScreenStage_FadeIn;
	m_iElapsedTime = 0;
	m_fAlpha = 0.0f;
}

// =============================================================================
// Nat Ryall                                                          6-Apr-2008
// =============================================================================
void CFadeScreen::Update()
{
	m_iElapsedTime += _TIMEDELTA;

	switch (m_iStage)
	{
	case FadeScreenStage_FadeIn:
		{
			if (m_iElapsedTime > m_iFadeTime)
			{
				m_iElapsedTime -= m_iFadeTime;

				m_fAlpha = 1.0f;
				m_iStage = FadeScreenStage_Display;
			}
			else
				m_fAlpha = (float)m_iElapsedTime / (float)m_iFadeTime;
		}
		break;

	case FadeScreenStage_Display:
		{
			if (m_iElapsedTime > m_iDisplayTime)
			{
				m_iElapsedTime -= m_iDisplayTime;
				m_iStage = FadeScreenStage_FadeOut;
			}
		}
		break;

	case FadeScreenStage_FadeOut:
		if (m_iElapsedTime > m_iFadeTime)
		{
			m_iElapsedTime = 0;

			m_fAlpha = 0.0f;
			m_iStage = FadeScreenStage_Complete;

			OnFadeComplete();
		}
		else
			m_fAlpha = 1.0f - ((float)m_iElapsedTime / (float)m_iFadeTime);
		break;

	case FadeScreenStage_Complete:
		break;
	}
}

//##############################################################################
#pragma endregion