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
// Author: Nat Ryall                                           Date: 25-Feb-2008
// =============================================================================
void CCollisionManager::Reset()
{
	m_lpCollidables.clear();
}

// =============================================================================
// Nat Ryall                                                         28-Jul-2008
// =============================================================================
void CCollisionManager::OnUpdate()
{
	XEN_LIST_FOREACH(t_CollidableList, ppCollidable, m_lpCollidables)
	{
		t_CollidableList::iterator ppCollideWith = ppCollidable;
		ppCollideWith++;

		for (; ppCollideWith != m_lpCollidables.end(); ppCollideWith++)
		{
			XMASSERT(*ppCollidable != *ppCollideWith, "Collidables should never match.");

			if ((*ppCollidable)->IsCollidable(*ppCollideWith) && (*ppCollideWith)->IsCollidable(*ppCollidable))
			{
				if (IsColliding(*ppCollidable, *ppCollideWith))
				{
					(*ppCollidable)->OnCollision(*ppCollideWith);
					(*ppCollideWith)->OnCollision(*ppCollidable);
				}
			}
		}
	}
}

// =============================================================================
// Author: Nat Ryall                                           Date: 25-Feb-2008
// =============================================================================
void CCollisionManager::Add(CCollidable* pCollidable)
{
	m_lpCollidables.push_back(pCollidable);
}

// =============================================================================
// Author: Nat Ryall                                           Date: 25-Feb-2008
// =============================================================================
void CCollisionManager::Remove(CCollidable* pCollidable)
{
	m_lpCollidables.remove(pCollidable);
}

// =============================================================================
// Nat Ryall                                                         28-Jul-2008
// =============================================================================
xbool CCollisionManager::IsColliding(CCollidable* pA, CCollidable* pB)
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