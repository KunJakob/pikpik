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

//##############################################################################

//##############################################################################
//
//                                   TYPES
//
//##############################################################################

// Lists.
typedef XLIST<CResourceFile*> t_ResourceFileList;
typedef XLIST<CResourceMetadata*> t_ResourceMetadataList;

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
CResourceFile::CResourceFile(t_ResourceType iResourceType, const XCHAR* pResourceFile) :
	iType(iResourceType),
	pResource(NULL),
	pFile(pResourceFile),
	iReferenceCount(1)
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
	iType(iResourceType),
	pName(pDataset->GetName())
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
	for (XUINT iA = 0; iA < ResourceType_Max; ++iA)
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
}

// =============================================================================
// Nat Ryall                                                         22-Apr-2008
// =============================================================================
CResourceFile* ResourceManager::AllocResourceFile(t_ResourceType iType, const XCHAR* pFile)
{
	XEN_LIST_FOREACH(t_ResourceFileList, ppResourceFile, s_lpResourceFiles[iType])
	{
		if (strcmp((*ppResourceFile)->pFile, pFile) == 0)
		{
			(*ppResourceFile)->iReferenceCount++;
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
	XEN_LIST_FOREACH(t_ResourceFileList, ppResourceFile, s_lpResourceFiles[pFile->iType])
	{
		if (*ppResourceFile == pFile)
		{
			if (--pFile->iReferenceCount == 0)
				delete pFile;
		}
	}
}

// =============================================================================
// Nat Ryall                                                         22-Apr-2008
// =============================================================================
CResourceMetadata* ResourceManager::FindResource(t_ResourceType iType, const XCHAR* pName)
{
	XEN_LIST_FOREACH(t_ResourceMetadataList, ppResource, s_lpResourceMetadata[iType])
	{
		if (strcmp((*ppResource)->pName, pName) == 0)
			return *ppResource;
	}

	return NULL;
}

//##############################################################################