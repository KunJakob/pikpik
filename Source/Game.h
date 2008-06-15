#pragma once

/**
* @file Game.h
* @author Nat Ryall
* @date 11/04/2008
* @brief The actual game screen.
*
* Copyright © SAPIAN
*/

#pragma region Include
//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Global.
#include <Global.h>

// Other.
#include <Map.h>
#include <Player.h>
#include <Background.h>
#include <Interface.h>

//##############################################################################
#pragma endregion

#pragma region Declaration
//##############################################################################
//
//                                 DECLARATION
//
//##############################################################################
class CGameScreen : public CScreen
{
public:
	CGameScreen() : CScreen(ScreenIndex_GameScreen) {}

	virtual void Load();
	virtual void Wake() {}

	virtual void Update();
	virtual void Render();

	virtual void Notify(xuint iEventType, void* pEventInfo) {}

	virtual void Sleep() {}
	virtual void Unload();

protected:
	// The map background.
	CBackground m_Background;

	// The field mask.
	hgeSprite* m_pFieldMask;

	// The background music.
	FMOD::Sound* m_pMusic;
	FMOD::Channel* m_pChannel;
};

//##############################################################################
#pragma endregion