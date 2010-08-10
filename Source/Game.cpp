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
	RenderLayer(GameLayerIndex_Background)->AttachRenderable(&m_xBackground);
	RenderLayer(GameLayerIndex_Map)->AttachRenderable(Global.m_pActiveMap);

	InitialisePlayers();

	m_pMinimap = new CMinimap(Global.m_pActiveMap);
	RenderLayer(GameLayerIndex_Radar)->AttachRenderable(m_pMinimap);

	m_pGhostMask = new hgeSprite(GenerateGhostMask(48 * 3, 48 * 5), 0, 0, _SWIDTH, _SHEIGHT);
	RenderLayer(GameLayerIndex_GhostMask)->SetRenderOverride(xbind(this, &CGameScreen::RenderGhostMask));

	// Load the music to associate with the map colourisation.
	m_pMusic = new CSound(_SOUND("Game-Music"));
	m_pMusic->Play();

	// Load the sound effects.
	m_pDeathSound = new CSound(_SOUND("Game-Death"));
}

// =============================================================================
void CGameScreen::OnDeactivate()
{
	CollisionManager.Reset();

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
		DebugCharacterSwitch();
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

			pPlayer->SetLogicType(PlayerLogicType_AI);

			if (pPlayer->GetType() == PlayerType_Pacman)
				CollisionManager.Add((CPacman*)pPlayer);
			else if (pPlayer->GetType() == PlayerType_Ghost)
				CollisionManager.Add((CGhost*)pPlayer);
		}
	}

	// Add all active players to the player render layer.
	XEN_LIST_FOREACH(t_PlayerList, ppPlayer, Global.m_lpActivePlayers)
		RenderLayer(GameLayerIndex_Player)->AttachRenderable(*ppPlayer);
}

// =============================================================================
HTEXTURE CGameScreen::GenerateGhostMask(xint iInnerRadius, xint iOuterRadius)
{
	xint iRadiusDifference = iOuterRadius - iInnerRadius;

	static xint s_iWidth = _SWIDTH;
	static xint s_iHeight = _SHEIGHT;
	static xpoint s_xCentre = xpoint(_HSWIDTH, _HSHEIGHT);

	HTEXTURE hFieldMask = _HGE->Texture_Create(s_iWidth, s_iHeight);

	DWORD* pTexMem = _HGE->Texture_Lock(hFieldMask, false);
	{
		for (xint iY = 0; iY < s_iHeight; ++iY)
		{
			for (xint iX = 0; iX < s_iWidth; ++iX)
			{
				DWORD* pPixel = &pTexMem[iX + (iY * s_iWidth)];
				xint iAlpha = 255;

				xpoint xDistance = xpoint(abs(iX - s_xCentre.m_tX), abs(iY - s_xCentre.m_tY));
				xDistance *= xDistance;

				xint iDistance = (xint)sqrt((xfloat)xDistance.m_tX + (xfloat)xDistance.m_tY);

				if (iDistance < iInnerRadius)
					iAlpha = 0;
				else if (iDistance < iOuterRadius)
					iAlpha = ((iDistance - iInnerRadius) * 255) / iRadiusDifference;

				*pPixel = ARGB(Math::Clamp<xint>(iAlpha, 0, 255), 0, 0, 0);
			}
		}
	}
	_HGE->Texture_Unlock(hFieldMask);

	return hFieldMask;
}

// =============================================================================
void CGameScreen::RenderGhostMask(CRenderLayer* pLayer)
{
	if (Global.m_pLocalPlayer->GetType() == PlayerType_Ghost)
		m_pGhostMask->Render(0.0f, 0.0f);
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
void CGameScreen::OnPacmanDie(CGhost* pGhost)
{
	m_pMusic->Stop();
	m_pDeathSound->Play();

	Global.m_fMusicEnergy = 0.2f;

	XEN_LIST_FOREACH(t_PlayerList, ppPlayer, Global.m_lpActivePlayers)
	{
		if ((*ppPlayer)->GetType() == PlayerType_Ghost)
			(*ppPlayer)->SetLogicType(PlayerLogicType_AI);
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
void CGameScreen::DebugCharacterSwitch()
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
}

//##############################################################################