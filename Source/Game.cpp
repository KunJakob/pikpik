/**
* @file Game.cpp
* @author Nat Ryall
* @date 11/04/2008
* @brief The actual game screen.
*
* Copyright © SAPIAN
*/

//##############################################################################

// Global.
#include <Global.h>

// Local.
#include <Game.h>

// Other.
#include <Minimap.h>
#include <Network.h>
#include <Sound.h>

//##############################################################################

// =============================================================================
void CGameScreen::OnActivate()
{
	Global.m_fMapAlpha = 1.f;
	Global.m_fMusicEnergy = 0.f;

	// Initialise the render manager for the game.
	m_xRenderView = new CRenderView(GameLayerIndex_Max);

	RenderManager.SetRenderView(m_xRenderView);

	// Initialise the render layers.
	m_pBackground = new CSprite(_SPRITE("Game-Background"));
	m_pBackground->GetMetadata()->GetSprite()->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHABLEND);
	RenderLayer(GameLayerIndex_Background)->AttachRenderable(m_pBackground);

	RenderLayer(GameLayerIndex_Map)->AttachRenderable(Global.m_pActiveMap);

	InitialisePlayers();

	XEN_LIST_FOREACH(t_PlayerList, ppPlayer, Global.m_lpActivePlayers)
		RenderLayer(GameLayerIndex_Player)->AttachRenderable(*ppPlayer);

	m_pMinimap = new CMinimap(Global.m_pActiveMap);
	RenderLayer(GameLayerIndex_Radar)->AttachRenderable(m_pMinimap);

	m_pGhostOverlay = new CSprite(_SPRITE("Ghost-Overlay"));
	RenderLayer(GameLayerIndex_GhostOverlay)->AttachRenderable(m_pGhostOverlay);

	m_pEdgeOverlay = new CSprite(_SPRITE("Edge-Overlay"));
	m_pEdgeOverlay->GetMetadata()->GetSprite()->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHABLEND);

	RenderLayer(GameLayerIndex_EdgeOverlay)->AttachRenderable(m_pEdgeOverlay);

#if !XRETAIL
	RenderLayer(GameLayerIndex_PathDebug)->SetRenderOverride(xbind(this, &CGameScreen::RenderPlayerPath));
	RenderLayer(GameLayerIndex_PathDebug)->SetEnabled(false);
#endif

	// Load the music to associate with the map colourisation.
	m_pMusic = new CSound(_SOUND("Game-Music"));
	m_pMusic->Play();
	//m_pMusic->GetChannel()->setVolume(0.0f);

	// Load the sound effects.
	m_pDeathSound = new CSound(_SOUND("Game-Death"));

	// Initialise the colourisation.
	for (xint iA = 0; iA < 3; ++iA)
	{
		Global.m_fColourChannels[iA] = .5f;
		m_bColouriseDir[iA] = (rand() % 2 == 0);
	}

}

// =============================================================================
void CGameScreen::OnDeactivate()
{
	CollisionManager.Reset();

	delete m_pBackground;

	delete m_pGhostOverlay;
	delete m_pEdgeOverlay;

	delete m_pMusic;
	delete m_pDeathSound;

	delete m_pMinimap;

	delete m_xRenderView;
	m_xRenderView = NULL;

	RenderManager.ClearRenderView();
}

// =============================================================================
void CGameScreen::OnWake()
{
	m_iState = GameState_Playing;
}

// =============================================================================
void CGameScreen::OnSleep()
{
}

// =============================================================================
xbool CGameScreen::OnEvent(xint iEventType, void* pEventInfo)
{
	hgeInputEvent* pEvent = (hgeInputEvent*)pEventInfo;

	switch (iEventType)
	{
	case INPUT_KEYUP:
		{
			switch (pEvent->key)
			{
				// ESCAPE.
			case HGEK_ESCAPE:
				{
					ScreenManager.Pop();
					return true;
				}
				break;
			}
		}
		break;
	}

	return false;
}

// =============================================================================
void CGameScreen::OnUpdate()
{
#if XDEBUG
	if (!NetworkManager.IsRunning())
		DebugControls();
#endif

	switch (m_iState)
	{
	case GameState_Playing:
		{
			// Calculate the music energy using spectrum analysis.
			if (m_pMusic->IsPlaying())
				CalculateMusicEnergy(m_pMusic->GetChannel());
		}
		break;
	}

	// Update the map.
	Global.m_pActiveMap->Update();

	// Update all the players.
	XEN_LIST_FOREACH(t_PlayerList, ppPlayer, Global.m_lpActivePlayers)
	{
		(*ppPlayer)->Update();
	}

	// Calculate the music colourisation.
	CalculateColourisation();

	// Generate the minimap.
	GenerateMinimap();
}

// =============================================================================
void CGameScreen::OnPreRender()
{
	// Set the transformations.
	xpoint xOffset = (Global.m_pLocalPlayer->GetSprite()->GetPosition() - xpoint(_HSWIDTH, _HSHEIGHT)) * -1;

	RenderLayer(GameLayerIndex_Map)->SetTransformation(xOffset);
	RenderLayer(GameLayerIndex_Player)->SetTransformation(xOffset);

#if !XRETAIL
	RenderLayer(GameLayerIndex_PathDebug)->SetTransformation(xOffset);
#endif

	// Scale the overlays to the beat of the music.
	if (m_iState == GameState_Playing)
	{
		if (Global.m_pLocalPlayer->GetType() == PlayerType_Pacman)
			ScaleToEnergy(GameLayerIndex_EdgeOverlay, Global.m_fMusicEnergy * 4.0f, 1.2f);
		else
		{
			ScaleToEnergy(GameLayerIndex_EdgeOverlay, 0.0f, 1.24f);
			ScaleToEnergy(GameLayerIndex_GhostOverlay, Global.m_fMusicEnergy * 15.0f, 1.5f);
		}
	}

	RenderLayer(GameLayerIndex_GhostOverlay)->SetEnabled(Global.m_pLocalPlayer->GetType() == PlayerType_Ghost);

	// Colourise the overlays.
	m_pBackground->GetMetadata()->GetSprite()->SetColor(_ARGBF(1.0f, Global.m_fColourChannels[0], Global.m_fColourChannels[1], Global.m_fColourChannels[2]));
	m_pEdgeOverlay->GetMetadata()->GetSprite()->SetColor(_ARGBF(1.0f, Global.m_fColourChannels[0], Global.m_fColourChannels[1], Global.m_fColourChannels[2]));
}

// =============================================================================
void CGameScreen::InitialisePlayers()
{
	// This is temporary until the character select screen is implemented.
	if (!NetworkManager.IsRunning())
	{
		Global.ResetActivePlayers();
		Global.m_pLocalPlayer = Global.m_lpActivePlayers.front();

		XEN_LIST_FOREACH(t_PlayerList, ppPlayer, Global.m_lpActivePlayers)
		{
			CPlayer* pPlayer = *ppPlayer;

			if (pPlayer->GetType() == PlayerType_Pacman)
			{
				pPlayer->SetLogicType(PlayerLogicType_Local);

				CollisionManager.Add((CPacman*)pPlayer);
			}
			else if (pPlayer->GetType() == PlayerType_Ghost)
			{
				pPlayer->SetLogicType(PlayerLogicType_AI);

				CollisionManager.Add((CGhost*)pPlayer);
			}
		}
	}
}

// =============================================================================
void CGameScreen::OnPacmanDie(CGhost* pGhost)
{
	m_pMusic->Stop();
	m_pDeathSound->Play();

	Global.m_fMusicEnergy = 0.1f;

	XEN_LIST_FOREACH(t_PlayerList, ppPlayer, Global.m_lpActivePlayers)
	{
		if ((*ppPlayer)->GetType() == PlayerType_Ghost)
		{
			(*ppPlayer)->SetLogicType(PlayerLogicType_None);
			(*ppPlayer)->NavigateTo((*ppPlayer)->m_pStartingBlock);
		}
		else
			(*ppPlayer)->SetLogicType(PlayerLogicType_None);
	}

	m_iState = GameState_Finished;
}

// =============================================================================
void CGameScreen::GenerateMinimap()
{
	xuint iVisibleBlocks = MinimapElement_Walls | MinimapElement_GhostWalls | MinimapElement_GhostBase;
	iVisibleBlocks |= (Global.m_pLocalPlayer->GetType() == PlayerType_Pacman) ? MinimapElement_Pacman : MinimapElement_Ghost;

	if (Global.m_pLocalPlayer->GetType() == PlayerType_Ghost)
	{
		if (Global.m_pLocalPlayer->GetCurrentBlock()->IsGhostBase())
			iVisibleBlocks |= MinimapElement_Pellets;
	}

	m_pMinimap->Generate(iVisibleBlocks);
}

// =============================================================================
void CGameScreen::CalculateMusicEnergy(FMOD::Channel* pChannel)
{
	const static xint s_iIterations = 2048;
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

	Global.m_fMusicEnergy = fAverageStrength * 0.1f;
}

// =============================================================================
void CGameScreen::CalculateColourisation()
{
	// Blend the colours based on the music energy.
	const static xfloat s_fMinColour = 0.2f;
	const static xfloat s_fMaxColour = 1.0f;

	for (xint iA = 0; iA < 3; ++iA)
	{
		xfloat fChannelEnergy = Global.m_fMusicEnergy * (iA + 1);

		if (m_bColouriseDir[iA])
		{
			Global.m_fColourChannels[iA] += fChannelEnergy;
			m_bColouriseDir[iA] = !(Global.m_fColourChannels[iA] > s_fMaxColour);
		}
		else
		{
			Global.m_fColourChannels[iA] -= fChannelEnergy;
			m_bColouriseDir[iA] = (Global.m_fColourChannels[iA] < s_fMinColour);
		}

		Global.m_fColourChannels[iA] = Math::Clamp(Global.m_fColourChannels[iA], s_fMinColour, s_fMaxColour);
	}
}

// =============================================================================
void CGameScreen::ScaleToEnergy(t_GameLayerIndex iLayer, xfloat fEnergy, xfloat fInitialScale)
{
	xfloat fScale = fInitialScale - fEnergy;

	xfloat fOffsetX = ((fEnergy - (fInitialScale - 1.0f)) * (xfloat)_SWIDTH) * 0.5f;
	xfloat fOffsetY = ((fEnergy - (fInitialScale - 1.0f)) * (xfloat)_SHEIGHT) * 0.5f;

	RenderLayer(iLayer)->SetTransformation(xpoint((xint)fOffsetX, (xint)fOffsetY) , 0.0f, fScale, fScale);
}

// =============================================================================
void CGameScreen::RenderPlayerPath(CRenderLayer* pLayer)
{
	XEN_LIST_FOREACH(t_PlayerList, ppPlayer, Global.m_lpActivePlayers)
	{
		CNavigationPath* pPath = (*ppPlayer)->GetNavPath();

		if (pPath)
		{
			for (xint iA = 0; iA < pPath->GetNodeCount(); ++iA)
			{
				CMapBlock* pBlock = pPath->GetNode(iA)->GetDataAs<CMapBlock>();
				xpoint xPos = pBlock->GetScreenPosition();

				RenderManager.RenderBox(false, xrect(xPos.m_tX - 5, xPos.m_tY - 5, xPos.m_tX + 5, xPos.m_tY + 5), _RGB(255, 0, 0));
			}
		}
	}
}

// =============================================================================
void CGameScreen::DebugControls()
{
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
				Global.m_fMapAlpha = 1.f;

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

	// Test path-finding on the local player.
	if (_HGE->Input_KeyDown(HGEK_CTRL))
	{
		Global.m_pLocalPlayer->NavigateTo(Global.m_pActiveMap->GetBlock(rand() % Global.m_pActiveMap->GetBlockCount()));
	}

	if (_HGE->Input_KeyDown(HGEK_F1))
	{
		RenderLayer(GameLayerIndex_PathDebug)->SetEnabled(!RenderLayer(GameLayerIndex_PathDebug)->IsEnabled());
	}

	// Test overlays.
	if (_HGE->Input_KeyDown(HGEK_F3))
		RenderLayer(GameLayerIndex_EdgeOverlay)->SetEnabled(!RenderLayer(GameLayerIndex_EdgeOverlay)->IsEnabled());
	if (_HGE->Input_KeyDown(HGEK_F4))
		RenderLayer(GameLayerIndex_GhostOverlay)->SetEnabled(!RenderLayer(GameLayerIndex_GhostOverlay)->IsEnabled());
}
