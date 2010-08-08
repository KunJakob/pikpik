//##############################################################################

// Global.
#include <Global.h>

// Local.
#include <Player.h>

// Other.
#include <Resource.h>
#include <Sprite.h>

//##############################################################################

// The time it takes to move the player from one block to the next.
#define _MOVETIME 200

//##############################################################################

// =============================================================================
CPlayer::CPlayer(t_PlayerType iType, const xchar* pSpriteName) : CRenderable(RenderableType_Player), 
	m_iType(iType),
	m_pSprite(NULL)
{
	m_iIndex = (xint)Global.m_lpPlayers.size();

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
	m_iTransitionDir = PlayerDirection_Left;
	m_iMoveDir = PlayerDirection_Left;
	m_liQueuedMoves.clear();

	m_pSprite->Play("Idle");
	m_pSprite->SetAlpha(1.f);

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

			if (Global.m_pLocalPlayer == this)
				Global.m_fScreenAlpha = Math::Clamp(1.f - m_fTransition, 0.f, 1.f);
		}
		break;

	case PlayerState_Die:
		{
			// Burst into particle dust, the ghost should keep moving.
			// Restart the round.
		}
		break;
	}

	if (this != Global.m_pLocalPlayer)
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
	case PlayerLogicType_Local:		LogicLocal();		break;
	case PlayerLogicType_AI:		LogicAI();			break;
	case PlayerLogicType_Remote:	LogicRemote();		break;
	}
}

// =============================================================================
void CPlayer::LogicLocal()
{
	// Currently we can only control one player at a time.
	if (this != Global.m_pLocalPlayer || !Global.m_bWindowFocused)
		return;

	// Check player input for all directions.
	for (xuint iA = 0; iA < PlayerDirection_Max; ++iA)
	{
		if (_HGE->Input_GetKeyState(HGEK_LEFT + iA))
		{
			// If we can move, begin the transition to the next block.
			if (!m_pCurrentBlock->m_pAdjacents[iA] || IsPassable(m_pCurrentBlock->m_pAdjacents[iA]))
			{
				Move((t_PlayerDirection)iA);
				break;
			}
		}
	}
}

// =============================================================================
void CPlayer::LogicAI()
{
	// Check for a retreat mode/chase mode... otherwise...
	BehaviourWander();
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
void CPlayer::BehaviourWander()
{
	CMapBlock* pMoveDirection[PlayerDirection_Max];
	t_PlayerDirection iRealDirection[PlayerDirection_Max];
	xint iDirectionCount = 0;

	for (xint iA = 0; iA < PlayerDirection_Max; ++iA)
	{
		iRealDirection[iA] = (t_PlayerDirection)((m_iMoveDir + iA + 3) % PlayerDirection_Max);
		pMoveDirection[iA] = Global.m_pActiveMap->GetAdjacentBlock((t_AdjacentDirection)iRealDirection[iA], m_pCurrentBlock);

		if (IsPassable(pMoveDirection[iA]))
			iDirectionCount++;
		else
			pMoveDirection[iA] = NULL;		
	}

	// If we have no choice, 
	if (iDirectionCount == 1)
	{
		for (xint iA = 0; iA < PlayerDirection_Max; ++iA)
		{
			if (pMoveDirection[iA])
			{
				Move(iRealDirection[iA]);
				break;
			}
		}
	}
	// Otherwise pick a random path to move down.
	else
	{
		xint iRandomDir = rand() % (iDirectionCount - 1);

		for (xint iA = 0; iA < PlayerDirection_Max; ++iA)
		{
			if (pMoveDirection[iA])
				iRandomDir--;

			if (iRandomDir == -1)
			{
				Move(iRealDirection[iA]);
				break;
			}
		}
	}
}

// =============================================================================
void CPlayer::OnAnimationEvent(CAnimatedSprite* pSprite, const xchar* pEvent)
{
	if (String::IsMatch(pEvent, "Eat"))
	{
		if (m_pTargetBlock && m_pTargetBlock->IsEdible())
			m_pTargetBlock->Eat();
	}
}

// =============================================================================
void CPlayer::OnReceivePlayerUpdate(CNetworkPeer* pFrom, BitStream* pStream)
{
	xuint8 iStreamType;
	xuint8 iPlayerIndex;

	pStream->Read(iStreamType);
	pStream->Read(iPlayerIndex);

	CPlayer* pPlayer = (iPlayerIndex < Global.m_lpPlayers.size()) ? Global.m_lpPlayers[iPlayerIndex] : NULL;

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
	}

	CPlayer::SetState(iState);
}

// =============================================================================
xcircle CPacman::GetCollisionCircle()
{
	return xcircle(m_pSprite->GetPosition(), m_pSprite->GetAreaWidth() / 2);
}

// =============================================================================
xbool CPacman::IsCollidable(CCollidable* pWith)
{
	return false;
}

// =============================================================================
void CPacman::OnCollision(CCollidable* pWith)
{
}

//##############################################################################

// =============================================================================
CGhost::CGhost(xuint iColour) : CPlayer(PlayerType_Ghost, "Player-Ghost"), CCollidable(CollisionGroup_Player, CollisionType_Circle),
	m_pEyes(NULL),
	m_iColour(iColour)
{
	m_pEyes = new CSprite(_SPRITE("Player-Ghost-Eyes"));
	m_pEyes->SetArea("F1");
	m_pEyes->SetAnchor(m_pEyes->GetAreaCentre());

	SetState(PlayerState_Idle);
}

// =============================================================================
CGhost::~CGhost()
{
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

	if (Global.m_pLocalPlayer->GetType() == PlayerType_Pacman || Global.m_pLocalPlayer == this)
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
	return xcircle(m_pSprite->GetPosition(), m_pSprite->GetAreaWidth() / 2);
}

// =============================================================================
xbool CGhost::IsCollidable(CCollidable* pWith)
{
	return false;
}

// =============================================================================
void CGhost::OnCollision(CCollidable* pWith)
{
}

//##############################################################################
