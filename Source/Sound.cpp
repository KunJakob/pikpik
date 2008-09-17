/**
* @file Sound.cpp
* @author Nat Ryall
* @date 11/08/2008
* @brief Audio management.
*
* Copyright © SAPIAN
*/

//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Global.
#include <Global.h>

// Local.
#include <Sound.h>

//##############################################################################

//##############################################################################
//
//                                 SOUND FILE
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         03-Sep-2008
// =============================================================================
CSoundFile::CSoundFile(const xchar* pFile) : CResourceFile(ResourceType_Sound, pFile)
{
	// Load compressed files into memory without decompressing. Determine this by extension.
	static const xchar* s_pCompressedTypes[] = { "mp3", "ogg" };
	static const xint s_iCompressedTypesCount = sizeof(s_pCompressedTypes) / sizeof(xchar*);

	FMOD_MODE iCreationMode = FMOD_CREATESAMPLE;
	xint iPathLen = String::Length(pFile);

	if (iPathLen >= 3)
	{
		const xchar* pExtension = &pFile[iPathLen - 3];

		for (int iA = 0; iA < s_iCompressedTypesCount; ++iA)
		{
			if (String::IsCaselessMatch(s_pCompressedTypes[iA], pExtension))
			{
				iCreationMode = FMOD_CREATECOMPRESSEDSAMPLE;
				break;
			}
		}
	}

	// Create the sound.
	_FMOD->createSound(pFile, FMOD_SOFTWARE | iCreationMode, NULL, (FMOD::Sound**)&m_pResource);

	// Save the sound type we used.
	m_iSoundType = (t_SoundType)iCreationMode;
}

// =============================================================================
// Nat Ryall                                                         03-Sep-2008
// =============================================================================
CSoundFile::~CSoundFile()
{
	GetResource()->release();
}

//##############################################################################

//##############################################################################
//
//                               SOUND METADATA
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         03-Sep-2008
// =============================================================================
CSoundMetadata::CSoundMetadata(CDataset* pDataset) : CResourceMetadata(ResourceType_Sound, pDataset)
{
	// File.
	m_pFile = (CSoundFile*)ResourceManager.CreateResourceFile(ResourceType_Sound, pDataset->GetProperty("File")->GetString());

	// Info.
	m_iGroup = SoundGroup_Unknown;
	m_fVolume = 1.f;

	if (_PROPERTY_EXISTS(pDataset, "Group"))
		m_iGroup = (t_SoundGroup)_PROPERTY->GetInt();

	if (_PROPERTY_EXISTS(pDataset, "Volume"))
		m_fVolume = _PROPERTY->GetFloat();

	// Markers.
	_DATASET_FOREACH(pMarkerDataset, pDataset, "Marker", NULL)
	{
		CMarker* pMarker = new CMarker;
		m_lpMarkers.push_back(pMarker);

		pMarker->m_pName = pMarkerDataset->GetName();
		pMarker->m_iTime = 0;

		if (_PROPERTY_EXISTS(pMarkerDataset, "Time"))
			pMarker->m_iTime = (xuint)_PROPERTY->GetInt();
	}
}

// =============================================================================
// Nat Ryall                                                         03-Sep-2008
// =============================================================================
CSoundMetadata::~CSoundMetadata()
{
	ResourceManager.ReleaseResourceFile(m_pFile);

	XEN_LIST_ERASE_ALL(m_lpMarkers);
}

// =============================================================================
// Nat Ryall                                                         04-Sep-2008
// =============================================================================
CSoundMetadata::CMarker* CSoundMetadata::FindMarker(const xchar* pName)
{
	XEN_LIST_FOREACH(t_MarkerList, ppMarker, m_lpMarkers)
	{
		if (String::IsMatch((*ppMarker)->m_pName, pName))
			return *ppMarker;
	}

	return NULL;
}

//##############################################################################

//##############################################################################
//
//                                   SOUND
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         16-Sep-2008
// =============================================================================
CSound::CSound(CSoundMetadata* pMetadata) : 
	m_pMetadata(pMetadata),
	m_pChannel(NULL)
{
}

// =============================================================================
// Nat Ryall                                                         16-Sep-2008
// =============================================================================
CSound::~CSound()
{
	Stop();
}

// =============================================================================
// Nat Ryall                                                         16-Sep-2008
// =============================================================================
void CSound::Play(const xchar* pName)
{
	// If the channel exists already, free it.
	Stop();

	// Start the sound playing.
	_FMOD->playSound(FMOD_CHANNEL_FREE, m_pMetadata->GetSound(), false, &m_pChannel);
}

// =============================================================================
// Nat Ryall                                                         16-Sep-2008
// =============================================================================
void CSound::Stop()
{
	if (m_pChannel)
	{
		m_pChannel->stop();
		m_pChannel = NULL;
	}
}

// =============================================================================
// Nat Ryall                                                         16-Sep-2008
// =============================================================================
xuint CSound::GetMarkerTime(const xchar* pName)
{
	if (CSoundMetadata::CMarker* pMarker = m_pMetadata->FindMarker(pName))
		return pMarker->m_iTime;

	return 0;
}

// =============================================================================
// Nat Ryall                                                         16-Sep-2008
// =============================================================================
xbool CSound::IsMarkerExpired(const xchar* pName)
{
	if (CSoundMetadata::CMarker* pMarker = m_pMetadata->FindMarker(pName))
		return (GetTime() > pMarker->m_iTime);

	return true;
}

//##############################################################################

//##############################################################################
//
//                               SOUND MANAGER
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         11-Sep-2008
// =============================================================================
CSoundManager::CSoundManager() :
	m_pSoundSystem(NULL)
{
}

// =============================================================================
// Nat Ryall                                                         11-Sep-2008
// =============================================================================
void CSoundManager::OnInitialise()
{
	if (FMOD::System_Create(&m_pSoundSystem) == FMOD_OK)
	{
		if (m_pSoundSystem->init(32, FMOD_INIT_NORMAL, NULL) != FMOD_OK)
		{
			m_pSoundSystem->release();
			m_pSoundSystem = NULL;
		}
	}
	else
		m_pSoundSystem = NULL;
}

// =============================================================================
// Nat Ryall                                                         11-Sep-2008
// =============================================================================
void CSoundManager::OnDeinitialise()
{
	if (m_pSoundSystem)
	{
		m_pSoundSystem->release();
		m_pSoundSystem = NULL;
	}
}

// =============================================================================
// Nat Ryall                                                         11-Sep-2008
// =============================================================================
void CSoundManager::OnUpdate()
{
	m_pSoundSystem->update();
}

//##############################################################################
