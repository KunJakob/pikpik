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

//##############################################################################
//
//                                   MACROS
//
//##############################################################################

// Shortcuts.
#define RenderManager CRenderManager::Get()

// The maximum number of layers allowed in the renderer.
#define RENDERER_MAXLAYERS 16

//##############################################################################

//##############################################################################

//##############################################################################
//
//                                   TYPES
//
//##############################################################################

// Predeclare.
class CRenderable;

// Callbacks.
typedef xfunction(1)<CRenderable* /*Renderable*/> t_RenderCallback;

// Lists.
typedef xlist<CRenderable*> t_RenderableList;

//##############################################################################

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

	// Update the object ready for rendering.
	virtual void Update() {}

	// Render the object.
	virtual void Render() = 0;

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

//##############################################################################
//
//                           RENDER TRANSFORMATION
//
//##############################################################################
class CRenderTransformation
{
public:
	// Reset the transformation to nothing.
	void Reset();

	// Apply the current transformation.
	void Apply();

	// The positional transformation.
	xpoint m_xPosition;

	// The centre point transformation.
	xpoint m_xCentre;

	// The rotational transformation (in degrees).
	xfloat m_fRotation;

	// The horizontal scaling transformation.
	xfloat m_fHorizontalScale;

	// The vertical scaling transformation.
	xfloat m_fVerticalScale;
};

//##############################################################################

//##############################################################################
//
//                                RENDER LAYER
//
//##############################################################################
class CRenderLayer
{
public:
	// The layer index.
	xuint m_iLayer;

	// The enabled/disabled status of this layer.
	xbool m_bEnabled;

	// The render override callback.
	t_RenderCallback m_fpRenderCallback;

	// The renderable list for this layer.
	t_RenderableList m_lpRenderables;

	// The render layer transformation.
	CRenderTransformation m_xTransformation;
};

//##############################################################################

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

	// Update.
	virtual void OnUpdate();

	// Render.
	virtual void OnRender();

	// Remove all existing renderables.
	void Reset();

	// Add a renderable to the system on the specified layer (0 to RENDERER_MAXLAYERS-1).
	// ~note Lower layers are obscured by higher layers.
	void Add(xuint iLayer, CRenderable* pRenderable);

	// Find and remove a renderable from the system.
	void Remove(CRenderable* pRenderable);

	// Enable a layer and allow update/rendering. Layers are enabled by default.
	void EnableLayer(xuint iLayer);

	// Disable a layer and stop update/rendering. Layers are enabled by default.
	void DisableLayer(xuint iLayer);

	// Check if a specific layer is enabled.
	xbool IsLayerEnabled(xuint iLayer);

	// Specify a function to override the render process for each renderable on a specific layer.
	// ~note Renderables must have Render() called within the override for the object to appear.
	void SetRenderCallback(xuint iLayer, t_RenderCallback fpCallback);

	// Set the transformation for a specific layer. The transformation will be applied to all renderables on the layer.
	void SetTransformation(xuint iLayer, xpoint xPosition = xpoint(), xpoint xCentre = xpoint(), xfloat fRotation = 0.f, xfloat fHorizontalScale = 1.f, xfloat fVerticalScale = 1.f);

	// Get the currently applied transformation params for a specific layer.
	CRenderTransformation& GetTransformation(xuint iLayer);

protected:
	// The renderable list.
	CRenderLayer m_xLayers[RENDERER_MAXLAYERS];
};

//##############################################################################