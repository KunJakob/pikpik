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

// Global.
#include <Global.h>

// Other.
#include <Map.h>
#include <Player.h>
#include <Background.h>
#include <Interface.h>
#include <Minimap.h>

//##############################################################################
class CGameScreen : public CScreen
{
public:
	// Constructor.
	CGameScreen() : CScreen(ScreenIndex_GameScreen) {}

protected:
	// Called to load the screen resources.
	virtual void OnLoad() {}

	// Called to unload the screen resources.
	virtual void OnUnload() {}

	// Called when the screen is activated.
	virtual void OnActivate();

	// Called when the state is deactivated.
	virtual void OnDeactivate();

	// Called when the screen is active and is the immediate screen in the stack.
	virtual void OnWake();

	// Called when the screen is no longer the immediate screen in the stack but is still active.
	virtual void OnSleep();

	// Called to update the screen (updates the parent screen by default).
	virtual void OnUpdate();

	// Called to pre-render the next screen frame.
	virtual void OnPreRender();

	// Initialise all players in the game.
	void InitialisePlayers();

	// Generate a field mask texture for ghost vision.
	HTEXTURE GenerateGhostMask(xint iInnerRadius, xint iOuterRadius);

	// Render the ghost mask.
	void RenderGhostMask(CRenderLayer* pLayer);

	// Calculate the "energy" for the music to determine background colour fading.
	void CalculateMusicEnergy(FMOD::Channel* pChannel);

	// The screen's render view.
	CRenderView* m_xRenderView;

	// The map background.
	CBackground m_xBackground;

	// The field mask.
	hgeSprite* m_pGhostMask;

	// The background music.
	FMOD::Sound* m_pMusic;

	// The music channel.
	FMOD::Channel* m_pChannel;

	// The world transform offset.
	xpoint m_xOffset;

	// The game's minimap.
	CMinimap* m_pMinimap;
};

//##############################################################################