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
	// Initialise the players.
	PlayerManager.InitialisePlayers(PlayerLogicType_None);
	PlayerManager.SetLocalPlayer(0);

	// Initialise the render manager for the game.
	m_xRenderView = new CRenderView(GameLayerIndex_Max);

	RenderManager.SetRenderView(m_xRenderView);

	// Initialise the render layers.
	m_pBackground = new CSprite(_SPRITE("Game-Background"));
	m_pBackground->GetMetadata()->GetSprite()->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHABLEND);
	RenderLayer(GameLayerIndex_Background)->AttachRenderable(m_pBackground);

	RenderLayer(GameLayerIndex_Map)->AttachRenderable(MapManager.GetCurrentMap());

	XEN_LIST_FOREACH(t_PlayerList, ppPlayer, PlayerManager.GetActivePlayers())
	{
		RenderLayer(GameLayerIndex_Player)->AttachRenderable(*ppPlayer);
	}

	m_pMinimap = new CMinimap(MapManager.GetCurrentMap());
	RenderLayer(GameLayerIndex_Radar)->AttachRenderable(m_pMinimap);

	m_pGhostOverlay = new CSprite(_SPRITE("Ghost-Overlay"));
	RenderLayer(GameLayerIndex_GhostOverlay)->AttachRenderable(m_pGhostOverlay);

	m_pEdgeOverlay = new CSprite(_SPRITE("Edge-Overlay"));
	m_pEdgeOverlay->GetMetadata()->GetSprite()->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHABLEND);

	RenderLayer(GameLayerIndex_EdgeOverlay)->AttachRenderable(m_pEdgeOverlay);

	m_pCountdownFont = new CFont(_FONT("Game-Countdown"));
	RenderLayer(GameLayerIndex_Countdown)->SetRenderOverride(xbind(this, &CGameScreen::RenderCountdown));

#if !XRETAIL
	RenderLayer(GameLayerIndex_PathDebug)->SetRenderOverride(xbind(this, &CGameScreen::RenderPlayerPath));
	RenderLayer(GameLayerIndex_PathDebug)->SetEnabled(false);
#endif

	// Load the sound effects.
	m_pDeathSound = new CSound(_SOUND("Game-Death"));
	m_pCountdownSound = new CSound(_SOUND("Countdown-Beep"));

	m_iState = GameState_None;
}

// =============================================================================
void CGameScreen::OnDeactivate()
{
	CollisionManager.Reset();

	delete m_pCountdownFont;
	delete m_pCountdownSound;

	delete m_pBackground;

	delete m_pGhostOverlay;
	delete m_pEdgeOverlay;

	if (m_pMusic)
		m_pMusic->release();

	//delete m_pMusic;
	delete m_pDeathSound;

	delete m_pMinimap;

	delete m_xRenderView;
	m_xRenderView = NULL;

	RenderManager.ClearRenderView();
}

// =============================================================================
void CGameScreen::OnWake()
{
	// Start the countdown.
	SetState(GameState_Intro);
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
			case HGEK_ESCAPE:
				{
					ScreenManager.Pop();
					return true;
				}
				break;

			case HGEK_ENTER:
				{
					if (m_iState == GameState_Finished)
						SetState(GameState_Intro);
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
	case GameState_Intro:
		{
			UpdateIntro();
		}
		break;

	case GameState_Playing:
		{
			// Calculate the music energy using spectrum analysis.
			if (m_pChannel)
			{
				xbool bPlaying = false;
				m_pChannel->isPlaying(&bPlaying);

				if (bPlaying)
					CalculateMusicEnergy(m_pChannel);
			}
		}
		break;
	}

	// Set certain layer enabled/disabled statuses.
	RenderLayer(GameLayerIndex_Countdown)->SetEnabled(m_iState == GameState_Intro);

	// Calculate the music colourisation.
	CalculateColourisation();

	// Update the other components.
	MapManager.Update();
	PlayerManager.Update();

	// Generate the minimap.
	GenerateMinimap();
}

// =============================================================================
void CGameScreen::UpdateIntro()
{
	if (m_xCountdownTimer.IsExpired())
	{
		if (--m_iCountdown == 0)
			SetState(GameState_Playing);
		else
		{
			m_pCountdownSound->Play();
			m_xCountdownTimer.ExpireAfter(1000);
		}
	}
}

// =============================================================================
void CGameScreen::OnPreRender()
{
	// Set the transformations.
	xpoint xOffset = (PlayerManager.GetLocalPlayer()->GetSprite()->GetPosition() - xpoint(_HSWIDTH, _HSHEIGHT)) * -1;

	RenderLayer(GameLayerIndex_Map)->SetTransformation(xOffset);
	RenderLayer(GameLayerIndex_Player)->SetTransformation(xOffset);

#if !XRETAIL
	RenderLayer(GameLayerIndex_PathDebug)->SetTransformation(xOffset);
#endif

	// Scale the overlays to the beat of the music.
	if (m_iState == GameState_Playing)
	{
		if (PlayerManager.GetLocalPlayer()->GetType() == PlayerType_Pacman)
			ScaleToEnergy(GameLayerIndex_EdgeOverlay, Global.m_fMusicEnergy * 4.0f, 1.2f);
		else
		{
			ScaleToEnergy(GameLayerIndex_EdgeOverlay, 0.0f, 1.24f);
			ScaleToEnergy(GameLayerIndex_GhostOverlay, Global.m_fMusicEnergy * 15.0f, 1.5f);
		}
	}

	RenderLayer(GameLayerIndex_GhostOverlay)->SetEnabled(PlayerManager.GetLocalPlayer()->GetType() == PlayerType_Ghost);

	// Colourise the overlays.
	xfloat fColours[3];

	for (xint iA = 0; iA < 3; ++iA)
		fColours[iA] = Global.m_fColourChannels[iA];

	m_pBackground->GetMetadata()->GetSprite()->SetColor(_ARGBF(0.2f, fColours[0], fColours[1], fColours[2]));
	m_pEdgeOverlay->GetMetadata()->GetSprite()->SetColor(_ARGBF(1.0f, fColours[0], fColours[1], fColours[2]));
}

// =============================================================================
void CGameScreen::ResetGame()
{
	// Reset the map effect control values.
	Global.m_fMapAlpha = 1.f;
	Global.m_fMusicEnergy = 0.f;

	// Start a random track from the level music directory.
	//m_pMusic = new CSound(_SOUND("Game-Music"));
	//m_pMusic->GetChannel()->setVolume(0.0f);

	m_pMusic = NULL;
	m_pChannel = NULL;

	t_FileScanResult lsMusicFiles = FileManager.Scan("Sound\\Level\\*.mp3");
	xint iMusicIndex = rand() % lsMusicFiles.size();

	if (lsMusicFiles.size())
		_FMOD->createStream(XFORMAT("Sound\\Level\\%s", lsMusicFiles[iMusicIndex].c_str()), FMOD_SOFTWARE, 0, &m_pMusic);

	// Initialise the colourisation.
	for (xint iA = 0; iA < 3; ++iA)
	{
		Global.m_fColourChannels[iA] = .5f;
		m_bColouriseDir[iA] = (rand() % 2 == 0);
	}

	// Disable the players.
	PlayerManager.SetPlayersEnabled(false);

	// Reset the countdown and start it.
	m_iCountdown = 4;
	m_xCountdownTimer.ExpireAfter(0);
}

// =============================================================================
void CGameScreen::StartGame()
{
	//m_pMusic->Play();
	if (m_pMusic)
		_FMOD->playSound(FMOD_CHANNEL_FREE, m_pMusic, false, &m_pChannel);

	XEN_LIST_FOREACH(t_PlayerList, ppPlayer, PlayerManager.GetActivePlayers())
	{
		(*ppPlayer)->Revive();

		if ((*ppPlayer)->GetType() == PlayerType_Pacman)
			(*ppPlayer)->SetLogicType(PlayerLogicType_Local);
		else if ((*ppPlayer)->GetType() == PlayerType_Ghost)
			(*ppPlayer)->SetLogicType(PlayerLogicType_AI);
	}

	PlayerManager.SetPlayersEnabled(true);
}

// =============================================================================
void CGameScreen::EndGame()
{
	Global.m_fMapAlpha = 1.f;

	XEN_LIST_FOREACH(t_PlayerList, ppPlayer, PlayerManager.GetActivePlayers())
	{
		if ((*ppPlayer)->GetType() == PlayerType_Ghost)
		{
			(*ppPlayer)->SetLogicType(PlayerLogicType_None);
			(*ppPlayer)->NavigateTo((*ppPlayer)->m_pStartingBlock);
		}
		else
			(*ppPlayer)->SetLogicType(PlayerLogicType_None);
	}

	if (m_pMusic)
	{
		m_pMusic->release();
		m_pMusic = NULL;
	}
}

// =============================================================================
void CGameScreen::SetState(t_GameState iGameState)
{
	m_iState = iGameState;

	switch (iGameState)
	{
	case GameState_Intro:
		ResetGame();
		break;

	case GameState_Playing:
		StartGame();
		break;

	case GameState_Finished:
		EndGame();
		break;
	}
}

// =============================================================================
void CGameScreen::OnPacmanDie(CGhost* pGhost)
{
	//m_pMusic->Stop();
	m_pDeathSound->Play();

	Global.m_fMusicEnergy = 0.1f;

	SetState(GameState_Finished);
}

// =============================================================================
void CGameScreen::GenerateMinimap()
{
	xuint iVisibleBlocks = MinimapElement_Walls | MinimapElement_GhostWalls | MinimapElement_GhostBase;
	iVisibleBlocks |= (PlayerManager.GetLocalPlayer()->GetType() == PlayerType_Pacman) ? MinimapElement_Pacman : MinimapElement_Ghost;

	if (PlayerManager.GetLocalPlayer()->GetType() == PlayerType_Ghost)
	{
		if (PlayerManager.GetLocalPlayer()->GetCurrentBlock()->IsGhostBase())
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
void CGameScreen::RenderCountdown(CRenderLayer* pLayer)
{
	static xfloat s_fMaxScale = 1.f;
	static xfloat s_fMinScale = 0.1f;

	xfloat fScale = ((s_fMaxScale - s_fMinScale) * ((xfloat)m_xCountdownTimer.TimeToExpiration() / 1000.f)) + s_fMinScale;

	m_pCountdownFont->GetMetadata()->m_fScale = fScale;
	m_pCountdownFont->Render(XFORMAT("%d", m_iCountdown), xpoint(_HSWIDTH, _HSHEIGHT - (xint)(((xfloat)m_pCountdownFont->GetFontHeight() / 2.f) * fScale)), HGETEXT_CENTER | HGETEXT_MIDDLE);
}

// =============================================================================
void CGameScreen::RenderPlayerPath(CRenderLayer* pLayer)
{
	XEN_LIST_FOREACH(t_PlayerList, ppPlayer, PlayerManager.GetActivePlayers())
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
		XEN_LIST_FOREACH(t_PlayerList, ppPlayer, PlayerManager.GetActivePlayers())
		{
			if (PlayerManager.GetLocalPlayer() == *ppPlayer)
			{
				if (*ppPlayer == PlayerManager.GetActivePlayers().back())
					ppPlayer = PlayerManager.GetActivePlayers().begin();
				else
					ppPlayer++;

				PlayerManager.SetLocalPlayer(*ppPlayer);
				Global.m_fMapAlpha = 1.f;

				break;
			}
		}
	}

	// Switch between logic types.
	if (_HGE->Input_KeyDown(HGEK_SHIFT))
	{
		t_PlayerLogicType iLogicType = PlayerManager.GetLocalPlayer()->GetLogicType();

		if (iLogicType == PlayerLogicType_Local)
			iLogicType = PlayerLogicType_AI;
		else
			iLogicType = PlayerLogicType_Local;

		PlayerManager.GetLocalPlayer()->SetLogicType(iLogicType);
	}

	// Test path-finding on the local player.
	if (_HGE->Input_KeyDown(HGEK_CTRL))
	{
		PlayerManager.GetLocalPlayer()->NavigateTo(MapManager.GetCurrentMap()->GetBlock(rand() % MapManager.GetCurrentMap()->GetBlockCount()));
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
