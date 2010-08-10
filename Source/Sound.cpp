/**
* @file Sound.cpp
* @author Nat Ryall
* @date 11/08/2008
* @brief Audio management.
*
* Copyright © SAPIAN
*/

//##############################################################################

// Global.
#include <Global.h>

// Local.
#include <Sound.h>

//##############################################################################

// =============================================================================
CSoundFile::CSoundFile(const xchar* pFile, CDataset* pDataset) : CResourceFile(ResourceType_Sound, pFile)
{
	FMOD_MODE iCreationMode = FMOD_CREATESTREAM;

	if (!pDataset->GetProperty("Stream"))
	{
		iCreationMode = FMOD_CREATESAMPLE;

		// Load compressed files into memory without decompressing. Determine this by extension.
		static const xchar* s_pCompressedTypes[] = { "mp3", "ogg" };
		static const xint s_iCompressedTypesCount = sizeof(s_pCompressedTypes) / sizeof(xchar*);

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
	}

	// Create the sound.
	_FMOD->createSound(pFile, FMOD_SOFTWARE | iCreationMode, NULL, (FMOD::Sound**)&m_pResource);

	// Save the sound type we used.
	m_iSoundType = (t_SoundType)iCreationMode;
}

// =============================================================================
CSoundFile::~CSoundFile()
{
	GetResource()->release();
}

//##############################################################################

// =============================================================================
CSoundMetadata::CSoundMetadata(CDataset* pDataset) : CResourceMetadata(ResourceType_Sound, pDataset)
{
	// File.
	m_pFile = (CSoundFile*)ResourceManager.CreateResourceFile(ResourceType_Sound, pDataset->GetProperty("File")->GetString(), pDataset);

	// Info.
	m_iGroup = SoundGroup_Unknown;
	m_fVolume = 1.f;

	if (XEN_METADATA_PROPERTY_EXISTS(pDataset, "Group"))
		m_iGroup = (t_SoundGroup)XEN_METADATA_PROPERTY->GetInt();

	if (XEN_METADATA_PROPERTY_EXISTS(pDataset, "Volume"))
		m_fVolume = XEN_METADATA_PROPERTY->GetFloat();

	// Markers.
	XEN_METADATA_DATASET_FOREACH(pMarkerDataset, pDataset, "Marker", NULL)
	{
		CMarker* pMarker = new CMarker;
		m_lpMarkers.push_back(pMarker);

		pMarker->m_pName = pMarkerDataset->GetName();
		pMarker->m_iTime = 0;

		if (XEN_METADATA_PROPERTY_EXISTS(pMarkerDataset, "Time"))
			pMarker->m_iTime = (xuint)XEN_METADATA_PROPERTY->GetInt();
	}
}

// =============================================================================
CSoundMetadata::~CSoundMetadata()
{
	ResourceManager.ReleaseResourceFile(m_pFile);

	XEN_LIST_ERASE_ALL(m_lpMarkers);
}

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

// =============================================================================
CSound::CSound(CSoundMetadata* pMetadata) : 
	m_pMetadata(pMetadata),
	m_pChannel(NULL)
{
}

// =============================================================================
CSound::~CSound()
{
	Stop();
}

// =============================================================================
void CSound::Play(const xchar* pName)
{
	// If the channel exists already, free it.
	Stop();

	// Start the sound playing.
	_FMOD->playSound(FMOD_CHANNEL_FREE, m_pMetadata->GetSound(), false, &m_pChannel);
}

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
xuint CSound::GetMarkerTime(const xchar* pName)
{
	if (CSoundMetadata::CMarker* pMarker = m_pMetadata->FindMarker(pName))
		return pMarker->m_iTime;

	return 0;
}

// =============================================================================
xbool CSound::IsMarkerExpired(const xchar* pName)
{
	if (CSoundMetadata::CMarker* pMarker = m_pMetadata->FindMarker(pName))
		return (GetTime() > pMarker->m_iTime);

	return true;
}

//##############################################################################

// =============================================================================
CSoundManager::CSoundManager() :
	m_pSoundSystem(NULL)
{
}

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
void CSoundManager::OnDeinitialise()
{
	if (m_pSoundSystem)
	{
		m_pSoundSystem->release();
		m_pSoundSystem = NULL;
	}
}

// =============================================================================
void CSoundManager::OnUpdate()
{
	m_pSoundSystem->update();
}

//##############################################################################
