#pragma once

/**
* @file Play.h
* @author Nat Ryall
* @date 27/05/2008
* @brief The game flow and sequencing control.
*
* Copyright © SAPIAN
*/

//##############################################################################

// Global.
#include <Global.h>

//##############################################################################

// Singleton accessor.
#define PlayManager CPlayManager::Get()

//##############################################################################
class CPlayManager
{
public:
	// Singleton instance.
	static inline CPlayManager& Get() 
	{
		static CPlayManager s_Instance;
		return s_Instance;
	}

protected:
};

//##############################################################################