#pragma region Include
//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Global.
#include <Global.h>

// Local.
#include <Renderer.h>

//##############################################################################
#pragma endregion

#pragma region Static
//##############################################################################
//
//                                   STATIC
//
//##############################################################################

// The renderable list.
static CRenderLayer s_xLayers[RENDERER_MAXLAYERS];

//##############################################################################
#pragma endregion

#pragma region ModuleManager
//##############################################################################
//
//                                   MODULE
//
//##############################################################################
static class CRenderModule : public Xen::CModule
{
public:
	// Constructor.
	CRenderModule()
	{
		XMODULE(this);
	}

	// Initialise.
	virtual void Initialise()
	{
		RenderManager::Reset();
	}

	// Update.
	virtual void Update()
	{
		for (xuint iA = 0; iA < RENDERER_MAXLAYERS; ++iA)
		{
			if (!s_xLayers[iA].bEnabled)
				continue;

			XEN_LIST_FOREACH(t_RenderableList, ppRenderable, s_xLayers[iA].lpRenderables)
				(*ppRenderable)->Update();
		}
	}

	// Render.
	virtual void Render()
	{
		for (xuint iA = 0; iA < RENDERER_MAXLAYERS; ++iA)
		{
			if (!s_xLayers[iA].bEnabled)
				continue;

			XEN_LIST_FOREACH(t_RenderableList, ppRenderable, s_xLayers[iA].lpRenderables)
			{
				if (s_xLayers[iA].fpRenderCallback)
					s_xLayers[iA].fpRenderCallback(*ppRenderable);
				else
					(*ppRenderable)->Render();
			}
		}
	}

	// Deinitialise.
	virtual void Deinitialise()
	{
		RenderManager::Reset();
	}
} 
s_Module;

//##############################################################################
#pragma endregion

#pragma region Definition
//##############################################################################
//
//                                 DEFINITION
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                          8-Apr-2008
// =============================================================================
void RenderManager::Reset()
{
	for (xuint iA = 0; iA < RENDERER_MAXLAYERS; ++iA)
	{
		s_xLayers[iA].iLayer = iA;
		s_xLayers[iA].bEnabled = true;
		s_xLayers[iA].fpRenderCallback = NULL;
		s_xLayers[iA].lpRenderables.clear();
	}
}

// =============================================================================
// Nat Ryall                                                          8-Apr-2008
// =============================================================================
void RenderManager::Add(xuint iLayer, CRenderable* pRenderable)
{
	XMASSERT(iLayer < RENDERER_MAXLAYERS, "Layer index out of bounds.");
	s_xLayers[iLayer].lpRenderables.push_back(pRenderable);
}

// =============================================================================
// Nat Ryall                                                          8-Apr-2008
// =============================================================================
void RenderManager::Remove(CRenderable* pRenderable)
{
	for (xuint iA = 0; iA < RENDERER_MAXLAYERS; ++iA)
		s_xLayers[iA].lpRenderables.remove(pRenderable);
}

// =============================================================================
// Nat Ryall                                                         14-Apr-2008
// =============================================================================
void RenderManager::EnableLayer(xuint iLayer)
{
	XMASSERT(iLayer < RENDERER_MAXLAYERS, "Layer index out of bounds.");
	s_xLayers[iLayer].bEnabled = true;
}

// =============================================================================
// Nat Ryall                                                         14-Apr-2008
// =============================================================================
void RenderManager::DisableLayer(xuint iLayer)
{
	XMASSERT(iLayer < RENDERER_MAXLAYERS, "Layer index out of bounds.");
	s_xLayers[iLayer].bEnabled = false;
}

// =============================================================================
// Nat Ryall                                                         14-Apr-2008
// =============================================================================
xbool RenderManager::IsLayerEnabled(xuint iLayer)
{
	XMASSERT(iLayer < RENDERER_MAXLAYERS, "Layer index out of bounds.");
	return s_xLayers[iLayer].bEnabled;
}

// =============================================================================
// Nat Ryall                                                         14-Apr-2008
// =============================================================================
void RenderManager::SetRenderCallback(xuint iLayer, t_RenderCallback fpCallback)
{
	XMASSERT(iLayer < RENDERER_MAXLAYERS, "Layer index out of bounds.");
	s_xLayers[iLayer].fpRenderCallback = fpCallback;
}

//##############################################################################
#pragma endregion