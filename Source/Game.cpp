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

//##############################################################################

// =============================================================================
void CGameScreen::OnActivate()
{
	Global.m_fScreenAlpha = 1.f;
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
}

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
		RenderLayer(GameLayerIndex_Player)->AttachRenderable(*ppPlayer);
}

// =============================================================================
void CGameScreen::OnDeactivate()
{
	CollisionManager.Reset();

	//m_pMusic->release();

	delete m_pMinimap;

	delete m_xRenderView;
	m_xRenderView = NULL;

	RenderManager.ClearRenderView();
}

// =============================================================================
void CGameScreen::OnWake()
{
}

// =============================================================================
void CGameScreen::OnSleep()
{
}

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
				Global.m_fScreenAlpha = 1.f;

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

	// Update all the players.
	XEN_LIST_FOREACH(t_PlayerList, ppPlayer, Global.m_lpActivePlayers)
	{
		(*ppPlayer)->Update();
	}

	// Calculate the music energy using spectrum analysis.
	Global.m_fMusicEnergy = 0.f;

	// Generate the minimap.
	xuint iVisibleBlocks = MinimapElement_Walls | MinimapElement_GhostWalls | MinimapElement_GhostBase;
	iVisibleBlocks |= (Global.m_pLocalPlayer->GetType() == PlayerType_Pacman) ? MinimapElement_Pacman : MinimapElement_Ghost;

	m_pMinimap->Generate(iVisibleBlocks);
}

// =============================================================================
void CGameScreen::OnPreRender()
{
	// Set the transformations.
	xpoint xOffset = (Global.m_pLocalPlayer->GetSprite()->GetPosition() - xpoint(_HSWIDTH, _HSHEIGHT)) * -1;

	RenderLayer(GameLayerIndex_Map)->SetTransformation(xOffset);
	RenderLayer(GameLayerIndex_Player)->SetTransformation(xOffset);
}

//##############################################################################