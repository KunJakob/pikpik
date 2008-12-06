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
//
//                           RENDER TRANSFORMATION
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         30-Oct-2008
// =============================================================================
CRenderTransformation::CRenderTransformation()
{
	Reset();
}

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
		(xfloat)m_xPosition.m_tX,
		(xfloat)m_xPosition.m_tY,
		m_fRotation,
		m_fHorizontalScale,
		m_fVerticalScale
	);
}

//##############################################################################
//
//                                RENDER LAYER
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         30-Oct-2008
// =============================================================================
CRenderLayer::CRenderLayer(xuint iLayerIndex) :
	m_iLayerIndex(iLayerIndex),
	m_bEnabled(true),
	m_fpRenderOverrideCallback(NULL)
{
}

// =============================================================================
// Nat Ryall                                                         30-Oct-2008
// =============================================================================
void CRenderLayer::ClearRenderables()
{
	m_lpRenderables.clear();
}

// =============================================================================
// Nat Ryall                                                          8-Apr-2008
// =============================================================================
void CRenderLayer::AttachRenderable(CRenderable* pRenderable)
{
	m_lpRenderables.push_back(pRenderable);
}

// =============================================================================
// Nat Ryall                                                          8-Apr-2008
// =============================================================================
void CRenderLayer::DetachRenderable(CRenderable* pRenderable)
{
	XLISTREMOVE(t_RenderableList, m_lpRenderables, pRenderable);
}

// =============================================================================
// Nat Ryall                                                         30-Oct-2008
// =============================================================================
xbool CRenderLayer::IsRenderableAttached(CRenderable* pRenderable)
{
	XLISTFOREACH(t_RenderableList, ppRenderable, m_lpRenderables)
	{
		if (*ppRenderable == pRenderable)
			return true;
	}

	return false;
}

// =============================================================================
// Nat Ryall                                                         30-Oct-2008
// =============================================================================
xint CRenderLayer::GetRenderableCount()
{
	return m_lpRenderables.size();
}

// =============================================================================
// Nat Ryall                                                         30-Oct-2008
// =============================================================================
CRenderable* CRenderLayer::GetRenderable(xint iIndex)
{
	XMASSERT(iIndex > (GetRenderableCount() - 1), XFORMAT("Renderable index %d is out of bounds.", iIndex));
	return m_lpRenderables[iIndex];
}

// =============================================================================
// Nat Ryall                                                         14-Apr-2008
// =============================================================================
void CRenderLayer::SetEnabled(xbool bEnabled)
{
	m_bEnabled = bEnabled;
}

// =============================================================================
// Nat Ryall                                                         14-Apr-2008
// =============================================================================
xbool CRenderLayer::IsEnabled()
{
	return m_bEnabled;
}

// =============================================================================
// Nat Ryall                                                         14-Apr-2008
// =============================================================================
void CRenderLayer::SetRenderOverride(t_RenderLayerOverrideCallback fpCallback)
{
	m_fpRenderOverrideCallback = fpCallback;
}

// =============================================================================
// Nat Ryall                                                         18-Sep-2008
// =============================================================================
void CRenderLayer::SetTransformation(xpoint xPosition, xfloat fRotation, xfloat fHorizontalScale, xfloat fVerticalScale)
{
	m_xTransformation.m_xPosition = xPosition;
	m_xTransformation.m_fRotation = fRotation;
	m_xTransformation.m_fHorizontalScale = fHorizontalScale;
	m_xTransformation.m_fVerticalScale = fVerticalScale;
}

// =============================================================================
// Nat Ryall                                                         18-Sep-2008
// =============================================================================
CRenderTransformation& CRenderLayer::GetTransformation()
{
	return m_xTransformation;
}

// =============================================================================
// Nat Ryall                                                         30-Oct-2008
// =============================================================================
void CRenderLayer::Render()
{
	m_xTransformation.Apply();

	if (m_fpRenderOverrideCallback)
		m_fpRenderOverrideCallback(this);
	else
	{
		XLISTFOREACH(t_RenderableList, ppRenderable, m_lpRenderables)
		{
			(*ppRenderable)->OnRender();
		}
	}
}

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
	m_fpRenderOverrideCallback = NULL;
}

// =============================================================================
// Nat Ryall                                                         30-Oct-2008
// =============================================================================
void CRenderManager::OnDeinitialise()
{
	ClearLayers();
}

// =============================================================================
// Nat Ryall                                                          3-Nov-2008
// =============================================================================
void CRenderManager::SetRenderOverride(t_RenderOverrideCallback fpCallback)
{
	m_fpRenderOverrideCallback = fpCallback;
}

// =============================================================================
// Nat Ryall                                                         30-Oct-2008
// =============================================================================
void CRenderManager::ReinitLayers(xint iLayerCount)
{
	XMASSERT(iLayerCount >= -1 && GetLayerCount() > -1, "Layer count must be zero or a positive value or -1 for the current number of layers.");

	// Use -1 for the current number of layers.
	if (iLayerCount == -1)
		iLayerCount = GetLayerCount();

	// Clear the existing layers.
	XLISTFOREACH(t_RenderLayerList, ppLayer, m_lpLayerList)
	{
		delete *ppLayer;
	}

	m_lpLayerList.clear();

	// Create new layers.
	for (xint iA = 0; iA < iLayerCount; ++iA)
	{
		m_lpLayerList.push_back(new CRenderLayer(iA));
	}
}

// =============================================================================
// Nat Ryall                                                          3-Nov-2008
// =============================================================================
void CRenderManager::ClearLayers()
{
	ReinitLayers(0);
}

// =============================================================================
// Nat Ryall                                                         30-Oct-2008
// =============================================================================
xint CRenderManager::GetLayerCount()
{
	return m_lpLayerList.size();
}

// =============================================================================
// Nat Ryall                                                         30-Oct-2008
// =============================================================================
CRenderLayer* CRenderManager::GetLayer(xint iIndex)
{
	XMASSERT(iIndex < GetLayerCount(), XFORMAT("Layer index %d is out of bounds.", iIndex));
	return m_lpLayerList[iIndex];
}

// =============================================================================
// Nat Ryall                                                         28-Jul-2008
// =============================================================================
void CRenderManager::Render()
{
	if (m_fpRenderOverrideCallback)
		m_fpRenderOverrideCallback();
	else
	{
		XLISTFOREACH(t_RenderLayerList, ppLayer, m_lpLayerList)
		{
			CRenderLayer* pLayer = *ppLayer;

			if (pLayer->IsEnabled())
				pLayer->Render();
		}

		_HGE->Gfx_SetTransform(0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f);
	}
}

// =============================================================================
// Nat Ryall                                                         19-Sep-2008
// =============================================================================
void CRenderManager::RenderBox(xbool bFilled, xrect xRect, xuint iColour)
{
	if (bFilled)
	{
		hgeQuad xQuad;
		memset(&xQuad, 0, sizeof(hgeQuad));

		xQuad.blend = BLEND_DEFAULT;

		xQuad.v[0].x = (xfloat)xRect.m_tLeft;
		xQuad.v[0].y = (xfloat)xRect.m_tTop;
		xQuad.v[1].x = (xfloat)xRect.m_tRight;
		xQuad.v[1].y = (xfloat)xRect.m_tTop;
		xQuad.v[3].x = (xfloat)xRect.m_tLeft;
		xQuad.v[3].y = (xfloat)xRect.m_tBottom;
		xQuad.v[2].x = (xfloat)xRect.m_tRight;
		xQuad.v[2].y = (xfloat)xRect.m_tBottom;

		xQuad.v[0].col = xQuad.v[1].col = xQuad.v[2].col = xQuad.v[3].col = iColour;

		_HGE->Gfx_RenderQuad(&xQuad);
	}
	else
	{
		_HGE->Gfx_RenderLine((xfloat)xRect.m_tLeft, (xfloat)xRect.m_tTop, (xfloat)xRect.m_tRight, (xfloat)xRect.m_tTop, iColour);
		_HGE->Gfx_RenderLine((xfloat)xRect.m_tRight, (xfloat)xRect.m_tTop, (xfloat)xRect.m_tRight, (xfloat)xRect.m_tBottom, iColour);
		_HGE->Gfx_RenderLine((xfloat)xRect.m_tRight, (xfloat)xRect.m_tBottom, (xfloat)xRect.m_tLeft, (xfloat)xRect.m_tBottom, iColour);
		_HGE->Gfx_RenderLine((xfloat)xRect.m_tLeft, (xfloat)xRect.m_tBottom, (xfloat)xRect.m_tLeft, (xfloat)xRect.m_tTop, iColour);
	}
}

//##############################################################################
