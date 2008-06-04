#pragma once

#pragma region Include
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
#pragma endregion

#pragma region Logo
//##############################################################################
//
//                                     LOGO
//
//##############################################################################
class CLogoScreen : public CFadeScreen
{
public:
	// Constructor.
	CLogoScreen() : CFadeScreen(ScreenIndex_LogoScreen, 500, 2000), m_pImage(NULL) {}

	// Destructor.
	virtual ~CLogoScreen() {}

	/**
	* Called when the screen is registered in the constructor.
	*/
	virtual void Load();

	/**
	* Called when the state is dismissed in the destructor.
	*/
	virtual void Unload();

	/**
	* Called once when the screen is placed at the top of the stack.
	*/
	virtual void Wake();

	/**
	* Called when a game-specific event is executed when active.
	*/
	virtual void Event(XIN xuint iEventType, XIN void* pEventInfo);

	/**
	* Called each frame to update the screen when active.
	*/
	virtual void Update();

	/**
	* Called each frame to render the screen when active.
	*/
	virtual void Render();

	// Called when the fade has completed.
	virtual void OnFadeComplete();

protected:
	// The logo image.
	CSprite* m_pImage;
};

//##############################################################################
#pragma endregion

#pragma region Warning
//##############################################################################
//
//                                   WARNING
//
//##############################################################################
class CWarningScreen : public CFadeScreen
{
public:
	// Constructor.
	CWarningScreen() : CFadeScreen(ScreenIndex_WarningScreen, 500, 4000) {}

	// Destructor.
	virtual ~CWarningScreen() {}

	// Called when a game-specific event is executed when active.
	virtual void Event(XIN xuint iEventType, XIN void* pEventInfo);

	// Called each frame to render the screen when active.
	virtual void Render();

	// Called when the fade has completed.
	virtual void OnFadeComplete();
};

//##############################################################################
#pragma endregion