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

// Global.
#include <Global.h>

// Other.
#include <Resource.h>

//##############################################################################

// Shortcuts.
#define SoundManager CSoundManager::Get()

// Find a sound resource.
#define _SOUND(NAME) ((CSoundMetadata*)ResourceManager.GetResourceMetadata(ResourceType_Sound, NAME))

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
class CSoundFile : public CResourceFile
{
public:
	// Constructor: Initialise the file.
	CSoundFile(const xchar* pFile, CDataset* pDataset);

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
		xuint m_iTime;
	};

	// The list of markers associated with this sound.
	t_MarkerList m_lpMarkers;
};

//##############################################################################
class CSound
{
public:
	// Constructor.
	CSound(CSoundMetadata* pMetadata);

	// Destructor.
	virtual ~CSound();

	// Play the sound specified in the metadata.
	// ~pName Play the sound from the specified marker of from the current offset if NULL.
	void Play(const xchar* pName = NULL);
	
	// Pause the sound at its current position.
	void Pause()
	{
		m_pChannel->setPaused(true);
	}

	// Resume the sound from the current offset.
	void Resume()
	{
		m_pChannel->setPaused(false);
	}

	// Stop the sound from playing and reset the play offset.
	void Stop();

	// Check if the sound is currently playing.
	inline xbool IsPlaying()
	{
		xbool bPlaying = false;

		if (m_pChannel)
			m_pChannel->isPlaying(&bPlaying);

		return bPlaying;
	}

	// Get the sound offset position in milliseconds.
	inline xuint GetTime()
	{
		xuint iTrackTime = 0;

		if (m_pChannel)
			m_pChannel->getPosition(&iTrackTime, FMOD_TIMEUNIT_MS);

		return iTrackTime;
	}

	// Set the sound offset position in milliseconds.
	inline void SetTime(xuint iTime)
	{
		if (m_pChannel)
			m_pChannel->setPosition(iTime, FMOD_TIMEUNIT_MS);
	}

	// Get the time from the specified marker.
	xuint GetMarkerTime(const xchar* pName);

	// Determine if an audio marker has expired.
	// ~pName The name of the marker to check against.
	xbool IsMarkerExpired(const xchar* pName);

	// Get the sound metadata structure. This is for advanced use only.
	inline CSoundMetadata* GetMetadata()
	{
		return m_pMetadata;
	}

	// Get the channel for this sound for direct manipulation.
	inline FMOD::Channel* GetChannel()
	{
		return m_pChannel;
	}

protected:
	// The sound metadata.
	CSoundMetadata* m_pMetadata;

	// The active channel for this sound.
	FMOD::Channel* m_pChannel;
};

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
