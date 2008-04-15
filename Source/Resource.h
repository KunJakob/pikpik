#pragma once

/**
* @file Resource.h
* @author Nat Ryall
* @date 18/02/2008
* @brief Manages all game resources.
*
* Copyright © Creative Disorder
*/

#pragma region Include
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
#pragma endregion

#pragma region Types
//##############################################################################
//
//                                   TYPES
//
//##############################################################################

/**
* Pre-declarations.
*/
class CSprite;

/**
* List of supported resource types.
*/
enum t_ResourceType
{
  ResourceType_Unknown,       // The resource type is unknown.
  ResourceType_Surface,       // The resource is a surface.
  ResourceType_Sprite,        // The resource is a sprite.
};

//##############################################################################
#pragma endregion

#pragma region Base
//##############################################################################
//
//                                    BASE
//
//##############################################################################

/**
* Base class for resource templates.
*/
class CResourceTemplate
{
public:
  // The type of the resource template.
  t_ResourceType iResourceType;

  // The name of the resource template.
  const XCHAR* pResourceName;
};

/**
* Base class for resource objects.
*/
class CResource
{
public:
  /**
  * Get the resource type specified within the resource class constructor.
  */
  t_ResourceType GetResourceType()
  {
    return m_iResourceType;
  }

	/**
	* Destructor: Ensure any derived object is properly destroyed.
	*/
	virtual ~CResource() {}

protected:
  /**
  * Constructor: Initialise the resource type.
  */
  CResource(t_ResourceType iType) : m_iResourceType(iType) {}

private:
  // The resource type.
  t_ResourceType m_iResourceType;
};

//##############################################################################
#pragma endregion

#pragma region Declaration
//##############################################################################
//
//                                 DECLARATION
//
//##############################################################################
namespace ResourceManager
{
	/**
	* Unload all metadata and free all associated resources.
	*/
	void Reset();

  /**
  * Load a metadata file and any resources associated with it. 
  */
  void LoadMetadata(CMetadata* pMetadata);

  /**
  * Unload a loaded metadata file and any resources associated with it.
  */
  void UnloadMetadata(CMetadata* pMetadata);

  /**
  * Create a new sprite by name as it appears in the metadata.
  */
  CSprite* CreateSprite(const XCHAR* pName);

  /**
  * Free a resource of any type.
  */
  void FreeResource(CResource* pResource);
}

//##############################################################################
#pragma endregion