/**
* @file Game.cpp
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

// Local.
#include <Game.h>

//##############################################################################
#pragma endregion

#pragma region Static
//##############################################################################
//
//                                   STATIC
//
//##############################################################################

// The world transform offset.
static XPOINT s_xOffset;

//##############################################################################
#pragma endregion

#pragma region Local
//##############################################################################
//
//                                   LOCAL
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         14-Apr-2008
// =============================================================================
void WorldTransform(CRenderable* pRenderable)
{
	switch (pRenderable->GetRenderableType())
	{
	case RenderableType_Map:
		{
			CMap* pMap = (CMap*)pRenderable;
			pMap->SetOffset(s_xOffset);
			pMap->Render();
		}
		break;

	case RenderableType_Player:
		{
			CPlayer* pPlayer = (CPlayer*)pRenderable;

			XPOINT xPosition = pPlayer->GetSprite()->GetPosition();

			pPlayer->GetSprite()->SetPosition(xPosition - s_xOffset);
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
HTEXTURE GenerateFieldMask(xint iInnerRadius, xint iOuterRadius)
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
// Nat Ryall                                                          2-Jun-2008
// =============================================================================
xfloat GetSpectrumValue(FMOD::Channel* pChannel)
{
	/*xfloat fSpectrum[64];
	pChannel->getSpectrum(fSpectrum, 64, 0, FMOD_DSP_FFT_WINDOW_RECT);

	xfloat fFrequency = 0.f;
	pChannel->getFrequency(&fFrequency);

	xfloat fMax = 0.f;
	xfloat fAverage = 0.f;

	for (xint iA = 0; iA < 64; ++iA)
	{
	if (fSpectrum[iA] > fMax)
	fMax = fSpectrum[iA];
	}

	xfloat fNormalise = 1.f / fMax;

	for (xint iA = 0; iA < 64; ++iA)
	fSpectrum[iA] *= fNormalise;

	for (xint iA = 0; iA < 64; ++iA)
	fAverage += fSpectrum[iA];*/

	//xfloat fValue = (fFrequency / 2.f) / 64.f;

	//float fLevel = 0.f;
	//pChannel->getSpeakerLevels(FMOD_SPEAKER_MONO, &fLevel, 1);

	return Math::Clamp(1.f, 0.f, 1.f);
}

//##############################################################################
#pragma endregion

#pragma region Defenition
//##############################################################################
//
//                                 DEFENITION
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         11-Apr-2008
// =============================================================================
void CGameScreen::Load()
{
	_GLOBAL.pActiveMap = new CMap(XFORMAT("M%03d", 4));

	_GLOBAL.lpPlayers.push_back(new CPacMan(_GLOBAL.pActiveMap->GetSpawnBlock(PlayerType_PacMan)));
	_GLOBAL.lpPlayers.push_back(new CGhost(_GLOBAL.pActiveMap->GetSpawnBlock(PlayerType_Ghost)));
	_GLOBAL.lpPlayers.push_back(new CGhost(_GLOBAL.pActiveMap->GetSpawnBlock(PlayerType_Ghost)));
	_GLOBAL.lpPlayers.push_back(new CGhost(_GLOBAL.pActiveMap->GetSpawnBlock(PlayerType_Ghost)));

	_GLOBAL.pActivePlayer = _GLOBAL.lpPlayers.front();

	RenderManager::Add(LayerIndex_Background, &m_Background);
	RenderManager::Add(LayerIndex_Map, _GLOBAL.pActiveMap);

	XEN_LIST_FOREACH(t_PlayerList, ppPlayer, _GLOBAL.lpPlayers)
		RenderManager::Add(LayerIndex_Player, *ppPlayer);

	RenderManager::SetRenderCallback(LayerIndex_Map, &WorldTransform);
	RenderManager::SetRenderCallback(LayerIndex_Player, &WorldTransform);

	m_pFieldMask = new hgeSprite(GenerateFieldMask(48 * 1, 48 * 5), 0, 0, _SWIDTH, _SHEIGHT);

	// Start the game music.
	_GLOBAL.fMusicEnergy = 0.f;

	_FMOD->createStream("Audio\\Level-Test.mp3", FMOD_SOFTWARE, 0, &m_pMusic);
	_FMOD->playSound(FMOD_CHANNEL_FREE, m_pMusic, false, &m_pChannel);
}

// =============================================================================
// Nat Ryall                                                         11-Apr-2008
// =============================================================================
void CGameScreen::Unload()
{
	m_pMusic->release();

	_HGE->Texture_Free(m_pFieldMask->GetTexture());
	delete m_pFieldMask;

	RenderManager::Reset();

	XEN_LIST_ERASE_ALL(_GLOBAL.lpPlayers);
	delete _GLOBAL.pActiveMap;
}

// =============================================================================
// Nat Ryall                                                         11-Apr-2008
// =============================================================================
void CGameScreen::Update()
{
	// Exit the game.
	if (_HGE->Input_KeyDown(HGEK_ESCAPE))
	{
		ScreenManager::Pop();
		return;
	}

	// Switch between players.
	if (_HGE->Input_KeyDown(HGEK_SPACE))
	{
		XEN_LIST_FOREACH(t_PlayerList, ppPlayer, _GLOBAL.lpPlayers)
		{
			if (_GLOBAL.pActivePlayer == *ppPlayer)
			{
				if (*ppPlayer == _GLOBAL.lpPlayers.back())
					_GLOBAL.pActivePlayer = _GLOBAL.lpPlayers.front();
				else
				{
					ppPlayer++;
					_GLOBAL.pActivePlayer = *ppPlayer;
				}
			}
		}
	}

	// Calculate the map offset.
	s_xOffset = _GLOBAL.pActivePlayer->GetSprite()->GetPosition() - XPOINT(_HSWIDTH, _HSHEIGHT);

	// Calculate the spectrum value.
	//_GLOBAL.fSpectrum = GetSpectrumValue(m_pChannel);
	XLOG("[CGameScreen] Spectrum: %f", _GLOBAL.fMusicEnergy);
}

// =============================================================================
// Nat Ryall                                                         11-Apr-2008
// =============================================================================
void CGameScreen::Render()
{
	if (_GLOBAL.pActivePlayer->GetType() == PlayerType_Ghost)
		m_pFieldMask->Render(0, 0);

	// Spectrum analysis.
	const static xint s_iIterations = 2048;
	const static xint s_iSearch = 8;

	xfloat fSpectrum[2][s_iIterations];

	m_pChannel->getSpectrum(fSpectrum[0], s_iIterations, 0, FMOD_DSP_FFT_WINDOW_HANNING);
	m_pChannel->getSpectrum(fSpectrum[1], s_iIterations, 1, FMOD_DSP_FFT_WINDOW_HANNING);

	xfloat fStrength[s_iSearch];

	for (xint iA = 0; iA < s_iSearch; ++iA)
		fStrength[iA] = fSpectrum[0][iA] + fSpectrum[1][iA];

	xfloat fAverageStrength = 0.f;

	for (xint iA = 4; iA < s_iSearch; ++iA)
		fAverageStrength += fStrength[iA];

	fAverageStrength /= 4.f;

	_GLOBAL.fMusicEnergy = fAverageStrength * 0.1f;

	// Draw the beat-spectrum.
	xfloat fX = 0.f;

	for (xint iA = 4; iA < s_iSearch; ++iA)
	{
		for (xint iB = 0; iB < 8; ++iB)
		{
			_HGE->Gfx_RenderLine(fX, 0.f, fX, fStrength[iA] * 200.f, 0xFFFF00FF);
			fX += 1.f;
		}

		fX += 1.f;
	}

	/*xfloat fWaveDataL[s_iIterations];
	m_pChannel->getWaveData(fWaveDataL, s_iIterations, 0);
	xfloat fWaveDataR[s_iIterations];
	m_pChannel->getWaveData(fWaveDataR, s_iIterations, 1);

	xfloat fEnergy = 0.f;

	for (xint iA = 1; iA < s_iIterations; ++iA)
	fEnergy += abs(fWaveDataL[iA] - fWaveDataL[iA - 1]);// + abs(fWaveDataR[iA]);

	fEnergy /= s_iIterations;
	_GLOBAL.fMusicEnergy = fEnergy * 0.25f;//Math::Clamp(fEnergy - 0.2f, 0.0f, 0.8f) * 0.1f;

	_HGE->Gfx_RenderLine(10.f, 10.f, 10.f, 10.f + (fEnergy * 100.f), 0xFFFFFFFF);*/
}

//##############################################################################
#pragma endregion