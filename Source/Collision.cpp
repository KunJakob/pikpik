#pragma region Include
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
#pragma endregion

#pragma region Types
//##############################################################################
//
//                                   TYPES
//
//##############################################################################

// Lists.
typedef XLIST<CCollidable*> t_CollidableList;

//##############################################################################
#pragma endregion

#pragma region Static
//##############################################################################
//
//                                   STATIC
//
//##############################################################################

// Lists.
static t_CollidableList s_lpCollidables;

//##############################################################################
#pragma endregion

#pragma region ModuleManager
//##############################################################################
//
//                                   MODULE
//
//##############################################################################
static class CCollisionModule : public Xen::CModule
{
public:
	// Constructor.
	CCollisionModule()
	{
		XMODULE(this);
	}

	// Update.
	virtual void Update()
	{
		XEN_LIST_FOREACH(t_CollidableList, ppCollidable, s_lpCollidables)
		{
			t_CollidableList::iterator ppCollideWith = ppCollidable;
			ppCollideWith++;

			for (; ppCollideWith != s_lpCollidables.end(); ppCollideWith++)
			{
				XASSERT(*ppCollidable != *ppCollideWith, "Collidables should never match.");

				if ((*ppCollidable)->IsCollidable(*ppCollideWith) && (*ppCollideWith)->IsCollidable(*ppCollidable))
				{
					if (Xen::Math::Intersect((*ppCollidable)->GetCollisionRect(), (*ppCollideWith)->GetCollisionRect()))
					{
						(*ppCollidable)->OnCollision(*ppCollideWith);
						(*ppCollideWith)->OnCollision(*ppCollidable);
					}
				}
			}
		}
	}

	// Deinitialise.
	virtual void Deinitialise()
	{
		CollisionManager::Reset();
	}
} 
s_Module;

//##############################################################################
#pragma endregion

#pragma region Base
//##############################################################################
//
//                                    BASE
//
//##############################################################################

// =============================================================================
// Author: Nat Ryall                                           Date: 25-Feb-2008
// =============================================================================
CCollidable::CCollidable(t_CollisionGroup iCollisionGroup) :
  m_iCollisionGroup(iCollisionGroup)
{
  //CollisionManager::Add(this);
}

// =============================================================================
// Author: Nat Ryall                                           Date: 25-Feb-2008
// =============================================================================
CCollidable::~CCollidable()
{
  //CollisionManager::Remove(this);
}

//##############################################################################
#pragma endregion

#pragma region Declaration
//##############################################################################
//
//                                 DECLARATION
//
//##############################################################################

// =============================================================================
// Author: Nat Ryall                                           Date: 25-Feb-2008
// =============================================================================
void CollisionManager::Add(CCollidable* pCollidable)
{
  s_lpCollidables.push_back(pCollidable);
}

// =============================================================================
// Author: Nat Ryall                                           Date: 25-Feb-2008
// =============================================================================
void CollisionManager::Remove(CCollidable* pCollidable)
{
  s_lpCollidables.remove(pCollidable);
}

// =============================================================================
// Author: Nat Ryall                                           Date: 25-Feb-2008
// =============================================================================
void CollisionManager::Reset()
{
  s_lpCollidables.clear();
}

//##############################################################################
#pragma endregion