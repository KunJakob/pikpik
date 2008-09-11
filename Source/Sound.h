#pragma once

/**
* @file Sound.h
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

// Other.
#include <Metadata.h>
#include <Resource.h>

//##############################################################################

//##############################################################################
//
//                                   MACROS
//
//##############################################################################

// Shortcuts.
#define SoundManager CSoundManager::Get()

// Find a sound resource.
#define _SOUND(NAME) ((CSoundMetadata*)ResourceManager.GetResourceMetadata(ResourceType_Sound, NAME))

//##############################################################################

//##############################################################################
//
//                                   TYPES
//
//##############################################################################

// Predeclare.
class CSound;

// The audio type.
enum t_SoundType
{
	SoundType_Sample			= FMOD_CREATESAMPLE,			// The sound is loaded into memory and decompressed.
	SoundType_CompressedSample	= FMOD_CREATECOMPRESSEDSAMPLE,	// The sound is loaded into memory but remains compressed.
	SoundType_Stream			= FMOD_CREATESTREAM,			// The sound is streamed in from a file. Can only have one active channel.
};

// The audio groups.
enum t_SoundGroup
{
	SoundGroup_Unknown = -1,	// The group was not specified.
	SoundGroup_System,			// System non-modifiable sounds (protected).
	SoundGroup_Music,			// Background music tracks.
	SoundGroup_Effects,			// Short effect sounds and audio clips.
	SoundGroup_Voice,			// Voice overs.
};

//##############################################################################

//##############################################################################
//
//                                 SOUND FILE
//
//##############################################################################
class CSoundFile : public CResourceFile
{
public:
	// Constructor: Initialise the file.
	CSoundFile(const xchar* pFile);

	// Destructor: Clean up the file memory.
	virtual ~CSoundFile();

	// Get the internal sound object.
	FMOD::Sound* GetResource()
	{
		return (FMOD::Sound*)m_pResource;
	}

	// The sound type for the file.
	t_SoundType m_iSoundType;
};

//##############################################################################

//##############################################################################
//
//                               SOUND METADATA
//
//##############################################################################
class CSoundMetadata : public CResourceMetadata
{
public:
	// Predeclare.
	class CMarker;

	// Types.
	typedef xlist<CMarker*> t_MarkerList;

	// Initialise the metadata.
	CSoundMetadata(CDataset* pDataset);

	// Deinitialise the metadata.
	virtual ~CSoundMetadata();

	// Get the sprite object.
	FMOD::Sound* GetSound()
	{
		return m_pFile->GetResource();
	}

	// Find a marker by name.
	CMarker* FindMarker(const xchar* pName);

	// The sound file.
	CSoundFile* m_pFile;

	// The sound group.
	t_SoundGroup m_iGroup;

	// The sound volume.
	xfloat m_fVolume;

	/**
	* Specifies a timed marker in the sound.
	*/
	class CMarker
	{
	public:
		// The name of the marker.
		const xchar* m_pName;

		// The sound time, in milliseconds, that the marker is set to.
		xint m_iTime;
	};

	// The list of markers associated with this sound.
	t_MarkerList m_lpMarkers;
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

protected:
	//
};

//##############################################################################

//##############################################################################
//
//                               SOUND MANAGER
//
//##############################################################################
class CSoundManager : public CModule
{
public:
	// Singleton instance.
	static inline CSoundManager& Get() 
	{
		static CSoundManager s_Instance;
		return s_Instance;
	}

	// Constructor.
	CSoundManager();

	// Initialise the module.
	virtual void OnInitialise();

	// Deinitialise the module.
	virtual void OnDeinitialise();

	// Update the module.
	virtual void OnUpdate();

	// Get the sound interface.
	inline FMOD::System* GetSoundSystem()
	{
		return m_pSoundSystem;
	}

	// Set the master volume, this will affect all sound types.
	void SetVolume(xfloat fVolume);

	// Set the volume of a specific audio group.
	void SetVolume(t_SoundGroup iGroup, xfloat fVolume);

protected:
	// The FMOD sound interface.
	FMOD::System* m_pSoundSystem;
};

//##############################################################################
