#pragma once

//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Global.
#include <Global.h>

// Other.
#include <Fade.h>
#include <Resource.h>
#include <Sprite.h>
#include <Map.h>

//##############################################################################

//##############################################################################
//
//                                     LOGO
//
//##############################################################################
class CLogoScreen : public CFadeScreen
{
public:
	// Constructor.
	CLogoScreen() : CFadeScreen(ScreenIndex_LogoScreen, 500, 4500), m_pImage(NULL) {}

	// Destructor.
	virtual ~CLogoScreen() {}

	// Called when the screen is registered in the constructor.
	virtual void OnActivate();

	// Called when the state is dismissed in the destructor.
	virtual void OnDeactivate();

	// Called once when the screen is placed at the top of the stack.
	virtual void OnWake();

	// Called when a game-specific event is executed when active.
	virtual void OnNotify(xuint iEventType, void* pEventInfo);

	// Called each frame to update the screen when active.
	virtual void OnUpdate();

	// Called each frame to render the screen when active.
	virtual void OnRender();

	// Called when the fade has completed.
	virtual void OnFadeComplete();

protected:
	// The logo image.
	CSprite* m_pImage;

	// The logo sound clip.
	FMOD::Sound* m_pSound;
	FMOD::Channel* m_pChannel;
};

//##############################################################################

//##############################################################################
//
//                                   WARNING
//
//##############################################################################
class CWarningScreen : public CFadeScreen
{
public:
	// Constructor.
	CWarningScreen() : CFadeScreen(ScreenIndex_WarningScreen, 600, 4000) {}

	// Destructor.
	virtual ~CWarningScreen() {}

	// Called when a game-specific event is executed when active.
	virtual void OnNotify(xuint iEventType, void* pEventInfo);

	// Called each frame to render the screen when active.
	virtual void OnRender();

	// Called when the fade has completed.
	virtual void OnFadeComplete();
};

//##############################################################################