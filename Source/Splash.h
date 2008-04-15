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
#include <Resource.h>
#include <Sprite.h>
#include <Map.h>

//##############################################################################
#pragma endregion

#pragma region Types
//##############################################################################
//
//                                   TYPES
//
//##############################################################################
enum t_SplashScreenStage
{
	SplashScreenStage_None,
	SplashScreenStage_FadeIn,
	SplashScreenStage_Display,
	SplashScreenStage_FadeOut,
};

//##############################################################################
#pragma endregion

#pragma region Declaration
//##############################################################################
//
//                                 DECLARATION
//
//##############################################################################
class CSplashScreen : public CScreen
{
public:
  CSplashScreen() : CScreen(ScreenIndex_SplashScreen), m_pImage(NULL) {}
  virtual ~CSplashScreen() {}

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
	* Called once when the screen goes out of scope either through destruction or another screen is placed above this on the stack.
	*/
	virtual void Sleep() {}

	/**
	* Called each frame to update the screen when active.
	*/
	virtual void Update();

	/**
	* Called each frame to render the screen when active.
	*/
	virtual void Render();

	/**
	* Called when a game-specific event is executed when active.
	*/
	virtual void Event(XIN xuint iEventType, XIN void* pEventInfo);

protected:
	t_SplashScreenStage m_iStage;

	CSprite* m_pImage;

	XUINT m_iElapsedTime;
	XFLOAT m_fAlpha;
};

//##############################################################################
#pragma endregion