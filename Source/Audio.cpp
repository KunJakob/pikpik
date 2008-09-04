/**
* @file Sound.cpp
* @author Nat Ryall
* @date 11/08/2008
* @brief Audio management.
*
* Copyright � SAPIAN
*/

//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Global.
#include <Global.h>

// Local.
#include <Audio.h>

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
	/*// Areas.
	_DATASET_FOREACH(pAreaDataset, pDataset, "Area", NULL)
	{
		CArea* pArea = new CArea;
		m_lpAreas.push_back(pArea);

		pArea->m_pName = pAreaDataset->GetName();

		if (_PROPERTY_EXISTS(pAreaDataset, "Rect"))
			pArea->m_xRect = _PROPERTY->GetRect();
		else if (_PROPERTY_EXISTS(pAreaDataset, "Size"))
			pArea->m_xRect = xrect(_PROPERTY->GetPoint(0), _PROPERTY->GetPoint(0) + _PROPERTY->GetPoint(1));
		else if (_PROPERTY_EXISTS(pAreaDataset, "Tile"))
			pArea->m_xRect = xrect(_PROPERTY->GetInt(0) * _PROPERTY->GetInt(1), 0, (_PROPERTY->GetInt(0) + 1) * _PROPERTY->GetInt(1), _PROPERTY->GetInt(2));
	}*/

	// File.
	m_pFile = (CSoundFile*)ResourceManager::CreateResourceFile(ResourceType_Sound, pDataset->GetProperty("File")->GetString());
}

// =============================================================================
// Nat Ryall                                                         03-Sep-2008
// =============================================================================
CSoundMetadata::~CSoundMetadata()
{
	ResourceManager::ReleaseResourceFile(m_pFile);
}

//##############################################################################

//##############################################################################
//
//                               SOUND MANAGER
//
//##############################################################################


//##############################################################################
