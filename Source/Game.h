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
#include <Sound.h>
#include <Navigation.h>

//##############################################################################

// Game layer index list.
enum t_GameLayerIndex
{
	GameLayerIndex_Background,
	GameLayerIndex_Map,
	GameLayerIndex_Items,
	GameLayerIndex_Player,
	GameLayerIndex_Effects,
	GameLayerIndex_GhostOverlay,
	GameLayerIndex_EdgeOverlay,
	GameLayerIndex_Radar,
	GameLayerIndex_Countdown,

	GameLayerIndex_PathDebug,

	GameLayerIndex_Max,
};

// The game states.
enum t_GameState
{
	GameState_None,
	GameState_Intro,
	GameState_Playing,
	GameState_Finished,

	GameState_Max,
};

//##############################################################################
class CGameScreen : public CScreen
{
public:
	// Constructor.
	CGameScreen() : CScreen(ScreenIndex_GameScreen) {}

	// Callback for when Pacman is captured by a ghost.
	void OnPacmanDie(CGhost* pGhost);

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

	// Called when a specific event is executed.
	// ~return Specifies if the event was consumed.
	virtual xbool OnEvent(xint iEventType, void* pEventInfo);

	// Called to update the screen (updates the parent screen by default).
	virtual void OnUpdate();

	// Update the intro sequence.
	void UpdateIntro();

	// Generate the minimap for the current playing field.
	void GenerateMinimap();

	// Called to pre-render the next screen frame.
	virtual void OnPreRender();

	// Initialise all players in the game.
	void InitialisePlayers();

	// Reset the game.
	void ResetGame();

	// Start the game.
	void StartGame();

	// Called to end the game.
	void EndGame();

	// Set the game state.
	void SetState(t_GameState iGameState);

	// Calculate the "energy" for the music to determine background colour fading.
	void CalculateMusicEnergy(FMOD::Channel* pChannel);

	// Calculate the current colourisation based on the music energy.
	void CalculateColourisation();

	// Scale the specified layer to the "energy" of the music.
	void ScaleToEnergy(t_GameLayerIndex iLayer, xfloat fEnergy, xfloat fInitialScale);

	// Render the game countdown.
	void RenderCountdown(CRenderLayer* pLayer);

	// Render the local player's path on the map.
	void RenderPlayerPath(CRenderLayer* pLayer);

	// Debug controls for character switching.
	void DebugControls();

	// The current game state.
	t_GameState m_iState;

	// The screen's render view.
	CRenderView* m_xRenderView;

	// The map background.
	CSprite* m_pBackground;

	// The ghost overlay.
	CSprite* m_pGhostOverlay;

	// The pacman overlay.
	CSprite* m_pEdgeOverlay;

	// The background music.
	//CSound* m_pMusic;
	FMOD::Sound* m_pMusic;

	// The music channel.
	FMOD::Channel* m_pChannel;

	// The world transform offset.
	xpoint m_xOffset;

	// The game's minimap.
	CMinimap* m_pMinimap;

	// The death sound.
	CSound* m_pDeathSound;

	// The current colour transition direction.
	xbool m_bColouriseDir[3];

	// The countdown timer.
	CTimer m_xCountdownTimer;

	// The countdown count.
	xint m_iCountdown;

	// The countdown font.
	CFont* m_pCountdownFont;

	// The countdown sound.
	CSound* m_pCountdownSound;
};

//##############################################################################