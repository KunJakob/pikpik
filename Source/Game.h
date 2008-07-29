#pragma once

/**
* @file Game.h
* @author Nat Ryall
* @date 11/04/2008
* @brief The actual game screen.
*
* Copyright © SAPIAN
*/

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

//##############################################################################
//
//                                 DECLARATION
//
//##############################################################################
class CGameScreen : public CScreen
{
public:
	// Constructor.
	CGameScreen() : CScreen(ScreenIndex_GameScreen) {}

	// Load.
	virtual void Load();

	// Unload.
	virtual void Unload();

	// Wake.
	virtual void Wake() {}

	// Sleep.
	virtual void Sleep() {}

	// Update.
	virtual void Update();

	// Render.
	virtual void Render();

protected:
	// Load the currently active map.
	void LoadMap();

	// Load the game music track.
	void LoadMusic();

	// Initialise all players in the game.
	void InitialisePlayers();

	// The map background.
	CBackground m_xBackground;

	// The field mask.
	hgeSprite* m_pFieldMask;

	// The background music.
	FMOD::Sound* m_pMusic;

	// The music channel.
	FMOD::Channel* m_pChannel;

	// The world transform offset.
	xpoint m_xOffset;

	// The game's minimap.
	CMinimap* m_pMinimap;

private:
	// Calculate the music energy from the specified channel.
	void CalculateMusicEnergy(FMOD::Channel* pChannel);

	// Transform all players/objects and the world by the current offset to centre the player to the screen.
	void WorldTransform(CRenderable* pRenderable);

	// (DEBUG) Render collidable objects.
	void _RenderCollidables(CRenderable* pRenderable);
};

//##############################################################################