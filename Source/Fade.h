#pragma once

//##############################################################################

// Global.
#include <Global.h>

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
	virtual void OnUpdate();

	// Callback for when the screen has finished processing.
	virtual void OnFadeComplete() {};

protected:
	// The current action being performed in the fade.
	t_FadeScreenStage m_iStage;

	// The time to fade in/out.
	xuint m_iFadeTime;

	// The time to display before fading out.
	xuint m_iDisplayTime;

	// The current elapsed time for the operation.
	xuint m_iElapsedTime;

	// The total time elapsed.
	xuint m_iTotalTime;

	// The current alpha level to apply.
	xfloat m_fAlpha;
};

//##############################################################################