/**
* @file Game.cpp
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

// Local.
#include <Game.h>

// Other.
#include <Minimap.h>
#include <Network.h>

//##############################################################################

//##############################################################################
//
//                                 DEFINITION
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         11-Apr-2008
// =============================================================================
void CGameScreen::OnActivate()
{
	Global.m_fWorldAlpha = 1.f;
	Global.m_fMusicEnergy = 0.f;

	InterfaceManager.SetCursorVisible(false);

	LoadMap();
	LoadMusic();

	InitialisePlayers();

	RenderManager.Add(LayerIndex_Background, &m_xBackground);

	m_pMinimap = new CMinimap(Global.m_pActiveMap);
	RenderManager.Add(LayerIndex_Radar, m_pMinimap);
}

// =============================================================================
// Nat Ryall                                                         15-Jul-2008
// =============================================================================
void CGameScreen::LoadMap()
{
	RenderManager.Add(LayerIndex_Map, Global.m_pActiveMap);
}

// =============================================================================
// Nat Ryall                                                         15-Jul-2008
// =============================================================================
void CGameScreen::LoadMusic()
{
	//_FMOD->createStream("Audio\\Level-Test.mp3", FMOD_SOFTWARE, NULL, &m_pMusic);
	//_FMOD->playSound(FMOD_CHANNEL_FREE, m_pMusic, false, &m_pChannel);
}

// =============================================================================
// Nat Ryall                                                         15-Jul-2008
// =============================================================================
void CGameScreen::InitialisePlayers()
{
	// This is temporary until the character select screen is implemented (offline only).
	if (!NetworkManager.IsRunning())
	{
		Global.ResetActivePlayers();
		Global.m_pLocalPlayer = Global.m_lpActivePlayers.front();

		XEN_LIST_FOREACH(t_PlayerList, ppPlayer, Global.m_lpActivePlayers)
		{
			CPlayer* pPlayer = *ppPlayer;

			pPlayer->SetLogicType(PlayerLogicType_AI);

			if (pPlayer->GetType() == PlayerType_Pacman)
				CollisionManager.Add((CPacman*)pPlayer);
			else if (pPlayer->GetType() == PlayerType_Ghost)
				CollisionManager.Add((CGhost*)pPlayer);
		}
	}

	// Add all active players to the player render layer.
	XEN_LIST_FOREACH(t_PlayerList, ppPlayer, Global.m_lpActivePlayers)
		RenderManager.Add(LayerIndex_Player, *ppPlayer);
}

// =============================================================================
// Nat Ryall                                                         11-Apr-2008
// =============================================================================
void CGameScreen::OnDeactivate()
{
	delete m_pMinimap;

	//m_pMusic->release();

	RenderManager.Reset();
	CollisionManager.Reset();

	InterfaceManager.SetCursorVisible(true);
}

// =============================================================================
// Nat Ryall                                                         11-Apr-2008
// =============================================================================
void CGameScreen::OnUpdate()
{
	// Exit the game.
	if (_HGE->Input_KeyUp(HGEK_ESCAPE))
	{
		ScreenManager.Pop();
		return;
	}

	// Switch between players.
	if (_HGE->Input_KeyDown(HGEK_SPACE))
	{
		XEN_LIST_FOREACH(t_PlayerList, ppPlayer, Global.m_lpActivePlayers)
		{
			if (Global.m_pLocalPlayer == *ppPlayer)
			{
				if (*ppPlayer == Global.m_lpActivePlayers.back())
					ppPlayer = Global.m_lpActivePlayers.begin();
				else
					ppPlayer++;

				Global.m_pLocalPlayer = *ppPlayer;
				Global.m_fWorldAlpha = 1.f;

				break;
			}
		}
	}

	// Switch between logic types.
	if (_HGE->Input_KeyDown(HGEK_SHIFT))
	{
		t_PlayerLogicType iLogicType = Global.m_pLocalPlayer->GetLogicType();

		if (iLogicType == PlayerLogicType_Local)
			iLogicType = PlayerLogicType_AI;
		else
			iLogicType = PlayerLogicType_Local;

		Global.m_pLocalPlayer->SetLogicType(iLogicType);
	}

	// Calculate the music energy using spectrum analysis.
	Global.m_fMusicEnergy = 0.f;

	// Generate the minimap.
	m_pMinimap->Generate(MinimapElement_Walls | MinimapElement_GhostWalls | MinimapElement_GhostBase | MinimapElement_Ghost | MinimapElement_Pacman);
}

// =============================================================================
// Nat Ryall                                                         11-Apr-2008
// =============================================================================
void CGameScreen::OnRender()
{
	// Set the transformations.
	xpoint xOffset = (Global.m_pLocalPlayer->GetSprite()->GetPosition() - xpoint(_HSWIDTH, _HSHEIGHT)) * -1;

	RenderManager.SetTransformation(LayerIndex_Map, xOffset);
	RenderManager.SetTransformation(LayerIndex_Player, xOffset);
}

//##############################################################################
