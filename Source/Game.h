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
#include <Minimap.h>

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

	virtual void Unload();

	virtual void Wake() {}

	virtual void Sleep() {}

	virtual void Update();

	virtual void Render();

	virtual void Notify(xuint iEventType, void* pEventInfo) {}

protected:
	void LoadMap(xint iIndex);

	void LoadMusic();

	void InitialisePlayers();

	void WorldTransform(CRenderable* pRenderable);

	HTEXTURE GenerateFieldMask(xint iInnerRadius, xint iOuterRadius);

	void CalculateMusicEnergy(FMOD::Channel* pChannel);

	// The map background.
	CBackground m_xBackground;

	// The field mask.
	hgeSprite* m_pFieldMask;

	// The background music.
	FMOD::Sound* m_pMusic;
	FMOD::Channel* m_pChannel;

	// The world transform offset.
	xpoint m_xOffset;

	//
	CMinimap* m_pMinimap;
};

//##############################################################################
#pragma endregion