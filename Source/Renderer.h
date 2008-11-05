#pragma once

/**
* @file Renderer.h
* @author Nat Ryall
* @date 25/02/2008
* @brief Manages automatic object rendering.
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

//##############################################################################
//
//                                   MACROS
//
//##############################################################################

// Shortcuts.
#define RenderManager CRenderManager::Get()
#define RenderLayer(LAYERINDEX) CRenderManager::Get().GetLayer(LAYERINDEX)

//##############################################################################
//
//                                   TYPES
//
//##############################################################################

// Predeclare.
class CRenderable;
class CRenderLayer;

// Callbacks.
typedef xfunction(0)<> t_RenderOverrideCallback;
typedef xfunction(1)<CRenderLayer* /*Layer*/> t_RenderLayerOverrideCallback;

// Lists.
typedef xarray<CRenderable*> t_RenderableList;
typedef xarray<CRenderLayer*> t_RenderLayerList;

//##############################################################################
//
//                                 RENDERABLE
//
//##############################################################################
class CRenderable
{
public:
	// Constructor.
	CRenderable(xuint iRenderableType) : m_iRenderableType(iRenderableType) {}

	// Destructor.
	virtual ~CRenderable() {}

	// Render the object.
	virtual void OnRender() = 0;

	// Get the renderable type assigned to this renderable.
	xuint GetRenderableType()
	{
		return m_iRenderableType;
	}

protected:
	// The renderable type used for identification of renderables.
	xuint m_iRenderableType;
};

//##############################################################################
//
//                           RENDER TRANSFORMATION
//
//##############################################################################
class CRenderTransformation
{
public:
	// Constructor.
	CRenderTransformation();

	// Reset the transformation to nothing.
	void Reset();

	// Apply the current transformation.
	void Apply();

	// The positional transformation.
	xpoint m_xPosition;

	// The rotational transformation (in degrees).
	xfloat m_fRotation;

	// The horizontal scaling transformation.
	xfloat m_fHorizontalScale;

	// The vertical scaling transformation.
	xfloat m_fVerticalScale;
};

//##############################################################################
//
//                                RENDER LAYER
//
//##############################################################################
class CRenderLayer
{
public:
	// Constructor.
	CRenderLayer(xuint iLayerIndex);

	// Get the local layer index.
	xuint GetLayerIndex()
	{
		return m_iLayerIndex;
	}

	// Detach all renderables.
	void ClearRenderables();

	// Add a renderable to the layer.
	// ~note Renderables will be drawn in the order they are added unless manually overriden.
	// ~note Lower layers are obscured by higher layers.
	void AttachRenderable(CRenderable* pRenderable);

	// Find and remove a renderable from the layer.
	void DetachRenderable(CRenderable* pRenderable);

	// Determine if the specified renderable is attached to the layer.
	xbool IsRenderableAttached(CRenderable* pRenderable);

	// Get the number of renderables being managed by this layer.
	xint GetRenderableCount();

	// Get a specific renderable by index.
	CRenderable* GetRenderable(xint iIndex);

	// Enable/disable rendering for this layer. 
	// ~note Layers are enabled by default.
	void SetEnabled(xbool bEnabled);

	// Check if this layer is enabled.
	// ~note Layers are enabled by default.
	xbool IsEnabled();

	// Specify a function to override the default render process for this layer.
	void SetRenderOverride(t_RenderLayerOverrideCallback fpCallback);

	// Set the transformation to be applied to all renderables on the layer.
	void SetTransformation(xpoint xPosition = xpoint(), xfloat fRotation = 0.f, xfloat fHorizontalScale = 1.f, xfloat fVerticalScale = 1.f);

	// Get the currently applied transformation params for a specific layer.
	CRenderTransformation& GetTransformation();

	// Activate the transformation and render using the render override or using the default render pipeline.
	void Render();

private:
	// The layer index.
	xuint m_iLayerIndex;

	// The enabled/disabled status of this layer.
	xbool m_bEnabled;

	// The render layer override callback.
	t_RenderLayerOverrideCallback m_fpRenderOverrideCallback;

	// The renderable list for this layer.
	t_RenderableList m_lpRenderables;

	// The render layer transformation.
	CRenderTransformation m_xTransformation;
};

//##############################################################################
//
//                               RENDER MANAGER
//
//##############################################################################
class CRenderManager : public CModule
{
public:
	// Singleton instance.
	static inline CRenderManager& Get() 
	{
		static CRenderManager s_Instance;
		return s_Instance;
	}

	// Initialise.
	virtual void OnInitialise();

	// Deinitialise.
	virtual void OnDeinitialise();

	// Specify a function to override the default render process.
	void SetRenderOverride(t_RenderOverrideCallback fpCallback);

	// Destroy all existing layers and create the specified number of new layers.
	// ~iLayerCount The number of layers to create after resetting the system. Set to -1 for the current number of layers.
	void ReinitLayers(xint iLayerCount = -1);

	// Destroy all existing layers.
	void ClearLayers();

	// Get the number of active layers in the system.
	xint GetLayerCount();

	// Get a specific render layer by index.
	CRenderLayer* GetLayer(xint iIndex);

	// Called to render all enabled render layers and objects.
	void Render();

	// Render a box to the specified rect dimensions.
	// ~bFilled Specifies if the box should be filled or just a border.
	void RenderBox(xbool bFilled, xrect xRect, xuint iColour);

protected:
	// The global render override. This is used to bypass the render manager completely.
	t_RenderOverrideCallback m_fpRenderOverrideCallback;

	// The renderable list.
	t_RenderLayerList m_lpLayerList;
};

//##############################################################################
