#pragma once

/**
* @file Resource.h
* @author Nat Ryall
* @date 22/04/2008 (Original: 18/02/2008)
* 
* The resource manager is designed to simplify the import of constant object 
* values through the use of metadata. Resources can internally share the same
* file data if more than one request is made on a specific file meaning data
* can be recycled.
*
* Copyright © SAPIAN
*/

//##############################################################################

// Global.
#include <Global.h>

//##############################################################################

// Shortcuts.
#define ResourceManager CResourceManager::Get()

//##############################################################################

// Pre-declare.
class CResourceFile;
class CResourceMetadata;

// List of supported resource types.
enum t_ResourceType
{
	ResourceType_Sprite,
	ResourceType_Font,
	ResourceType_Sound,
	/*MAX*/ResourceType_Max,
};

//##############################################################################

/**
* Management class for sharing internal resource types.
*/
class CResourceFile
{
public:
	/**
	* Destructor: Ensure any derived object is properly destroyed.
	*/
	virtual ~CResourceFile() {}

	// The resource type.
	t_ResourceType m_iType;

	// The resource object.
	void* m_pResource;

	// The resource file name.
	const xchar* m_pFile;

	// The resource reference count.
	xuint m_iReferenceCount;

protected:
	/**
	* Constructor: Initialise the file.
	*/
	CResourceFile(t_ResourceType iResourceType, const xchar* pResourceFile);
};

//##############################################################################

/**
* Base class for meta-based resources (object templates).
*/
class CResourceMetadata
{
public:
	/**
	* Destructor: Ensure any derived object is properly destroyed.
	*/
	virtual ~CResourceMetadata() {}

	// The resource type.
	t_ResourceType m_iType;

	// The resource name.
	const xchar* m_pName;

protected:
	/**
	* Constructor: Initialise the metadata.
	*/
	CResourceMetadata(t_ResourceType iResourceType, CDataset* pDataset);
};

//##############################################################################
class CResourceManager : public CModule
{
public:
	// Singleton instance.
	static inline CResourceManager& Get() 
	{
		static CResourceManager s_Instance;
		return s_Instance;
	}

	// Deregister all metadata and in doing so, remove all existing resources.
	virtual void OnDeinitialise();

	// Free all resources in the system.
	void Clear();

	// Load all resources specified within a metadata file.
	void Load(CMetadata* pMetadata);

	// Unload all resources specified within a metadata file.
	void Unload(CMetadata* pMetadata);

	// Create a resource file with the specified type and file.
	CResourceFile* CreateResourceFile(t_ResourceType iType, const xchar* pFile);

	// Release a resource file and destroy it if there are no remaining references.
	void ReleaseResourceFile(CResourceFile* pFile);

	// Find the metadata for a managed resource with the specified type and name.
	// ~return The resource metadata or NULL if not found.
	CResourceMetadata* GetResourceMetadata(t_ResourceType iType, const xchar* pName);

protected:
	// Lists.
	typedef xlist<CResourceFile*> t_ResourceFileList;
	typedef xlist<CResourceMetadata*> t_ResourceMetadataList;

	// The categorised lists of managed resource files.
	t_ResourceFileList m_lpResourceFiles[ResourceType_Max];

	// The categorised lists of managed metadata.
	t_ResourceMetadataList m_lpResourceMetadata[ResourceType_Max];
};

//##############################################################################
