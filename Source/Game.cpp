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

	//RenderManager.SetRenderCallback(LayerIndex_Map, xbind(this, &CGameScreen::WorldTransform));
	//RenderManager.SetRenderCallback(LayerIndex_Player, xbind(this, &CGameScreen::WorldTransform));

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
	CalculateMusicEnergy(m_pChannel);

	// Generate the minimap.
	m_pMinimap->Generate(MinimapElement_Walls | MinimapElement_GhostWalls | MinimapElement_GhostBase | MinimapElement_Ghost | MinimapElement_Pacman);

	// Set the transformations.
	//xpoint xOffset = Global.m_pLocalPlayer->GetSprite()->GetPosition() - xpoint(_HSWIDTH, _HSHEIGHT);
	//xOffset = xpoint(0, 0);

	//RenderManager.SetTransformation(LayerIndex_Map, xpoint(0, 0));
	//RenderManager.SetTransformation(LayerIndex_Player, xpoint());
}

// =============================================================================
// Nat Ryall                                                         11-Apr-2008
// =============================================================================
void CGameScreen::OnRender()
{
}

// =============================================================================
// Nat Ryall                                                          3-Jun-2008
// =============================================================================
void CGameScreen::CalculateMusicEnergy(FMOD::Channel* pChannel)
{
	/*const static xint s_iIterations = 2048;
	const static xint s_iSearch = 8;

	xfloat fSpectrum[2][s_iIterations];

	pChannel->getSpectrum(fSpectrum[0], s_iIterations, 0, FMOD_DSP_FFT_WINDOW_HANNING);
	pChannel->getSpectrum(fSpectrum[1], s_iIterations, 1, FMOD_DSP_FFT_WINDOW_HANNING);

	xfloat fStrength[s_iSearch];

	for (xint iA = 0; iA < s_iSearch; ++iA)
		fStrength[iA] = fSpectrum[0][iA] + fSpectrum[1][iA];

	xfloat fAverageStrength = 0.f;

	for (xint iA = 4; iA < s_iSearch; ++iA)
		fAverageStrength += fStrength[iA];

	fAverageStrength /= 4.f;

	Global.m_fMusicEnergy = fAverageStrength * 0.1f;*/

	Global.m_fMusicEnergy = 0.f;
}

// =============================================================================
// Nat Ryall                                                         14-Apr-2008
// =============================================================================
void CGameScreen::WorldTransform(CRenderable* pRenderable)
{
	/*xpoint xOffset = Global.m_pLocalPlayer->GetSprite()->GetPosition() - xpoint(_HSWIDTH, _HSHEIGHT);

	switch (pRenderable->GetRenderableType())
	{
	case RenderableType_Map:
		{
			CMap* pMap = (CMap*)pRenderable;
			pMap->SetOffset(xOffset);
			pMap->Render();
		}
		break;

	case RenderableType_Player:
		{
			CPlayer* pPlayer = (CPlayer*)pRenderable;

			xpoint xPosition = pPlayer->GetSprite()->GetPosition();

			pPlayer->GetSprite()->SetPosition(xPosition - xOffset);
			pPlayer->Render();
			pPlayer->GetSprite()->SetPosition(xPosition);
		}
		break;

	default:
		pRenderable->Render();
	}*/
}

//##############################################################################
