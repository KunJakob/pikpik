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

// Predeclare.
class CSound;

// The audio groups.
enum t_AudioGroup
{
	AudioGroup_System,
	AudioGroup_Music,
	AudioGroup_Effects,
	AudioGroup_Voice,
};

// Callbacks.
typedef xfunction(3)<CSound* /*Sound*/, const xchar* /*MarkerName*/, xuint /*MarkerTime*/> t_MarkerCallback;

//##############################################################################

//##############################################################################
//
//                                SOUND MARKER
//
//##############################################################################
class CSoundMarker
{
public:

};

//##############################################################################

//##############################################################################
//
//                                   SOUND
//
//##############################################################################
class CSound
{
public:
	// Get a marker explicitly by name.
	//CSoundMarker* GetMarker(const xchar* pName)

	// Determine if an audio marker has expired.
	xbool IsMarkerExpired(const xchar* pName);

	// Bind a callback to an audio marker.
	void SetMarkerCallback(const xchar* pName, t_MarkerCallback fpCallback);

protected:
	//
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
	void SetVolume(xfloat fVolume);

	// Set the volume of a specific audio group.
	void SetVolume(t_AudioGroup iGroup, xfloat fVolume);

protected:
};

//##############################################################################