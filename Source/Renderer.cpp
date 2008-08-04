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

//##############################################################################
//
//                                 DEFINITION
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         28-Jul-2008
// =============================================================================
void CRenderManager::OnInitialise()
{
	Reset();
}

// =============================================================================
// Nat Ryall                                                         28-Jul-2008
// =============================================================================
void CRenderManager::OnUpdate()
{
	for (xuint iA = 0; iA < RENDERER_MAXLAYERS; ++iA)
	{
		if (!m_xLayers[iA].m_bEnabled)
			continue;

		XEN_LIST_FOREACH(t_RenderableList, ppRenderable, m_xLayers[iA].m_lpRenderables)
			(*ppRenderable)->Update();
	}
}

// =============================================================================
// Nat Ryall                                                         28-Jul-2008
// =============================================================================
void CRenderManager::OnRender()
{
	for (xuint iA = 0; iA < RENDERER_MAXLAYERS; ++iA)
	{
		if (!m_xLayers[iA].m_bEnabled)
			continue;

		XEN_LIST_FOREACH(t_RenderableList, ppRenderable, m_xLayers[iA].m_lpRenderables)
		{
			if (m_xLayers[iA].m_fpRenderCallback)
				m_xLayers[iA].m_fpRenderCallback(*ppRenderable);
			else
				(*ppRenderable)->Render();
		}
	}
}

// =============================================================================
// Nat Ryall                                                          8-Apr-2008
// =============================================================================
void CRenderManager::Reset()
{
	for (xuint iA = 0; iA < RENDERER_MAXLAYERS; ++iA)
	{
		m_xLayers[iA].m_iLayer = iA;
		m_xLayers[iA].m_bEnabled = true;
		m_xLayers[iA].m_fpRenderCallback = NULL;
		m_xLayers[iA].m_lpRenderables.clear();
	}
}

// =============================================================================
// Nat Ryall                                                          8-Apr-2008
// =============================================================================
void CRenderManager::Add(xuint iLayer, CRenderable* pRenderable)
{
	XMASSERT(iLayer < RENDERER_MAXLAYERS, "Layer index out of bounds.");
	m_xLayers[iLayer].m_lpRenderables.push_back(pRenderable);
}

// =============================================================================
// Nat Ryall                                                          8-Apr-2008
// =============================================================================
void CRenderManager::Remove(CRenderable* pRenderable)
{
	for (xuint iA = 0; iA < RENDERER_MAXLAYERS; ++iA)
		m_xLayers[iA].m_lpRenderables.remove(pRenderable);
}

// =============================================================================
// Nat Ryall                                                         14-Apr-2008
// =============================================================================
void CRenderManager::EnableLayer(xuint iLayer)
{
	XMASSERT(iLayer < RENDERER_MAXLAYERS, "Layer index out of bounds.");
	m_xLayers[iLayer].m_bEnabled = true;
}

// =============================================================================
// Nat Ryall                                                         14-Apr-2008
// =============================================================================
void CRenderManager::DisableLayer(xuint iLayer)
{
	XMASSERT(iLayer < RENDERER_MAXLAYERS, "Layer index out of bounds.");
	m_xLayers[iLayer].m_bEnabled = false;
}

// =============================================================================
// Nat Ryall                                                         14-Apr-2008
// =============================================================================
xbool CRenderManager::IsLayerEnabled(xuint iLayer)
{
	XMASSERT(iLayer < RENDERER_MAXLAYERS, "Layer index out of bounds.");
	return m_xLayers[iLayer].m_bEnabled;
}

// =============================================================================
// Nat Ryall                                                         14-Apr-2008
// =============================================================================
void CRenderManager::SetRenderCallback(xuint iLayer, t_RenderCallback fpCallback)
{
	XMASSERT(iLayer < RENDERER_MAXLAYERS, "Layer index out of bounds.");
	m_xLayers[iLayer].m_fpRenderCallback = fpCallback;
}

//##############################################################################
