#pragma once

/**
* @file Brain.h
* @author Nat Ryall
* @date 08/07/2008
* @brief The game's artificial intelligence.
*
* Copyright 2008, SAPIAN
*/

//##############################################################################
//
//                                  INCLUDE
//
//##############################################################################

// Global.
#include <Global.h>

//##############################################################################

//##############################################################################
//
//                                   BRAIN
//
//##############################################################################
class CBrain
{
public:
	//xlist<CPlayer*> ScanCoridoor();

protected:
};

//##############################################################################

//##############################################################################
//
//                                PACMAN BRAIN
//
//##############################################################################
class CPacManBrain : public CBrain
{
public:
protected:
};

//##############################################################################

//##############################################################################
//
//                                 GHOST BRAIN
//
//##############################################################################
class CGhostBrain : public CBrain
{
public:
protected:
};

//##############################################################################