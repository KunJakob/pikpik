//##############################################################################

// Global.
#include <Global.h>

// Local.
#include <Player.h>

// Other.
#include <Resource.h>
#include <Sprite.h>
#include <Game.h>
#include <Brain.h>

//##############################################################################

// The time it takes to move the player from one block to the next.
#define _MOVETIME 200

//##############################################################################

// =============================================================================
CPlayer::CPlayer(t_PlayerType iType, const xchar* pSpriteName) : CRenderable(RenderableType_Player), 
	m_iType(iType),
	m_pSprite(NULL),
	m_pNavPath(NULL),
	m_pBrain(NULL)
{
	m_iIndex = (xint)PlayerManager.m_lpPlayers.size();

	m_pSprite = new CAnimatedSprite(_SPRITE(pSpriteName));
	m_pSprite->SetAnimation("Idle");
	m_pSprite->SetAnchor(m_pSprite->GetAreaCentre());
	m_pSprite->SetEventCallback(xbind(this, &CPlayer::OnAnimationEvent));

	Reset();
}

// =============================================================================
CPlayer::~CPlayer()
{
	delete m_pSprite;

	ClearNavPath();
}

// =============================================================================
void CPlayer::Reset()
{
	m_iState = PlayerState_None;
	m_iLogicType = PlayerLogicType_None;
	m_pCurrentBlock = NULL;
	m_pTargetBlock = NULL;
	m_iTime = 0;
	m_iMoveTime = 0;
	m_fTransition = 0.f;
	m_bLeaving = false;
    m_iRequestedDir = PlayerDirection_None;
    m_iLastDir = PlayerDirection_None;
	m_iTransitionDir = PlayerDirection_Left;
	m_iMoveDir = PlayerDirection_Left;
	m_liQueuedMoves.clear();

	m_pSprite->Play("Idle");
	m_pSprite->SetAlpha(1.f);

	SetState(PlayerState_Idle);
}

// =============================================================================
void CPlayer::Revive()
{
	SetState(PlayerState_Idle);
}

// =============================================================================
void CPlayer::Update()
{
	const static xfloat s_fMoveDir[PlayerDirection_Max] = {-1.f, -1.f, 1.f, 1.f};
	static xfloat s_fBlockSize = 48.f;

	xfloat fAlpha = 1.f;

	switch (m_iState)
	{
	case PlayerState_Idle:
		{
			Logic();

			// Idle is a logic state so if we have a state now, process it immediately.
			if (m_iState != PlayerState_Idle)
				Update();
		}
		break;

	case PlayerState_Move:
		{
			// Move the player along their path.
			m_iTime = Math::Clamp<xint>(m_iTime + _TIMEDELTA, 0, m_iMoveTime);
			m_fTransition = Math::Clamp((xfloat)m_iTime / (xfloat)m_iMoveTime, 0.f, 1.f);

			m_pSprite->SetPosition(m_pCurrentBlock->GetScreenPosition() + (((m_pTargetBlock->GetScreenPosition() - m_pCurrentBlock->GetScreenPosition()) * m_iTime) / m_iMoveTime));
			
			// See if we have arrived at the next block.
			if (m_pSprite->GetPosition() == m_pTargetBlock->GetScreenPosition())
			{
				m_pCurrentBlock = m_pTargetBlock;
				m_pTargetBlock = NULL;

				SetState(PlayerState_Idle);
			}
		}
		break;

	case PlayerState_Warp:
		{
			if (m_bLeaving)
			{
				m_iTime = Math::Clamp<xint>(m_iTime + _TIMEDELTA, 0, m_iMoveTime);
				m_fTransition = Math::Clamp((xfloat)m_iTime / (xfloat)m_iMoveTime, 0.f, 1.f);

				if (m_iTime == m_iMoveTime)
				{
					m_fTransition = 1.f;
					m_pCurrentBlock = Global.m_pActiveMap->GetAdjacentBlock((t_AdjacentDirection)m_iTransitionDir, m_pCurrentBlock);
					m_iTransitionDir = (t_PlayerDirection)((m_iTransitionDir + 2) % PlayerDirection_Max);
					m_bLeaving = false;
				}
			}
			else
			{
				m_iTime = Math::Clamp<xint>(m_iTime - _TIMEDELTA, 0, m_iMoveTime);
				m_fTransition = Math::Clamp((xfloat)m_iTime / (xfloat)m_iMoveTime, 0.f, 1.f);

				if (m_iTime == 0)
				{
					m_fTransition = 0.f;
					SetState(PlayerState_Idle);
				}
			}

			xpoint xOffset;
			
			if (m_iTransitionDir % 2)
				xOffset.m_tY = (xint)(m_fTransition * s_fBlockSize * s_fMoveDir[m_iTransitionDir]);
			else
				xOffset.m_tX = (xint)(m_fTransition * s_fBlockSize * s_fMoveDir[m_iTransitionDir]);

			m_pSprite->SetPosition(m_pCurrentBlock->GetScreenPosition() + xOffset);

			if (PlayerManager.GetLocalPlayer() == this)
				Global.m_fMapAlpha = Math::Clamp(1.f - m_fTransition, 0.f, 1.f);
		}
		break;

	case PlayerState_Die:
		{
			// Burst into particle dust, the ghost should keep moving.
			// Restart the round.
		}
		break;
	}

    // Things to do while we are alive.
    if (m_iState != PlayerState_Die)
    {
        // Local players should check input every update.
        if (m_iLogicType == PlayerLogicType_Local)
        {
            // Check player input for all directions.
	        for (xuint iA = 0; iA < PlayerDirection_Max; ++iA)
	        {
		        if (_HGE->Input_GetKeyState(HGEK_LEFT + iA))
                    m_iRequestedDir = (t_PlayerDirection)iA;
            }
        }
    }

    // Calculate visibility of local player.
	if (this != PlayerManager.GetLocalPlayer())
	{
		xfloat fVisibility = (m_pCurrentBlock->m_fPlayerVisibility * (1.f - m_fTransition));

		if (m_pTargetBlock)
			fVisibility += m_pTargetBlock->m_fPlayerVisibility * m_fTransition;

		m_pSprite->SetAlpha(fVisibility * fAlpha);
	}
	else
		m_pSprite->SetAlpha(fAlpha);

	m_pSprite->Update();
}

// =============================================================================
void CPlayer::OnRender()
{
	m_pSprite->Render();
}

// =============================================================================
void CPlayer::SetCurrentBlock(CMapBlock* pBlock)
{
	m_pCurrentBlock = pBlock;

	if (m_pCurrentBlock)
		SetPosition(m_pCurrentBlock->GetScreenPosition());
}

// =============================================================================
void CPlayer::SetState(t_PlayerState iState)
{
	m_iState = iState;

	switch (m_iState)
	{
	case PlayerState_Idle:
		{
			m_pTargetBlock = m_pCurrentBlock;
		}
		break;
	}
}

// =============================================================================
void CPlayer::Move(t_PlayerDirection iDirection)
{
	m_pTargetBlock = m_pCurrentBlock->m_pAdjacents[iDirection];
    m_iLastDir = iDirection;
	m_iMoveDir = iDirection;
	m_iTransitionDir = iDirection;
	m_iTime = 0;
	m_fTransition = 0.f;

	if (m_pCurrentBlock->m_pAdjacents[iDirection])
		SetState(PlayerState_Move);
	else
	{
		m_bLeaving = true;
		SetState(PlayerState_Warp);
	}

	// Network.
	if (NetworkManager.IsRunning())
	{
		if (m_iLogicType == PlayerLogicType_Local || m_iLogicType == PlayerLogicType_AI)
		{
			BitStream xStream;

			xStream.Write((xuint8)PlayerStreamType_Move);
			xStream.Write((xuint8)m_iIndex);
			xStream.Write((xuint8)iDirection);

			NetworkManager.Broadcast(NULL, NetworkStreamType_PlayerUpdate, &xStream, HIGH_PRIORITY, RELIABLE_ORDERED);
		}
	}
}

// =============================================================================
void CPlayer::Logic()
{
	switch (m_iLogicType)
	{
	case PlayerLogicType_None:
		LogicPath();
		break;
	case PlayerLogicType_Local:	
		LogicLocal();		
		break;
	case PlayerLogicType_AI:
		LogicPath();
		LogicAI();			
		break;
	case PlayerLogicType_Remote:	
		LogicRemote();		
		break;
	}
}

// =============================================================================
void CPlayer::LogicPath()
{
	if (m_pNavPath)
	{
		// If we are on our target node, move to the next node in the sequence.
		CNavigationNode* pCurrentNode = m_pNavPath->GetCurrentNode();
		CNavigationNode* pNextNode = m_pNavPath->GetNextNode();

		if (pCurrentNode && pNextNode)
		{
			CMapBlock* pCurrentBlock = pCurrentNode->GetDataAs<CMapBlock>();
			CMapBlock* pNextBlock = pNextNode->GetDataAs<CMapBlock>();

			// Find the adjacent direction from the current block to the new block.
			for (xuint iA = 0; iA < AdjacentDirection_Max; ++iA)
			{
				if (Global.m_pActiveMap->GetAdjacentBlock((t_AdjacentDirection)iA, pCurrentBlock) == pNextBlock)
				{
					Move((t_PlayerDirection)iA);
					return;
				}
			}
		}
		else
			ClearNavPath();
	}
}

// =============================================================================
void CPlayer::LogicLocal()
{
	// Currently we can only control one player at a time.
	if (this != PlayerManager.GetLocalPlayer() || !Global.m_bWindowFocused)
		return;

    // If we have a pending request, try to follow it.
    if (m_iRequestedDir != PlayerDirection_None && IsPassable(m_pCurrentBlock->m_pAdjacents[m_iRequestedDir]))
    {
        Move(m_iRequestedDir);
        m_iRequestedDir = PlayerDirection_None;
    }

    // If we haven't picked a new direction...
    if (m_iState == PlayerState_Idle)
    {
        // Try to continue the way we were going before.
        if (m_iLastDir != PlayerDirection_None && IsPassable(m_pCurrentBlock->m_pAdjacents[m_iLastDir]))
            Move(m_iLastDir);
    }
}

// =============================================================================
void CPlayer::LogicAI()
{
	if (m_pBrain)
		m_pBrain->Think();
}

// =============================================================================
void CPlayer::LogicRemote()
{
	if (m_liQueuedMoves.size())
	{
		while (m_liQueuedMoves.size() > 1)
		{
			m_pCurrentBlock = m_pCurrentBlock->m_pAdjacents[m_liQueuedMoves.front()];
			m_liQueuedMoves.pop_front();
		}

		Move(m_liQueuedMoves.front());
		m_liQueuedMoves.pop_front();
	}
}

// =============================================================================
void CPlayer::SetLogicType(t_PlayerLogicType _Value)
{
	ClearNavPath();

	m_iLogicType = _Value;
}

// =============================================================================
void CPlayer::NavigateTo(CMapBlock* pBlock)
{
	CNavigationRequest xRequest;
	CNavigationPath* pPath = new CNavigationPath();
	CMapEvaluator* pEvaluator = new CMapEvaluator(this);

	xRequest.m_pMesh = Global.m_pActiveMap->GetNavMesh();
	xRequest.m_pEvaluator = pEvaluator;
	xRequest.m_pStart = m_pTargetBlock ? m_pTargetBlock->m_pNavNode : m_pCurrentBlock->m_pNavNode;
	xRequest.m_pGoal = pBlock->m_pNavNode;

	NavigationManager.FindPath(&xRequest, *pPath);

	delete pEvaluator;

	SetNavPath(pPath);
}

// =============================================================================
void CPlayer::SetNavPath(CNavigationPath* pPath)
{
	ClearNavPath();

	if (pPath->GetNodeCount())
		m_pNavPath = pPath;
}

// =============================================================================
void CPlayer::ClearNavPath()
{
	if (m_pNavPath)
		delete m_pNavPath;

	m_pNavPath = NULL;
}

// =============================================================================
void CPlayer::OnAnimationEvent(CAnimatedSprite* pSprite, const xchar* pEvent)
{
	if (String::IsMatch(pEvent, "Eat"))
	{
		if (m_pTargetBlock && m_pTargetBlock->IsEdible())
			m_pTargetBlock->Eat();
	}

	if (String::IsMatch(pEvent, "Dead"))
	{
		Global.m_fMusicEnergy = 0.002f;
	}
}

// =============================================================================
void CPlayer::OnReceivePlayerUpdate(CNetworkPeer* pFrom, BitStream* pStream)
{
	xuint8 iStreamType;
	xuint8 iPlayerIndex;

	pStream->Read(iStreamType);
	pStream->Read(iPlayerIndex);

    CPlayer* pPlayer = (iPlayerIndex < PlayerManager.GetPlayerCount()) ? PlayerManager.GetPlayer(iPlayerIndex) : NULL;

	if (pPlayer)
	{
		switch (iStreamType)
		{
		// Move.
		case PlayerStreamType_Move:
			{
				xuint8 iMoveDirection;
				pStream->Read(iMoveDirection);

				pPlayer->m_liQueuedMoves.push_back((t_PlayerDirection)iMoveDirection);
			}
			break;
		}
	}
}

//##############################################################################

// =============================================================================
CPacman::CPacman() : CPlayer(PlayerType_Pacman, "Player-Pacman"), CCollidable(CollisionGroup_Player, CollisionType_Circle)
{
	SetState(PlayerState_Idle);
}

// =============================================================================
void CPacman::Update()
{
	CPlayer::Update();
}

// =============================================================================
void CPacman::SetState(t_PlayerState iState)
{
	switch (iState)
	{
	case PlayerState_Idle:
		{
			if (!m_pSprite->IsActiveAnimation("Idle"))
				m_pSprite->Play("Idle");
		}
		break;

	case PlayerState_Move:
	case PlayerState_Warp:
		{
			m_pSprite->Play("Move");
			m_pSprite->SetAngle((xfloat)m_iTransitionDir * 90.f, true);

			m_iMoveTime = _MOVETIME;
		}
		break;

	case PlayerState_Die:
		{
			m_pSprite->Play("Die");
		}
		break;
	}

	CPlayer::SetState(iState);
}

// =============================================================================
xcircle CPacman::GetCollisionCircle()
{
	return xcircle(m_pSprite->GetPosition(), m_pSprite->GetAreaWidth() / 3);
}

// =============================================================================
xbool CPacman::IsCollidable(CCollidable* pWith)
{
	return (dynamic_cast<CPlayer*>(pWith) != NULL) && m_iState != PlayerState_Die;
}

// =============================================================================
void CPacman::OnCollision(CCollidable* pWith)
{
	// If we collided with a ghost player.
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pWith);

	if (pPlayer && pPlayer->GetType() == PlayerType_Ghost)
	{
		CGameScreen* pGameScreen = (CGameScreen*)ScreenManager.FindScreen(ScreenIndex_GameScreen);

		if (pGameScreen)
			pGameScreen->OnPacmanDie((CGhost*)pPlayer);

		SetState(PlayerState_Die);
	}
}

//##############################################################################

// =============================================================================
CGhost::CGhost(xuint iColour) : CPlayer(PlayerType_Ghost, "Player-Ghost"), CCollidable(CollisionGroup_Player, CollisionType_Circle),
	m_pEyes(NULL),
	m_iColour(iColour)
{
	m_pBrain = new CGhostBrain(this);

	m_pEyes = new CSprite(_SPRITE("Player-Ghost-Eyes"));
	m_pEyes->SetArea("F1");
	m_pEyes->SetAnchor(m_pEyes->GetAreaCentre());

	SetState(PlayerState_Idle);
}

// =============================================================================
CGhost::~CGhost()
{
	delete m_pBrain;
	delete m_pEyes;
}

// =============================================================================
void CGhost::Update()
{
	CPlayer::Update();
}

// =============================================================================
void CGhost::OnRender()
{
	m_pSprite->GetMetadata()->GetSprite()->SetColor(m_iColour);

	CPlayer::OnRender();

	m_pEyes->SetPosition(m_pSprite->GetPosition());

	if (PlayerManager.GetLocalPlayer()->GetType() == PlayerType_Pacman || PlayerManager.GetLocalPlayer() == this)
		m_pEyes->SetAlpha(m_pSprite->GetAlpha());
	else
		m_pEyes->SetAlpha(1.f);
	
	m_pEyes->Render();
}

// =============================================================================
void CGhost::SetState(t_PlayerState iState)
{
	switch (iState)
	{
	case PlayerState_Idle:
		{
			if (!m_pSprite->IsActiveAnimation("Idle"))
				m_pSprite->Play("Idle");
		}
		break;

	case PlayerState_Move:
	case PlayerState_Warp:
		{
			m_pEyes->SetArea(XFORMAT("F%d", m_iTransitionDir + 1));
			m_iMoveTime = m_pSprite->GetAnimation()->m_iAnimationTime;

			if (m_pCurrentBlock->m_iTileType == TileType_Entrance || (m_pTargetBlock && m_pTargetBlock->m_iTileType == TileType_Entrance))
				m_iMoveTime *= 3;
		}
		break;
	}

	CPlayer::SetState(iState);
}

// =============================================================================
xcircle CGhost::GetCollisionCircle()
{
	return xcircle(m_pSprite->GetPosition(), m_pSprite->GetAreaWidth() / 3);
}

// =============================================================================
xbool CGhost::IsCollidable(CCollidable* pWith)
{
	return true;
}

// =============================================================================
void CGhost::OnCollision(CCollidable* pWith)
{
}

//##############################################################################

// =============================================================================
void CPlayerManager::OnInitialise()
{
    // Create all the available players.
	m_lpPlayers.push_back(new CPacman());
    m_lpPlayers.push_back(new CPacman());
	m_lpPlayers.push_back(new CGhost(0xFF40F0F0));
	m_lpPlayers.push_back(new CGhost(0xFFF04040));
	m_lpPlayers.push_back(new CGhost(0xFF4040F0));
	m_lpPlayers.push_back(new CGhost(0xFFF0F040));
	m_lpPlayers.push_back(new CGhost(0xFFF040F0));
}

// =============================================================================
void CPlayerManager::OnDeinitialise()
{
    // Create all the available players.
	XEN_LIST_ERASE_ALL(m_lpPlayers);
}

// =============================================================================
void CPlayerManager::InitialisePlayers(t_PlayMode iPlayMode)
{
    switch (iPlayMode)
    {
    case PlayMode_Online:
        {
            ResetActivePlayers();
	
	        // Initialise all players.
	        for (xint iA = 0; iA < GetActivePlayerCount(); ++iA)
		        GetActivePlayer(iA)->SetLogicType(NetworkManager.GetLocalPeer()->m_bHost ? PlayerLogicType_AI : PlayerLogicType_Remote);

	        // Setup all network players.
	        xint iPlayerIndex = 0;

	        XEN_LIST_FOREACH(t_NetworkPeerList, ppPeer, NetworkManager.GetVerifiedPeers())
	        {
		        CNetworkPeer* pPeer = *ppPeer;
		        CNetworkPeerInfo* pInfo = GetPeerInfo(pPeer);

		        pInfo->m_pPlayer = PlayerManager.GetActivePlayer(iPlayerIndex++);
		        pInfo->m_pPlayer->SetLogicType(pPeer->m_bLocal ? PlayerLogicType_Local : PlayerLogicType_Remote);

		        if (pPeer->m_bLocal)
			        PlayerManager.GetLocalPlayer() = pInfo->m_pPlayer;
	        }
        }
        break;
    }
}

// =============================================================================
void CPlayerManager::ResetActivePlayers()
{
    xint iPacmanCount = Global.m_pActiveMap->GetPacmanCount();
	xint iGhostCount = Global.m_pActiveMap->GetGhostCount();

	m_lpActivePlayers.clear();

	XEN_LIST_FOREACH(t_PlayerList, ppPlayer, m_lpPlayers)
	{
		CPlayer* pPlayer = *ppPlayer;
		xbool bPlaying = false;

		pPlayer->Reset();

		switch (pPlayer->GetType())
		{
		case PlayerType_Ghost:
			bPlaying = (iGhostCount-- > 0);
			break;

		case PlayerType_Pacman:
			bPlaying = (iPacmanCount-- > 0);
			break;
		}

		if (bPlaying)
		{
			m_lpActivePlayers.push_back(pPlayer);

			pPlayer->SetCurrentBlock(Global.m_pActiveMap->GetSpawnBlock(pPlayer->GetType()));
			pPlayer->m_pStartingBlock = pPlayer->GetCurrentBlock();
		}
	}
}