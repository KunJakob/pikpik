#pragma once

/**
* @file Renderer.h
* @author Nat Ryall
* @date 25/02/2008
* @brief Manages automatic object rendering.
*
* Copyright © SAPIAN
*/

#pragma region Include
//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Global.
#include <Global.h>

//##############################################################################
#pragma endregion

#pragma region Macros
//##############################################################################
//
//                                   MACROS
//
//##############################################################################
#define RENDERER_MAXLAYERS 16

//##############################################################################
#pragma endregion

#pragma region Types
//##############################################################################
//
//                                   TYPES
//
//##############################################################################

// Predeclare.
class CRenderable;

// Callbacks.
typedef void (*t_fpRenderCallback)(CRenderable* /*Renderable*/);

// Lists.
typedef XLIST<CRenderable*> t_RenderableList;

//##############################################################################
#pragma endregion

#pragma region Base
//##############################################################################
//
//                                    BASE
//
//##############################################################################
class CRenderable
{
public:
	/**
	* Constructor.
	*/
	CRenderable(XUINT iRenderableType) : m_iRenderableType(iRenderableType) {}

	/**
	* Destructor.
	*/
	virtual ~CRenderable() {}

  /**
  * Update the object ready for rendering.
  */
  virtual void Update() {}

  /**
  * Render the object.
  */
  virtual void Render() = 0;

	/**
	* Get the renderable type assigned to this renderable.
	*/
	XUINT GetRenderableType()
	{
		return m_iRenderableType;
	}

protected:
	// The renderable type used for identification of renderables.
	XUINT m_iRenderableType;
};

//##############################################################################
#pragma endregion

#pragma region Layer
//##############################################################################
//
//                                   LAYER
//
//##############################################################################
class CRenderLayer
{
public:
	// The layer index.
	XUINT iLayer;

	// The enabled/disabled status of this layer.
	XBOOL bEnabled;

	// The render override callback.
	t_fpRenderCallback fpRenderCallback;

	// The renderable list for this layer.
	t_RenderableList lpRenderables;
};

//##############################################################################
#pragma endregion

#pragma region Declaration
//##############################################################################
//
//                                 DECLARATION
//
//##############################################################################
namespace RenderManager
{
	/**
	* Remove all existing renderables.
	*/
	void Reset();

  /**
  * Add a renderable to the system on the specified layer (0 to RENDERER_MAXLAYERS-1).
  * @note Lower layers are obscured by higher layers.
  */
  void Add(XUINT iLayer, CRenderable* pRenderable);

  /**
  * Find and remove a renderable from the system.
  */
  void Remove(CRenderable* pRenderable);

	/**
	* Enable a layer and allow update/rendering. Layers are enabled by default.
	*/
	void EnableLayer(XUINT iLayer);

	/**
	* Disable a layer and stop update/rendering. Layers are enabled by default.
	*/
	void DisableLayer(XUINT iLayer);

	/**
	* Check if a specific layer is enabled.
	*/
	XBOOL IsLayerEnabled(XUINT iLayer);

	/**
	* Specify a function to override the render process for each renderable on a specific layer.
	* @note Renderables must have Render() called within the override for the object to appear.
	*/
	void SetRenderCallback(XUINT iLayer, t_fpRenderCallback fpCallback);
}

//##############################################################################
#pragma endregion