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
//                           RENDER TRANSFORMATION
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         18-Sep-2008
// =============================================================================
void CRenderTransformation::Reset()
{
	m_xPosition = xpoint();
	m_fRotation = 0.f;
	m_fHorizontalScale = 1.f;
	m_fVerticalScale = 1.f;
}

// =============================================================================
// Nat Ryall                                                         18-Sep-2008
// =============================================================================
void CRenderTransformation::Apply()
{
	_HGE->Gfx_SetTransform
	(
		0.f,
		0.f,
		(xfloat)m_xPosition.iX,
		(xfloat)m_xPosition.iY,
		m_fRotation,
		m_fHorizontalScale,
		m_fVerticalScale
	);
}

//##############################################################################

//##############################################################################
//
//                               RENDER MANAGER
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

		m_xLayers[iA].m_xTransformation.Apply();

		XEN_LIST_FOREACH(t_RenderableList, ppRenderable, m_xLayers[iA].m_lpRenderables)
		{
			if (m_xLayers[iA].m_fpRenderCallback)
				m_xLayers[iA].m_fpRenderCallback(*ppRenderable);
			else
				(*ppRenderable)->Render();
		}
	}

	_HGE->Gfx_SetTransform();
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
		m_xLayers[iA].m_xTransformation.Reset();
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

// =============================================================================
// Nat Ryall                                                         18-Sep-2008
// =============================================================================
void CRenderManager::SetTransformation(xuint iLayer, xpoint xPosition, xfloat fRotation, xfloat fHorizontalScale, xfloat fVerticalScale)
{
	XMASSERT(iLayer < RENDERER_MAXLAYERS, "Layer index out of bounds.");

	m_xLayers[iLayer].m_xTransformation.m_xPosition = xPosition;
	m_xLayers[iLayer].m_xTransformation.m_fRotation = fRotation;
	m_xLayers[iLayer].m_xTransformation.m_fHorizontalScale = fHorizontalScale;
	m_xLayers[iLayer].m_xTransformation.m_fVerticalScale = fVerticalScale;
}

// =============================================================================
// Nat Ryall                                                         18-Sep-2008
// =============================================================================
CRenderTransformation& CRenderManager::GetTransformation(xuint iLayer)
{
	XMASSERT(iLayer < RENDERER_MAXLAYERS, "Layer index out of bounds.");
	return m_xLayers[iLayer].m_xTransformation;
}

//##############################################################################
