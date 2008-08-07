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
void CCollisionManager::RemoveAll()
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
				if (AreColliding(*ppCollidable, *ppCollideWith))
				{
					(*ppCollidable)->OnCollision(*ppCollideWith);
					(*ppCollideWith)->OnCollision(*ppCollidable);
				}
			}
		}
	}
}

// =============================================================================
// Nat Ryall                                                         07-Aug-2008
// =============================================================================
/*void CCollisionManager::OnRender()
{
	if (m_bDebugRender)
	{
		XEN_LIST_FOREACH(t_CollidableList, ppCollidable, m_lpCollidables)
		{
			switch ((*ppCollidable)->GetCollisionType())
			{
			case CollisionType_Point:
				{
					xpoint xPoint = (*ppCollidable)->GetCollisionPoint() - m_xOffset;
					_HGE->Gfx_RenderLine((xfloat)xPoint.iX, (xfloat)xPoint.iY, (xfloat)xPoint.iX + 1.f, (xfloat)xPoint.iY + 1.f);
				}
				break;

			case CollisionType_Rect:
				{
					xrect xRect = (*ppCollidable)->GetCollisionRect() - m_xOffset;
					DrawBox(xRect, 0xFFFFFFFF);
				}
				break;

			case CollisionType_Circle:
				{
					xcircle xCircle = (*ppCollidable)->GetCollisionCircle();
					xCircle -= m_xOffset;

					DrawCircle((xfloat)xCircle.iX, (xfloat)xCircle.iY, (xfloat)xCircle.iRadius, 16, 0xFFFFFFFF);
				}
				break;
			}
		}
	}
}*/

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

// =============================================================================
// Nat Ryall                                                         06-Aug-2008
// =============================================================================
/*void CCollisionManager::DrawBox(xrect xRect, xuint iColour)
{
	_HGE->Gfx_RenderLine((float)xRect.iLeft, (float)xRect.iTop, (float)xRect.iRight, (float)xRect.iTop, iColour);
	_HGE->Gfx_RenderLine((float)xRect.iRight, (float)xRect.iTop, (float)xRect.iRight, (float)xRect.iBottom, iColour);
	_HGE->Gfx_RenderLine((float)xRect.iRight, (float)xRect.iBottom, (float)xRect.iLeft, (float)xRect.iBottom, iColour);
	_HGE->Gfx_RenderLine((float)xRect.iLeft, (float)xRect.iBottom, (float)xRect.iLeft, (float)xRect.iTop, iColour);
}*/

// =============================================================================
// Nat Ryall                                                         07-Aug-2008
// =============================================================================
/*void CCollisionManager::DrawCircle(xfloat fX, xfloat fY, xfloat fRadius, xint iSegments, xuint iColour)
{
	xfloat fPerAngle;
	xfloat fA;
	xfloat fLeft;
	xfloat fRight;
	xfloat fTop;
	xfloat fBottom;

	fPerAngle = (2.f * M_PI) / (xfloat)iSegments;

	fRight = fRadius;
	fBottom = 0.f;

	for(fA = 0.f; fA <= ((2.f * M_PI) + fPerAngle); fA += fPerAngle) 
	{
		fLeft = fRight;
		fTop = fBottom;
		fRight = fRadius * cos(fA);
		fBottom = fRadius * sin(fA);

		_HGE->Gfx_RenderLine(fLeft + fX, fTop + fY, fRight + fX, fBottom + fY, iColour);
	}
}*/

//##############################################################################