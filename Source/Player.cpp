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
CPlayer::CPlayer(t_PlayerType iType, const XCHAR* pSpriteName) : 
	CRenderable(RenderableType_Player), 
	m_iType(iType),
	m_iState(PlayerState_Idle),
	m_pSprite(NULL),
	m_pCurrentBlock(NULL),
	m_pTargetBlock(NULL),
	m_iTime(0),
	m_iTransitionDir(AdjacentDir_Left)
{
	m_pSprite = ResourceManager::CreateSprite(pSpriteName);
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
			for (XUINT iA = 0; iA < AdjacentDir_Max; ++iA)
			{
				if (_HGE->Input_GetKeyState(HGEK_LEFT + iA) && m_pCurrentBlock->pAdjacents[iA] && m_pCurrentBlock->pAdjacents[iA]->cChar != '#')
				{
					m_pTargetBlock = m_pCurrentBlock->pAdjacents[iA];

					m_pSprite->Play("Move");
					m_pSprite->SetRotation((XFLOAT)iA * 90.f, true);
					
					m_iState = PlayerState_Move;
					m_iTime = 0;
				}
			}

			if (m_iState == PlayerState_Idle && !m_pSprite->IsCurrentAnimation("Idle"))
				m_pSprite->Play("Idle");
		}
		break;

	case PlayerState_Move:
		{
			if (m_pSprite->GetPosition() == m_pTargetBlock->GetScreenPosition())
			{
				m_pCurrentBlock = m_pTargetBlock;
				m_pTargetBlock = NULL;

				m_iState = PlayerState_Idle;
			}
			else
			{
				m_iTime = Math::Clamp<XUINT>(m_iTime + _TIMEDELTA, 0, m_pSprite->GetAnimation()->iTime);

				XPOINT xDistance = ((m_pTargetBlock->GetScreenPosition() - m_pCurrentBlock->GetScreenPosition()) * m_iTime) / m_pSprite->GetAnimation()->iTime;
				m_pSprite->SetPosition(m_pCurrentBlock->GetScreenPosition() + xDistance);
			}
		}
		break;

	case PlayerState_Die:
		{

		}
		break;
	}

}

// =============================================================================
// Nat Ryall                                                         13-Apr-2008
// =============================================================================
void CPlayer::Render()
{
	m_pSprite->Render();
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
	m_pCurrentBlock = _MAP->GetRandomSpawnBlock();
	m_pSprite->SetPosition(m_pCurrentBlock->GetScreenPosition());
}

// =============================================================================
// Nat Ryall                                                         13-Apr-2008
// =============================================================================
void CPacMan::Update()
{
	switch (m_iState)
	{
	case PlayerState_Move:
		{
			if (m_pSprite->GetPosition() == m_pTargetBlock->GetScreenPosition())
			{
				if (m_pTargetBlock->iType == TileType_Pellet)
					m_pTargetBlock->iType = TileType_Blank;
			}
		}
		break;
	}

	CPlayer::Update();
}

//##############################################################################
#pragma endregion