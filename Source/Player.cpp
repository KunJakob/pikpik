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
	m_iState(PlayerState_None),
	m_pSprite(NULL),
	m_pCurrentBlock(NULL),
	m_pTargetBlock(NULL),
	m_iTime(0),
	m_iMoveTime(0),
	m_fTransition(0.f),
	m_iTransitionDir(AdjacentDir_Left)
{
	m_pSprite = ResourceManager::CreateAnimatedSprite(pSpriteName);
	m_pSprite->SetEventCallback(&OnAnimationEvent, this);
}

// =============================================================================
// Nat Ryall                                                         13-Apr-2008
// =============================================================================
CPlayer::~CPlayer()
{
	ResourceManager::FreeResource(m_pSprite);
}

// =============================================================================
// Nat Ryall                                                         13-Apr-2008
// =============================================================================
void CPlayer::Update()
{
	switch (m_iState)
	{
	case PlayerState_Idle:
		{
			if (this == _GLOBAL.pActivePlayer)
			{
				for (XUINT iA = 0; iA < AdjacentDir_Max; ++iA)
				{
					if (_HGE->Input_GetKeyState(HGEK_LEFT + iA) && m_pCurrentBlock->pAdjacents[iA] && IsPassable(m_pCurrentBlock->pAdjacents[iA]))
					{
						m_pTargetBlock = m_pCurrentBlock->pAdjacents[iA];
						m_iTransitionDir = (t_AdjacentDir)iA;
						m_iTime = 0;
						m_fTransition = 0.f;

						SetState(PlayerState_Move);
					}
				}
			}
		}
		break;

	case PlayerState_Move:
		{
			if (m_pSprite->GetPosition() == m_pTargetBlock->GetScreenPosition())
			{
				m_pCurrentBlock = m_pTargetBlock;
				m_pTargetBlock = NULL;

				SetState(PlayerState_Idle);
			}
			else
			{
				m_iTime = Math::Clamp<XUINT>(m_iTime + _TIMEDELTA, 0, m_iMoveTime);
				m_fTransition = Math::Clamp<XFLOAT>((XFLOAT)m_iTime / (XFLOAT)m_iMoveTime, 0.f, 1.f);

				m_pSprite->SetPosition(m_pCurrentBlock->GetScreenPosition() + (((m_pTargetBlock->GetScreenPosition() - m_pCurrentBlock->GetScreenPosition()) * m_iTime) / m_iMoveTime));
			}
		}
		break;

	case PlayerState_Die:
		{

		}
		break;
	}

	if (this != _GLOBAL.pActivePlayer)
		m_pSprite->SetAlpha(m_pCurrentBlock->fVisibility);
	else
		m_pSprite->SetAlpha(1.f);
}

// =============================================================================
// Nat Ryall                                                         13-Apr-2008
// =============================================================================
void CPlayer::Render()
{
	m_pSprite->Render();
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
// Nat Ryall                                                         16-Apr-2008
// =============================================================================
void CPlayer::OnAnimationEvent(const XCHAR* pEvent, void* pObject)
{
	CPlayer* pPlayer = (CPlayer*)pObject;

	if (strcmp(pEvent, "Eat") == 0)
	{
		if (pPlayer->m_pTargetBlock->iType == TileType_Pellet || pPlayer->m_pTargetBlock->iType == TileType_Power)
			pPlayer->m_pTargetBlock->iType = TileType_Eaten;
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
CPacMan::CPacMan() : CPlayer(PlayerType_PacMan, "Player-PacMan")
{
	m_pCurrentBlock = _GLOBAL.pActiveMap->GetSpawnBlock(PlayerType_PacMan);
	m_pSprite->SetPosition(m_pCurrentBlock->GetScreenPosition());

	SetState(PlayerState_Idle);
}

// =============================================================================
// Nat Ryall                                                         17-Apr-2008
// =============================================================================
void CPacMan::Update()
{
	CPlayer::Update();
}

// =============================================================================
// Nat Ryall                                                         16-Apr-2008
// =============================================================================
void CPacMan::SetState(t_PlayerState iState)
{
	switch (iState)
	{
	case PlayerState_Idle:
		{
			if (!m_pSprite->IsCurrentAnimation("Idle"))
				m_pSprite->Play("Idle");
		}
		break;

	case PlayerState_Move:
		{
			m_pSprite->Play("Move");
			m_pSprite->SetRotation((XFLOAT)m_iTransitionDir * 90.f, true);

			m_iMoveTime = m_pSprite->GetAnimation()->iTime;
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
CGhost::CGhost() : CPlayer(PlayerType_Ghost, "Player-Ghost"),
	m_pEyes(NULL)
{
	m_pCurrentBlock = _GLOBAL.pActiveMap->GetSpawnBlock(PlayerType_Ghost);
	m_pSprite->SetPosition(m_pCurrentBlock->GetScreenPosition());

	m_pEyes = ResourceManager::CreateSprite("Player-Ghost-Eyes");
	m_pEyes->SetArea("F1");

	SetState(PlayerState_Idle);
}

// =============================================================================
// Nat Ryall                                                         16-Apr-2008
// =============================================================================
CGhost::~CGhost()
{
	ResourceManager::FreeResource(m_pEyes);
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
	CPlayer::Render();

	//m_pEyes->SetAlpha((m_pSprite->GetAlpha());
	m_pEyes->SetPosition(m_pSprite->GetPosition());
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
			if (!m_pSprite->IsCurrentAnimation("Idle"))
				m_pSprite->Play("Idle");
		}
		break;

	case PlayerState_Move:
		{
			m_pEyes->SetArea(XFORMAT("F%d", m_iTransitionDir + 1));
			m_iMoveTime = m_pSprite->GetAnimation()->iTime;

			if (m_pCurrentBlock->iType == TileType_Entrance || m_pTargetBlock->iType == TileType_Entrance)
				m_iMoveTime *= 4;
		}
		break;
	}

	CPlayer::SetState(iState);
}


//##############################################################################
#pragma endregion