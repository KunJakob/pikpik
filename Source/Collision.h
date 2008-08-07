#pragma once

/**
* @file Collision.h
* @author Nat Ryall
* @date 25/02/2008
* @brief Manages collisions for all object types.
*
* Copyright © Krome Studios
*/

//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Global.
#include <Global.h>

//##############################################################################

//##############################################################################
//
//                                   MACROS
//
//##############################################################################

// Shortcuts.
#define CollisionManager CCollisionManager::Get()

//##############################################################################

//##############################################################################
//
//                                   TYPES
//
//##############################################################################

// Predeclare.
class CCollidable;

// The collision check types.
enum t_CollisionType
{
	CollisionType_Point,		// The collision area is a single point.
	CollisionType_Rect,			// The collision area is an axis aligned rectangle.
	CollisionType_Circle,		// The collision area is a circle.
};

// Lists.
typedef xlist<CCollidable*> t_CollidableList; 

//##############################################################################

//##############################################################################
//
//                                 COLLIDABLE
//
//##############################################################################
class CCollidable
{
public:
	// Get the collision group for this collidable.
	xint GetCollisionGroup()
	{
		return m_iCollisionGroup;
	}

	// Get the collision type for this collidable.
	t_CollisionType GetCollisionType()
	{
		return m_iCollisionType;
	}

	// Get the collision point.
	virtual xpoint GetCollisionPoint() 
	{ 
		return xpoint(); 
	}

	// Get the collision rect.
	virtual xrect GetCollisionRect() 
	{ 
		return xrect(); 
	}

	// Get the collision circle.
	virtual xcircle GetCollisionCircle() 
	{ 
		return xcircle(); 
	}

	// Check if the object is actually collidable at the present time.
	// ~note It is important that both objects must remain valid for the duration of the call.
	virtual xbool IsCollidable(CCollidable* pWith) = 0;

	// Callback that is executed when a valid collision occurs.
	virtual void OnCollision(CCollidable* pWith) = 0;

protected:
	// Constructor: Initialise the collidable group.
	CCollidable(xint iCollisionGroup, t_CollisionType iCollisionType);

	// Destructor.
	virtual ~CCollidable() {}

private:
	// The object's collision group.
	xint m_iCollisionGroup;

	// The object's collision type.
	t_CollisionType m_iCollisionType;
};

//##############################################################################

//##############################################################################
//
//                                 DECLARATION
//
//##############################################################################
class CCollisionManager : public CModule
{
public:
	// Singleton instance.
	static inline CCollisionManager& Get() 
	{
		static CCollisionManager s_Instance;
		return s_Instance;
	}

	// Constructor.
	CCollisionManager();

	// Check for valid collisions for managed collidables.
	virtual void OnUpdate();

	// Render collision bounds for debugging purposes.
	//virtual void OnRender();

	// Add a collidable to the manager. Events will be fired between types automatically once added.
	void Add(CCollidable* pCollidable);

	// Remove a collidable from the manager.
	void Remove(CCollidable* pCollidable);

	// Remove all currently managed collidables from the system.
	void RemoveAll();

	// Check if two collidables are colliding regardless of type.
	xbool AreColliding(CCollidable* pA, CCollidable* pB);

protected:
	// The list of collidable objects to be compared against each other.
	t_CollidableList m_lpCollidables;

	// TODO: Think about adding collision layers "m_lpCollidables[CollisionLayer_Max]".
	// Collision layers can have the same collision element added to them as another layer but only collisions in each layer are valid.

private:
	// Draw a box.
	//void DrawBox(xrect xRect, xuint iColour);

	// Draw a circle.
	//void DrawCircle(xfloat fX, xfloat fY, xfloat fRadius, xint iSegments, xuint iColour);
};

//##############################################################################