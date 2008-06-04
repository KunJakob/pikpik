#pragma once

#pragma region Include
//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Global.
#include <Global.h>

//##############################################################################
#pragma endregion

#pragma region Types
//##############################################################################
//
//                                   TYPES
//
//##############################################################################
enum t_FadeScreenStage
{
	FadeScreenStage_None,
	FadeScreenStage_FadeIn,
	FadeScreenStage_Display,
	FadeScreenStage_FadeOut,
	FadeScreenStage_Complete,
};

//##############################################################################
#pragma endregion

#pragma region Declaration
//##############################################################################
//
//                                 DECLARATION
//
//##############################################################################
class CFadeScreen : public CScreen
{
public:
	// Constructor.
	CFadeScreen(t_ScreenIndex iIndex, xuint iFadeTime, xuint iDisplayTime);

	// Destructor.
	virtual ~CFadeScreen() {}

	// Called to reset the fade screen to defaults.
	void Reset();

	// Called each frame to update the screen when active.
	virtual void Update();

	// Callback for when the screen has finished processing.
	virtual void OnFadeComplete() {};

protected:
	t_FadeScreenStage m_iStage;
	xuint m_iFadeTime;
	xuint m_iDisplayTime;
	xuint m_iElapsedTime;
	xfloat m_fAlpha;
};

//##############################################################################
#pragma endregion