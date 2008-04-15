#pragma once

/**
* @file Collision.h
* @author Nat Ryall
* @date 25/02/2008
* @brief Manages collisions for all object types.
*
* Copyright © Krome Studios
*/

#pragma region Include
//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Global.
#include <Global.h>

//##############################################################################
#pragma endregion

#pragma region Types
//##############################################################################
//
//                                   TYPES
//
//##############################################################################
enum t_CollisionGroup
{
  CG_Player,
  CG_Alien,
  CG_Spear,
  CG_Obstacle,
};

//##############################################################################
#pragma endregion

#pragma region Base
//##############################################################################
//
//                                    BASE
//
//##############################################################################
class CCollidable
{
public:
  /**
  * Get the collision group for this collidable.
  */
  t_CollisionGroup GetCollisionGroup()
  {
    return m_iCollisionGroup;
  }

  /**
  * Check if the object is actually collidable at the present time.
  * @note This can be used for optimisation and disabling collision per-object.
  * @important Both objects must remain valid for the duration of the call.
  */
  virtual XBOOL IsCollidable(CCollidable* pWith) = 0;

  /**
  * Get the collision rect to test against another collision rect.
  */
  virtual XRECT GetCollisionRect() = 0;

  /**
  * Callback that is executed when a valid collision occurs.
  */
  virtual void OnCollision(CCollidable* pWith) = 0;

protected:
  /**
  * Constructor: Initialise the collidable group.
  */
  CCollidable(t_CollisionGroup iCollisionGroup);

  /**
  * Destructor.
  */
  virtual ~CCollidable();

private:
  // The object's collision group.
  t_CollisionGroup m_iCollisionGroup;
};

// TODO: Add multiple collide types: CT_Rect, CT_Circle, CT_PixelMask

//##############################################################################
#pragma endregion

#pragma region Declaration
//##############################################################################
//
//                                 DECLARATION
//
//##############################################################################
namespace CollisionManager
{
  /**
  * Add a collidable to the manager. Events will be fired between types automatically once added.
  */
  void Add(CCollidable* pCollidable);

  /**
  * Remove a collidable from the manager.
  */
  void Remove(CCollidable* pCollidable);

  /**
  * Remove all currently managed collidables from the system and reset the manager to defaults.
  */
  void Reset();
}

//##############################################################################
#pragma endregion