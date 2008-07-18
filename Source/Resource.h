#pragma once

/**
* @file Resource.h
* @author Nat Ryall
* @date 22/04/2008 (Original: 18/02/2008)
* 
* The resource manager is designed to simplify the import of constant object 
* values through the use of metadata. Resources will internally share the same
* file data if more than one request is made on a specific file meaning data is 
* never duplicated.
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

// Other.
#include <Metadata.h>

//##############################################################################

//##############################################################################
//
//                                   TYPES
//
//##############################################################################

// Pre-declare.
class CResourceFile;
class CResourceMetadata;

// List of supported resource types.
enum t_ResourceType
{
	ResourceType_Sprite,
	ResourceType_Font,
	/*MAX*/ResourceType_Max,
};

//##############################################################################

//##############################################################################
//
//                               RESOURCE FILE
//
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
	t_ResourceType iType;

	// The resource object.
	void* pResource;

	// The resource file name.
	const xchar* pFile;

	// The resource reference count.
	xuint iReferenceCount;

protected:
	/**
	* Constructor: Initialise the file.
	*/
	CResourceFile(t_ResourceType iResourceType, const xchar* pResourceFile);
};

//##############################################################################

//##############################################################################
//
//                             RESOURCE METADATA
//
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
	t_ResourceType iType;

	// The resource name.
	const xchar* pName;

protected:
	/**
	* Constructor: Initialise the metadata.
	*/
	CResourceMetadata(t_ResourceType iResourceType, CDataset* pDataset);
};

//##############################################################################

//##############################################################################
//
//                              RESOURCE MANAGER
//
//##############################################################################
namespace ResourceManager
{
	/**
	* Deregister all metadata and in doing so, remove all existing resources.
	*/
	void Reset();

	/**
	* Register a metadata file with the system and parse all valid, managed 
	* resources described within the data.
	*/
	void RegisterMetadata(CMetadata* pMetadata);

	/**
	* Allocate a new or existing resource file with the specified type and file.
	*/
	CResourceFile* AllocResourceFile(t_ResourceType iType, const xchar* pFile);

	/**
	* Release a resource file and destroy it if there are no remaining references.
	*/
	void ReleaseResourceFile(CResourceFile* pFile);

	/**
	* Find the metadata for a managed resource with the specified type and name.
	* @return The resource metadata or NULL if not found.
	*/
	CResourceMetadata* FindResource(t_ResourceType iType, const xchar* pName);
}

//##############################################################################