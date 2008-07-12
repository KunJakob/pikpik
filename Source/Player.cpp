#pragma region Include
//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Global.
#include <Global.h>

// Local.
#include <Player.h>

// Other.
#include <Resource.h>
#include <Sprite.h>

//##############################################################################
#pragma endregion

#pragma region Macros
//##############################################################################
//
//                                   MACROS
//
//##############################################################################
#define PLAYER_MOVETIME 200

//##############################################################################
#pragma endregion

#pragma region Base
//##############################################################################
//
//                                    BASE
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         13-Apr-2008
// =============================================================================
CPlayer::CPlayer(t_PlayerType iType, const XCHAR* pSpriteName) : CRenderable(RenderableType_Player), 
	m_iType(iType),
	m_iLogicType(PlayerLogicType_None),
	m_iState(PlayerState_None),
	m_pSprite(NULL),
	m_pCurrentBlock(NULL),
	m_pTargetBlock(NULL),
	m_iTime(0),
	m_iMoveTime(0),
	m_fTransition(0.f),
	m_bLeaving(false),
	m_iTransitionDir(MoveDirection_Left),
	m_iMoveDir(MoveDirection_Left)
{
	m_pSprite = new CAnimatedSprite(_SPRITE(pSpriteName));
	m_pSprite->SetAnimation("Idle", true);
	m_pSprite->SetAnchor(m_pSprite->GetAreaCentre());
	m_pSprite->SetEventCallback(xbind(this, &CPlayer::OnAnimationEvent));

	SetName("Unknown");
}

// =============================================================================
// Nat Ryall                                                         13-Apr-2008
// =============================================================================
CPlayer::~CPlayer()
{
	delete m_pSprite;
}

// =============================================================================
// Nat Ryall                                                         13-Apr-2008
// =============================================================================
void CPlayer::Update()
{
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
			m_iTime = Math::Clamp<XUINT>(m_iTime + _TIMEDELTA, 0, m_iMoveTime);
			m_fTransition = Math::Clamp((XFLOAT)m_iTime / (XFLOAT)m_iMoveTime, 0.f, 1.f);

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
			const static xfloat s_fMoveDir[MoveDirection_Max] = {-1.f, -1.f, 1.f, 1.f};
			static xfloat s_fBlockSize = 48.f;

			xfloat fChange = _TIMEDELTAF * m_iMoveTime;

			if (m_bLeaving)
			{
				m_fTransition += fChange;

				if (m_fTransition > s_fBlockSize)
				{
					m_fTransition = s_fBlockSize;
					m_pCurrentBlock = _GLOBAL.pActiveMap->GetAdjacentBlock((t_AdjacentDir)m_iTransitionDir, m_pCurrentBlock);
					m_iTransitionDir = (t_MoveDirection)((m_iTransitionDir + 2) % MoveDirection_Max);
					m_bLeaving = false;
				}
			}
			else
			{
				m_fTransition -= fChange;

				if (m_fTransition < 0.f)
				{
					m_fTransition = 0.f;
					SetState(PlayerState_Idle);
				}
			}

			xpoint xOffset;
			
			if (m_iTransitionDir % 2)
				xOffset.iY = (xint)(m_fTransition * s_fMoveDir[m_iTransitionDir]);
			else
				xOffset.iX = (xint)(m_fTransition * s_fMoveDir[m_iTransitionDir]);

			m_pSprite->SetPosition(m_pCurrentBlock->GetScreenPosition() + xOffset);
			fAlpha = 1.f - (m_fTransition / s_fBlockSize);

			if (_GLOBAL.pActivePlayer == this)
				_GLOBAL.fWorldAlpha = Math::Clamp(1.f - (m_fTransition / s_fBlockSize), 0.f, 1.f);
		}
		break;

	case PlayerState_Die:
		{
			// Burst into particle dust, the ghost should keep moving.
			// Restart the round.
		}
		break;
	}

	if (this != _GLOBAL.pActivePlayer)
		m_pSprite->SetAlpha(m_pCurrentBlock->fVisibility * fAlpha);
	else
		m_pSprite->SetAlpha(fAlpha);

	m_pSprite->Update();
}

// =============================================================================
// Nat Ryall                                                         13-Apr-2008
// =============================================================================
void CPlayer::Render()
{
	m_pSprite->Render();
}

// =============================================================================
// Nat Ryall                                                         09-Jul-2008
// =============================================================================
void CPlayer::SetCurrentBlock(CMapBlock* pBlock)
{
	m_pCurrentBlock = pBlock;

	if (m_pCurrentBlock)
		SetPosition(m_pCurrentBlock->GetScreenPosition());
}

// =============================================================================
// Nat Ryall                                                         16-Apr-2008
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
// Nat Ryall                                                         11-Jul-2008
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
// Nat Ryall                                                         11-Jul-2008
// =============================================================================
void CPlayer::LogicLocal()
{
	// Currently we can only control one player at a time.
	if (this != _GLOBAL.pActivePlayer)
		return;

	// Check player input for all directions.
	for (XUINT iA = 0; iA < MoveDirection_Max; ++iA)
	{
		if (_HGE->Input_GetKeyState(HGEK_LEFT + iA))
		{
			// If we can move, begin the transition to the next block.
			if (!m_pCurrentBlock->pAdjacents[iA] || IsPassable(m_pCurrentBlock->pAdjacents[iA]))
				Move((t_MoveDirection)iA);
		}
	}
}

// =============================================================================
// Nat Ryall                                                         11-Jul-2008
// =============================================================================
void CPlayer::LogicAI()
{
	// Check for a retreat mode/chase mode... otherwise...

	// Wander logic.
	CMapBlock* pMoveDirection[MoveDirection_Max];
	t_MoveDirection iRealDirection[MoveDirection_Max];
	xint iDirectionCount = 0;

	for (xint iA = 0; iA < MoveDirection_Max; ++iA)
	{
		iRealDirection[iA] = (t_MoveDirection)((m_iMoveDir + iA + 3) % MoveDirection_Max);
		pMoveDirection[iA] = _GLOBAL.pActiveMap->GetAdjacentBlock((t_AdjacentDir)iRealDirection[iA], m_pCurrentBlock);

		if (!pMoveDirection[iA]->IsWall())
			iDirectionCount++;
		else
			pMoveDirection[iA] = NULL;		
	}

	// Reverse if we're at a dead end.
	if (iDirectionCount == 1)
	{
		Move((t_MoveDirection)((m_iMoveDir + 2) % MoveDirection_Max));
	}
	// Otherwise pick a random path to move down.
	else
	{
		xint iRandomDir = rand() % (iDirectionCount - 1);

		for (xint iA = 0; iA < MoveDirection_Max; ++iA)
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
// Nat Ryall                                                         11-Jul-2008
// =============================================================================
void CPlayer::LogicRemote()
{
	// Here we need to process incoming commands maybe.
}

// =============================================================================
// Nat Ryall                                                         11-Jul-2008
// =============================================================================
void CPlayer::Move(t_MoveDirection iDirection)
{
	m_pTargetBlock = m_pCurrentBlock->pAdjacents[iDirection];
	m_iMoveDir = iDirection;
	m_iTransitionDir = iDirection;
	m_iTime = 0;
	m_fTransition = 0.f;

	if (m_pCurrentBlock->pAdjacents[iDirection])
		SetState(PlayerState_Move);
	else
	{
		m_bLeaving = true;
		SetState(PlayerState_Warp);
	}
}

// =============================================================================
// Nat Ryall                                                         16-Apr-2008
// =============================================================================
void CPlayer::OnAnimationEvent(CAnimatedSprite* pSprite, const XCHAR* pEvent)
{
	if (strcmp(pEvent, "Eat") == 0)
	{
		if (m_pTargetBlock && m_pTargetBlock->IsEdible())
			m_pTargetBlock->Eat();
	}
}

//##############################################################################
#pragma endregion

#pragma region PacMan
//##############################################################################
//
//                                   PACMAN
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         13-Apr-2008
// =============================================================================
CPacman::CPacman() : CPlayer(PlayerType_Pacman, "Player-Pacman")
{
	SetState(PlayerState_Idle);
}

// =============================================================================
// Nat Ryall                                                         17-Apr-2008
// =============================================================================
void CPacman::Update()
{
	CPlayer::Update();
}

// =============================================================================
// Nat Ryall                                                         16-Apr-2008
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
			m_pSprite->SetAngle((XFLOAT)m_iTransitionDir * 90.f, true);

			m_iMoveTime = m_pSprite->GetAnimation()->iAnimationTime;
		}
		break;
	}

	CPlayer::SetState(iState);
}

//##############################################################################
#pragma endregion

#pragma region Ghost
//##############################################################################
//
//                                   GHOST
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         16-Apr-2008
// =============================================================================
CGhost::CGhost(xuint iColour) : CPlayer(PlayerType_Ghost, "Player-Ghost"),
	m_pEyes(NULL),
	m_iColour(iColour)
{
	m_pEyes = new CSprite(_SPRITE("Player-Ghost-Eyes"));
	m_pEyes->SetArea("F1");
	m_pEyes->SetAnchor(m_pEyes->GetAreaCentre());

	SetState(PlayerState_Idle);
}

// =============================================================================
// Nat Ryall                                                         16-Apr-2008
// =============================================================================
CGhost::~CGhost()
{
	delete m_pEyes;
}

// =============================================================================
// Nat Ryall                                                         16-Apr-2008
// =============================================================================
void CGhost::Update()
{
	CPlayer::Update();
}

// =============================================================================
// Nat Ryall                                                         16-Apr-2008
// =============================================================================
void CGhost::Render()
{
	m_pSprite->GetMetadata()->GetSprite()->SetColor(m_iColour);

	CPlayer::Render();

	m_pEyes->SetPosition(m_pSprite->GetPosition());
	m_pEyes->SetAlpha(m_pSprite->GetAlpha());
	m_pEyes->Render();
}

// =============================================================================
// Nat Ryall                                                         16-Apr-2008
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
			m_iMoveTime = m_pSprite->GetAnimation()->iAnimationTime;

			if (m_pCurrentBlock->iType == TileType_Entrance || (m_pTargetBlock && m_pTargetBlock->iType == TileType_Entrance))
				m_iMoveTime *= 4;
		}
		break;
	}

	CPlayer::SetState(iState);
}


//##############################################################################
#pragma endregion
