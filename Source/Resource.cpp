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
#include <Sound.h>

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
void CResourceManager::OnDeinitialise()
{
	Clear();
}

// =============================================================================
// Nat Ryall                                                         04-Sep-2008
// =============================================================================
void CResourceManager::Clear()
{
	for (xint iA = 0; iA < ResourceType_Max; ++iA)
	{
		XEN_LIST_ERASE_ALL(m_lpResourceMetadata[iA]);
	}
}

// =============================================================================
// Nat Ryall                                                         22-Apr-2008
// =============================================================================
void CResourceManager::Load(CMetadata* pMetadata)
{
	META_DATASET_FOREACH(pDataset, pMetadata, "Sprite", NULL)
		m_lpResourceMetadata[ResourceType_Sprite].push_back(new CSpriteMetadata(pDataset));

	META_DATASET_FOREACH(pDataset, pMetadata, "Font", NULL)
		m_lpResourceMetadata[ResourceType_Font].push_back(new CFontMetadata(pDataset));

	META_DATASET_FOREACH(pDataset, pMetadata, "Sound", NULL)
		m_lpResourceMetadata[ResourceType_Sound].push_back(new CSoundMetadata(pDataset));
}

// =============================================================================
// Nat Ryall                                                         04-Sep-2008
// =============================================================================
void CResourceManager::Unload(CMetadata* pMetadata)
{
	XMASSERT(false, "This feature is not yet implemented.");
}

// =============================================================================
// Nat Ryall                                                         22-Apr-2008
// =============================================================================
CResourceFile* CResourceManager::CreateResourceFile(t_ResourceType iType, const xchar* pFile)
{
	XMASSERT(GetFileAttributes(pFile) != INVALID_FILE_ATTRIBUTES, XFORMAT("Could not find file: %s", pFile));

	// If we are allowed to re-use resources, check for an existing resource file we can use.
	static const xbool s_bRecycleResources[ResourceType_Max] = 
	{ 
		/*ResourceType_Sprite*/ true, /*ResourceType_Font*/ true, /*ResourceType_Sound*/ false,	
	};

	if (s_bRecycleResources[iType])
	{
		XEN_LIST_FOREACH(t_ResourceFileList, ppResourceFile, m_lpResourceFiles[iType])
		{
			if (String::IsMatch((*ppResourceFile)->m_pFile, pFile))
			{
				(*ppResourceFile)->m_iReferenceCount++;
				return *ppResourceFile;
			}
		}
	}

	// Otherwise, create a new resource instance.
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
		m_lpResourceFiles[iType].push_back(pResourceFile);

	return pResourceFile;
}

// =============================================================================
// Nat Ryall                                                         22-Apr-2008
// =============================================================================
void CResourceManager::ReleaseResourceFile(CResourceFile* pFile)
{
	XEN_LIST_FOREACH(t_ResourceFileList, ppResourceFile, m_lpResourceFiles[pFile->m_iType])
	{
		if (*ppResourceFile == pFile)
		{
			if (--pFile->m_iReferenceCount == 0)
			{
				m_lpResourceFiles[pFile->m_iType].erase(ppResourceFile);
				delete pFile;

				break;
			}
		}
	}
}

// =============================================================================
// Nat Ryall                                                         22-Apr-2008
// =============================================================================
CResourceMetadata* CResourceManager::GetResourceMetadata(t_ResourceType iType, const xchar* pName)
{
	XEN_LIST_FOREACH(t_ResourceMetadataList, ppResource, m_lpResourceMetadata[iType])
	{
		if (String::IsMatch((*ppResource)->m_pName, pName))
			return *ppResource;
	}

	XMASSERT(false, XFORMAT("Could not find the resource metadata '%s'.", pName));

	return NULL;
}

//##############################################################################
