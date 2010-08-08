//##############################################################################

// Global.
#include <Global.h>

// Local.
#include <Renderer.h>

//##############################################################################

// =============================================================================
CRenderTransformation::CRenderTransformation()
{
	Reset();
}

// =============================================================================
void CRenderTransformation::Reset()
{
	m_xPosition = xpoint();
	m_fRotation = 0.f;
	m_fHorizontalScale = 1.f;
	m_fVerticalScale = 1.f;
}

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

// =============================================================================
CRenderLayer::CRenderLayer(xuint iLayerIndex) :
	m_iLayerIndex(iLayerIndex),
	m_bEnabled(true),
	m_fpRenderOverrideCallback(NULL)
{
}

// =============================================================================
void CRenderLayer::ClearRenderables()
{
	m_lpRenderables.clear();
}

// =============================================================================
void CRenderLayer::AttachRenderable(CRenderable* pRenderable)
{
	m_lpRenderables.push_back(pRenderable);
}

// =============================================================================
void CRenderLayer::DetachRenderable(CRenderable* pRenderable)
{
	XLISTREMOVE(t_RenderableList, m_lpRenderables, pRenderable);
}

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
xint CRenderLayer::GetRenderableCount()
{
	return m_lpRenderables.size();
}

// =============================================================================
CRenderable* CRenderLayer::GetRenderable(xint iIndex)
{
	XMASSERT(iIndex > (GetRenderableCount() - 1), XFORMAT("Renderable index %d is out of bounds.", iIndex));
	return m_lpRenderables[iIndex];
}

// =============================================================================
void CRenderLayer::SetEnabled(xbool bEnabled)
{
	m_bEnabled = bEnabled;
}

// =============================================================================
xbool CRenderLayer::IsEnabled()
{
	return m_bEnabled;
}

// =============================================================================
void CRenderLayer::SetRenderOverride(t_RenderLayerOverrideCallback fpCallback)
{
	m_fpRenderOverrideCallback = fpCallback;
}

// =============================================================================
void CRenderLayer::SetTransformation(xpoint xPosition, xfloat fRotation, xfloat fHorizontalScale, xfloat fVerticalScale)
{
	m_xTransformation.m_xPosition = xPosition;
	m_xTransformation.m_fRotation = fRotation;
	m_xTransformation.m_fHorizontalScale = fHorizontalScale;
	m_xTransformation.m_fVerticalScale = fVerticalScale;
}

// =============================================================================
CRenderTransformation& CRenderLayer::GetTransformation()
{
	return m_xTransformation;
}

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

// =============================================================================
void CRenderManager::OnInitialise()
{
	m_fpRenderOverrideCallback = NULL;
}

// =============================================================================
void CRenderManager::OnDeinitialise()
{
	ClearLayers();
}

// =============================================================================
void CRenderManager::SetRenderOverride(t_RenderOverrideCallback fpCallback)
{
	m_fpRenderOverrideCallback = fpCallback;
}

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
void CRenderManager::ClearLayers()
{
	ReinitLayers(0);
}

// =============================================================================
xint CRenderManager::GetLayerCount()
{
	return m_lpLayerList.size();
}

// =============================================================================
CRenderLayer* CRenderManager::GetLayer(xint iIndex)
{
	XMASSERT(iIndex < GetLayerCount(), XFORMAT("Layer index %d is out of bounds.", iIndex));
	return m_lpLayerList[iIndex];
}

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
