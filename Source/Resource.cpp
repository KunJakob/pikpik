//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Global.
#include <Global.h>

// Local.
#include <Resource.h>

// Other.
#include <Metadata.h>
#include <Dataset.h>
#include <Sprite.h>
#include <Font.h>
#include <Audio.h>

//##############################################################################

//##############################################################################
//
//                                   TYPES
//
//##############################################################################

// Lists.
typedef xlist<CResourceFile*> t_ResourceFileList;
typedef xlist<CResourceMetadata*> t_ResourceMetadataList;

//##############################################################################

//##############################################################################
//
//                                   STATIC
//
//##############################################################################

// Lists.
static t_ResourceFileList s_lpResourceFiles[ResourceType_Max];
static t_ResourceMetadataList s_lpResourceMetadata[ResourceType_Max];

//##############################################################################

//##############################################################################
//
//                               RESOURCE FILE
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         22-Apr-2008
// =============================================================================
CResourceFile::CResourceFile(t_ResourceType iResourceType, const xchar* pResourceFile) :
	m_iType(iResourceType),
	m_pResource(NULL),
	m_pFile(pResourceFile),
	m_iReferenceCount(1)
{
}

//##############################################################################

//##############################################################################
//
//                             RESOURCE METADATA
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         22-Apr-2008
// =============================================================================
CResourceMetadata::CResourceMetadata(t_ResourceType iResourceType, CDataset* pDataset) :
	m_iType(iResourceType),
	m_pName(pDataset->GetName())
{
}

//##############################################################################

//##############################################################################
//
//                              RESOURCE MANAGER
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         22-Apr-2008
// =============================================================================
void ResourceManager::Reset()
{
	for (xuint iA = 0; iA < ResourceType_Max; ++iA)
	{
		XEN_LIST_ERASE_ALL(s_lpResourceMetadata[iA]);
	}
}

// =============================================================================
// Nat Ryall                                                         22-Apr-2008
// =============================================================================
void ResourceManager::RegisterMetadata(CMetadata* pMetadata)
{
	_DATASET_FOREACH(pDataset, pMetadata, "Sprite", NULL)
		s_lpResourceMetadata[ResourceType_Sprite].push_back(new CSpriteMetadata(pDataset));

	_DATASET_FOREACH(pDataset, pMetadata, "Font", NULL)
		s_lpResourceMetadata[ResourceType_Font].push_back(new CFontMetadata(pDataset));

	_DATASET_FOREACH(pDataset, pMetadata, "Sound", NULL)
		s_lpResourceMetadata[ResourceType_Sound].push_back(new CSoundMetadata(pDataset));
}

// =============================================================================
// Nat Ryall                                                         22-Apr-2008
// =============================================================================
CResourceFile* ResourceManager::CreateResourceFile(t_ResourceType iType, const xchar* pFile)
{
	XMASSERT(GetFileAttributes(pFile) != INVALID_FILE_ATTRIBUTES, XFORMAT("Could not find file: %s", pFile));

	XEN_LIST_FOREACH(t_ResourceFileList, ppResourceFile, s_lpResourceFiles[iType])
	{
		if (strcmp((*ppResourceFile)->m_pFile, pFile) == 0)
		{
			(*ppResourceFile)->m_iReferenceCount++;
			return *ppResourceFile;
		}
	}

	CResourceFile* pResourceFile = NULL;

	switch (iType)
	{
	case ResourceType_Sprite:
		pResourceFile = new CSpriteFile(pFile);
		break;

	case ResourceType_Font:
		pResourceFile = new CFontFile(pFile);
		break;

	case ResourceType_Sound:
		pResourceFile = new CSoundFile(pFile);
		break;
	}

	if (pResourceFile)
		s_lpResourceFiles[iType].push_back(pResourceFile);

	return pResourceFile;
}

// =============================================================================
// Nat Ryall                                                         22-Apr-2008
// =============================================================================
void ResourceManager::ReleaseResourceFile(CResourceFile* pFile)
{
	XEN_LIST_FOREACH(t_ResourceFileList, ppResourceFile, s_lpResourceFiles[pFile->m_iType])
	{
		if (*ppResourceFile == pFile)
		{
			if (--pFile->m_iReferenceCount == 0)
				delete pFile;
		}
	}
}

// =============================================================================
// Nat Ryall                                                         22-Apr-2008
// =============================================================================
CResourceMetadata* ResourceManager::FindResource(t_ResourceType iType, const xchar* pName)
{
	XEN_LIST_FOREACH(t_ResourceMetadataList, ppResource, s_lpResourceMetadata[iType])
	{
		if (strcmp((*ppResource)->m_pName, pName) == 0)
			return *ppResource;
	}

	return NULL;
}

//##############################################################################