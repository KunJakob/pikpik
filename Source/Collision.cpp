//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Global.
#include <Global.h>

// Local.
#include <Collision.h>

//##############################################################################

//##############################################################################
//
//                                 COLLIDABLE
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         25-Feb-2008
// =============================================================================
CCollidable::CCollidable(xint iCollisionGroup, t_CollisionType iCollisionType) :
	m_iCollisionGroup(iCollisionGroup),
	m_iCollisionType(iCollisionType)
{
}

//##############################################################################

//##############################################################################
//
//                                 DECLARATION
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         07-Aug-2008
// =============================================================================
CCollisionManager::CCollisionManager()
{
}

// =============================================================================
// Author: Nat Ryall                                           Date: 25-Feb-2008
// =============================================================================
void CCollisionManager::Reset()
{
	for (xint iA = 0; iA < CollisionLayer_Max; ++iA)
		m_lpCollidables[iA].clear();
}

// =============================================================================
// Nat Ryall                                                         28-Jul-2008
// =============================================================================
void CCollisionManager::OnUpdate()
{
	for (xint iA = 0; iA < CollisionLayer_Max; ++iA)
	{
		XEN_LIST_FOREACH(t_CollidableList, ppCollidable, m_lpCollidables[iA])
		{
			t_CollidableList::iterator ppCollideWith = ppCollidable;
			ppCollideWith++;

			for (; ppCollideWith != m_lpCollidables[iA].end(); ppCollideWith++)
			{
				XMASSERT(*ppCollidable != *ppCollideWith, "Collidables should never match.");

				if ((*ppCollidable)->IsCollidable(*ppCollideWith) && (*ppCollideWith)->IsCollidable(*ppCollidable))
				{
					if (AreColliding(*ppCollidable, *ppCollideWith))
					{
						(*ppCollidable)->OnCollision(*ppCollideWith);
						(*ppCollideWith)->OnCollision(*ppCollidable);
					}
				}
			}
		}
	}
}

// =============================================================================
// Author: Nat Ryall                                           Date: 25-Feb-2008
// =============================================================================
void CCollisionManager::Add(CCollidable* pCollidable, xint iCollisionLayer)
{
	XASSERT(iCollisionLayer > CollisionLayer_All && iCollisionLayer < CollisionLayer_Max);

	m_lpCollidables[iCollisionLayer].push_back(pCollidable);
}

// =============================================================================
// Author: Nat Ryall                                           Date: 25-Feb-2008
// =============================================================================
void CCollisionManager::Remove(CCollidable* pCollidable, xint iCollisionLayer)
{
	XASSERT(iCollisionLayer >= CollisionLayer_All && iCollisionLayer < CollisionLayer_Max);

	if (iCollisionLayer == CollisionLayer_All)
	{
		for (xint iA = 0; iA < CollisionLayer_Max; ++iA)
			m_lpCollidables[iA].remove(pCollidable);
	}
	else
		m_lpCollidables[iCollisionLayer].remove(pCollidable);
}

// =============================================================================
// Nat Ryall                                                         28-Jul-2008
// =============================================================================
xbool CCollisionManager::AreColliding(CCollidable* pA, CCollidable* pB)
{
	if (pA == pB)
		return true;

	switch (pA->GetCollisionType())
	{
	case CollisionType_Point:
		{
			switch (pB->GetCollisionType())
			{
			case CollisionType_Point:	return Math::Intersect(pA->GetCollisionPoint(), pB->GetCollisionPoint());
			case CollisionType_Rect:	return Math::Intersect(pA->GetCollisionPoint(), pB->GetCollisionRect());
			case CollisionType_Circle:	return Math::Intersect(pA->GetCollisionPoint(), pB->GetCollisionCircle());
			}
		}
		break;

	case CollisionType_Rect:
		{
			switch (pB->GetCollisionType())
			{
			case CollisionType_Point:	return Math::Intersect(pB->GetCollisionPoint(), pA->GetCollisionRect());
			case CollisionType_Rect:	return Math::Intersect(pA->GetCollisionRect(), pB->GetCollisionRect());
			case CollisionType_Circle:	return Math::Intersect(pA->GetCollisionRect(), pB->GetCollisionCircle());
			}
		}
		break;

	case CollisionType_Circle:
		{
			switch (pB->GetCollisionType())
			{
			case CollisionType_Point:	return Math::Intersect(pB->GetCollisionPoint(), pA->GetCollisionCircle());
			case CollisionType_Rect:	return Math::Intersect(pB->GetCollisionRect(), pA->GetCollisionCircle());
			case CollisionType_Circle:	return Math::Intersect(pA->GetCollisionCircle(), pB->GetCollisionCircle());
			}
		}
		break;
	}

	return false;
}

//##############################################################################