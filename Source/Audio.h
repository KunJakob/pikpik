#pragma once

/**
* @file Audio.h
* @author Nat Ryall
* @date 11/08/2008
* @brief Audio management.
*
* Copyright © SAPIAN
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
//                                   MACROS
//
//##############################################################################

// Shortcuts.
#define AudioManager CAudioManager::Get()

//##############################################################################

//##############################################################################
//
//                                   TYPES
//
//##############################################################################

// The audio groups.
enum t_AudioGroup
{
	AudioGroup_Music,
	AudioGroup_Effects,
	AudioGroup_Voice,
};

//##############################################################################

//##############################################################################
//
//                               AUDIO MANAGER
//
//##############################################################################
class CAudioManager
{
public:
	// Singleton instance.
	static inline CAudioManager& Get() 
	{
		static CAudioManager s_Instance;
		return s_Instance;
	}

	// Set the master volume, this will affect all sound types.
	//void SetVolume(xfloat fVolume);

	// Set the volume of a specific audio group.
	//void SetVolume(t_AudioGroup iGroup, xfloat fVolume);

protected:
};

//##############################################################################