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

//##############################################################################

//##############################################################################
//
//                                 DEFINITION
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         11-Apr-2008
// =============================================================================
void CGameScreen::Load()
{
	LoadMap(6);
	LoadMusic();

	InitialisePlayers();

	RenderManager::Add(LayerIndex_Background, &m_xBackground);
	
	RenderManager::SetRenderCallback(LayerIndex_Map, xbind(this, &CGameScreen::WorldTransform));
	RenderManager::SetRenderCallback(LayerIndex_Player, xbind(this, &CGameScreen::WorldTransform));

	m_pFieldMask = new hgeSprite(GenerateFieldMask(48 * 3, 48 * 5), 0, 0, _SWIDTH, _SHEIGHT);

	Interface.SetCursorVisible(false);
}

// =============================================================================
// Nat Ryall                                                         15-Jul-2008
// =============================================================================
void CGameScreen::LoadMap(xint iIndex)
{
	_GLOBAL.pActiveMap = new CMap(XFORMAT("M%03d", iIndex));
	_GLOBAL.fWorldAlpha = 1.f;

	RenderManager::Add(LayerIndex_Map, _GLOBAL.pActiveMap);
}

// =============================================================================
// Nat Ryall                                                         15-Jul-2008
// =============================================================================
void CGameScreen::LoadMusic()
{
	_GLOBAL.fMusicEnergy = 0.f;

	_FMOD->createStream("Audio\\Level-Test.mp3", FMOD_SOFTWARE, 0, &m_pMusic);
	_FMOD->playSound(FMOD_CHANNEL_FREE, m_pMusic, false, &m_pChannel);
}

// =============================================================================
// Nat Ryall                                                         15-Jul-2008
// =============================================================================
void CGameScreen::InitialisePlayers()
{
	XEN_LIST_FOREACH(t_PlayerList, ppPlayer, _GLOBAL.lpPlayers)
	{
		(*ppPlayer)->Reset();
		(*ppPlayer)->SetLogicType(PlayerLogicType_AI);
		(*ppPlayer)->SetCurrentBlock(_GLOBAL.pActiveMap->GetSpawnBlock((*ppPlayer)->GetType()));

		RenderManager::Add(LayerIndex_Player, *ppPlayer);
	}

	_GLOBAL.pActivePlayer = _GLOBAL.lpPlayers.front();
	_GLOBAL.pActivePlayer->SetLogicType(PlayerLogicType_Local);
}

// =============================================================================
// Nat Ryall                                                         11-Apr-2008
// =============================================================================
void CGameScreen::Unload()
{
	delete _GLOBAL.pActiveMap;
	m_pMusic->release();

	RenderManager::Reset();

	_HGE->Texture_Free(m_pFieldMask->GetTexture());
	delete m_pFieldMask;

	Interface.SetCursorVisible(true);
}

// =============================================================================
// Nat Ryall                                                         11-Apr-2008
// =============================================================================
void CGameScreen::Update()
{
	// Exit the game.
	if (_HGE->Input_KeyUp(HGEK_ESCAPE))
	{
		ScreenManager::Pop();
		return;
	}

	// Switch between players.
	if (_HGE->Input_KeyDown(HGEK_SPACE))
	{
		XEN_LIST_FOREACH(t_PlayerList, ppPlayer, _GLOBAL.lpPlayers)
		{
			//_GLOBAL.pActivePlayer->SetLogicType(PlayerLogicType_AI);

			if (_GLOBAL.pActivePlayer == *ppPlayer)
			{
				if (*ppPlayer == _GLOBAL.lpPlayers.back())
					_GLOBAL.pActivePlayer = _GLOBAL.lpPlayers.front();
				else
				{
					ppPlayer++;

					_GLOBAL.pActivePlayer = *ppPlayer;
					_GLOBAL.fWorldAlpha = 1.f;
				}

				//_GLOBAL.pActivePlayer->SetLogicType(PlayerLogicType_Local); 
			}
		}
	}

	// Calculate the map offset.
	m_xOffset = _GLOBAL.pActivePlayer->GetSprite()->GetPosition() - xpoint(_HSWIDTH, _HSHEIGHT);

	// Calculate the music energy using spectrum analysis.
	CalculateMusicEnergy(m_pChannel);
}

// =============================================================================
// Nat Ryall                                                         11-Apr-2008
// =============================================================================
void CGameScreen::Render()
{
	if (_GLOBAL.pActivePlayer->GetType() == PlayerType_Ghost)
		m_pFieldMask->Render(0, 0);

	const char* pMusicTitle = "Unknown";

	FMOD_TAG fmArtist;
	FMOD_TAG fmTitle;

	if (m_pMusic->getTag("ARTIST", 0, &fmArtist) == FMOD_OK && m_pMusic->getTag("TITLE", 0, &fmTitle) == FMOD_OK && fmArtist.data && fmTitle.data)
		pMusicTitle = XFORMAT("%s - %s", fmArtist.data, fmTitle.data);

	_GLOBAL.pGameFont->Render(pMusicTitle, xpoint(10, 10), HGETEXT_LEFT);
}

// =============================================================================
// Nat Ryall                                                         14-Apr-2008
// =============================================================================
void CGameScreen::WorldTransform(CRenderable* pRenderable)
{
	switch (pRenderable->GetRenderableType())
	{
	case RenderableType_Map:
		{
			CMap* pMap = (CMap*)pRenderable;
			pMap->SetOffset(m_xOffset);
			pMap->Render();
		}
		break;

	case RenderableType_Player:
		{
			CPlayer* pPlayer = (CPlayer*)pRenderable;

			xpoint xPosition = pPlayer->GetSprite()->GetPosition();

			pPlayer->GetSprite()->SetPosition(xPosition - m_xOffset);
			pPlayer->Render();
			pPlayer->GetSprite()->SetPosition(xPosition);
		}
		break;

	default:
		pRenderable->Render();
	}
}

// =============================================================================
// Nat Ryall                                                         29-Apr-2008
// =============================================================================
HTEXTURE CGameScreen::GenerateFieldMask(xint iInnerRadius, xint iOuterRadius)
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

				xpoint xDistance = xpoint(abs(iX - s_xCentre.iX), abs(iY - s_xCentre.iY));
				xDistance *= xDistance;

				xint iDistance = (xint)sqrt((xfloat)xDistance.iX + (xfloat)xDistance.iY);

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
// Nat Ryall                                                          3-Jun-2008
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

	_GLOBAL.fMusicEnergy = fAverageStrength * 0.1f;
}

//##############################################################################
