//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Global.
#include <Global.h>

// Local.
#include <Interface.h>

//##############################################################################

//##############################################################################
//
//                             INTERFACE MANAGER
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                          1-May-2008
// =============================================================================
CInterface::CInterface() :
	m_pScreen(NULL),
	m_bCursorVisible(true),
	m_pActiveElement(NULL),
	m_pFocusedElement(NULL),
	m_bFoundActive(false),
	m_bDebugRender(false)
{
	m_pScreen = new CScreenElement();
	Reset();
}

// =============================================================================
// Nat Ryall                                                          1-May-2008
// =============================================================================
CInterface::~CInterface()
{
	delete m_pScreen;

	for (xint iA = 0; iA < ElementType_Max; ++iA)
	{
		if (m_pCursor[iA])
			delete m_pCursor[iA];
	}
}

// =============================================================================
// Nat Ryall                                                          2-May-2008
// =============================================================================
void CInterface::Reset()
{
	m_pActiveElement = NULL;
	m_pFocusedElement = NULL;

	m_pScreen->DetachAll();

	for (xint iA = 0; iA < ElementType_Max; ++iA)
		m_pCursor[iA] = NULL;
}

// =============================================================================
// Nat Ryall                                                          1-May-2008
// =============================================================================
void CInterface::OnUpdate()
{
	m_xLastMousePos = m_xMousePos;

	xfloat fX;
	xfloat fY;

	_HGE->Input_GetMousePos(&fX, &fY);
	m_xMousePos = xpoint((xint)fX, (xint)fY);

	if (m_pActiveElement)
		m_pActiveElement->OnMouseMove(m_xMousePos - m_xLastMousePos, IsMouseDown());

	m_bFoundActive = false;

	UpdateElement(m_pScreen);

	if (m_pFocusedElement)
	{
		CInterfaceElement* pElement = m_pFocusedElement;

		do 
		{
			pElement->ToFront();
		}
		while (pElement = pElement->m_pParent);
	}

	// Check key presses.
	xint iChar = _HGE->Input_GetChar();

	if (m_pFocusedElement && iChar)
	{
		if (iChar >= ' ' && iChar <= '~')
			m_pFocusedElement->OnKeyChar((xchar)iChar);
	}

	xint iKey = _HGE->Input_GetKey();

	if (m_pFocusedElement && iKey)
	{
		if (_HGE->Input_GetKeyState(iKey))
			m_pFocusedElement->OnKeyDown(iKey);
		else
			m_pFocusedElement->OnKeyUp(iKey);
	}
}

// =============================================================================
// Nat Ryall                                                          1-May-2008
// =============================================================================
void CInterface::OnRender()
{
	RenderElement(m_pScreen);

	// Render debug boxes over the interface to show the active and focused elements.
	if (m_bDebugRender && m_pScreen->IsVisible())
	{
		if (m_pActiveElement)
		{
			xrect xRect = m_pActiveElement->GetArea();
			xuint iColour = ARGB(255, 32, 32, 32);

			RenderBox(xRect, iColour);
		}

		if (m_pFocusedElement)
		{
			xrect xRect = m_pFocusedElement->GetFocusArea() + xrect(2, 2, -1, -1);
			xuint iColour = ARGB(255, 255, 0, 0);

			RenderBoxBorder(xRect, iColour);
		}
	}

	// Render the cursor.
	if (m_bCursorVisible && _HGE->Input_IsMouseOver() && m_pCursor[ElementType_Unknown])
	{
		CBasicSprite* pCursor = m_pCursor[ElementType_Unknown];

		// If we have an active element and an custom cursor exists.
		if (m_pActiveElement && m_pCursor[m_pActiveElement->GetType()])
		{
			CInterfaceElement* pElement = m_pActiveElement;

			while (pElement->IsEnabled() && pElement != m_pScreen)
				pElement = pElement->GetParent();

			// Check that the there is nothing disabled and so blocking the custom cursor.
			if (pElement->IsEnabled())
				pCursor = m_pCursor[m_pActiveElement->GetType()];
		}

		pCursor->Render(m_xMousePos);
	}
}

// =============================================================================
// Nat Ryall                                                         20-Jul-2008
// =============================================================================
void CInterface::RenderBox(xrect xRect, xuint iColour)
{
	hgeQuad xQuad;
	memset(&xQuad, 0, sizeof(hgeQuad));

	xQuad.v[0].x = (float)xRect.iLeft;
	xQuad.v[0].y = (float)xRect.iTop;
	xQuad.v[1].x = (float)xRect.iRight;
	xQuad.v[1].y = (float)xRect.iTop;
	xQuad.v[3].x = (float)xRect.iLeft;
	xQuad.v[3].y = (float)xRect.iBottom;
	xQuad.v[2].x = (float)xRect.iRight;
	xQuad.v[2].y = (float)xRect.iBottom;

	xQuad.v[0].col = xQuad.v[1].col = xQuad.v[2].col = xQuad.v[3].col = iColour;

	_HGE->Gfx_RenderQuad(&xQuad);
}

// =============================================================================
// Nat Ryall                                                         06-Aug-2008
// =============================================================================
void CInterface::RenderBoxBorder(xrect xRect, xuint iColour)
{
	_HGE->Gfx_RenderLine((float)xRect.iLeft, (float)xRect.iTop, (float)xRect.iRight, (float)xRect.iTop, iColour);
	_HGE->Gfx_RenderLine((float)xRect.iRight, (float)xRect.iTop, (float)xRect.iRight, (float)xRect.iBottom, iColour);
	_HGE->Gfx_RenderLine((float)xRect.iRight, (float)xRect.iBottom, (float)xRect.iLeft, (float)xRect.iBottom, iColour);
	_HGE->Gfx_RenderLine((float)xRect.iLeft, (float)xRect.iBottom, (float)xRect.iLeft, (float)xRect.iTop, iColour);
}

// =============================================================================
// Nat Ryall                                                          3-May-2008
// =============================================================================
void CInterface::SetCursor(t_ElementType iType, CSpriteMetadata* pMetadata)
{
	if (m_pCursor[iType])
		delete m_pCursor[iType];

	m_pCursor[iType] = new CBasicSprite(pMetadata);
}

// =============================================================================
// Nat Ryall                                                          2-May-2008
// =============================================================================
void CInterface::SetFocus(CInterfaceElement* pElement)
{
	if (m_pFocusedElement != pElement)
	{
		if (m_pFocusedElement)
			m_pFocusedElement->OnBlur();

		m_pFocusedElement = pElement;
		m_pFocusedElement->OnFocus();
	}
}

// =============================================================================
// Nat Ryall                                                          2-May-2008
// =============================================================================
xbool CInterface::IsMouseOver(CInterfaceElement* pElement)
{
	return Math::Intersect(m_xMousePos, pElement->GetArea());
}

// =============================================================================
// Nat Ryall                                                          1-May-2008
// =============================================================================
void CInterface::UpdateElement(CInterfaceElement* pElement)
{
	if (pElement->IsEnabled())
	{
		// Iterate through all children in reverse-render order.
		XEN_LIST_FOREACH_R(t_ElementList, ppElement, pElement->m_lpChildElements)
			UpdateElement(*ppElement);

		if (pElement->IsVisible())
		{
			if (Math::Intersect(m_xMousePos, pElement->GetArea()))
			{
				// If we are the first element intersecting, we become the active element.
				if (!m_bFoundActive)
				{
					m_bFoundActive = true;

					if (m_pActiveElement && m_pActiveElement != pElement)
						m_pActiveElement->OnMouseLeave();

					m_pActiveElement = pElement;
					m_pActiveElement->OnMouseEnter();
				}

				// If we're the active element, check for mouse clicks.
				if (m_pActiveElement == pElement)
				{
					if (_HGE->Input_KeyDown(HGEK_LBUTTON))
					{
						SetFocus(pElement);
						pElement->OnMouseDown(m_xMousePos);
					}
					else if (_HGE->Input_KeyUp(HGEK_LBUTTON))
						pElement->OnMouseUp(m_xMousePos);
				}
			}
		}

		pElement->OnUpdate();
	}
}

// =============================================================================
// Nat Ryall                                                          1-May-2008
// =============================================================================
void CInterface::RenderElement(CInterfaceElement* pElement)
{
	if (pElement->IsVisible())
	{
		pElement->OnRender();

		XEN_LIST_FOREACH(t_ElementList, ppElement, pElement->m_lpChildElements)
			RenderElement(*ppElement);
	}
}

// =============================================================================
// Nat Ryall                                                         09-Jun-2008
// =============================================================================
void CInterface::DeregisterElement(CInterfaceElement* pElement)
{
	if (CInterfaceElement* pParent = pElement->m_pParent)
		pParent->Detach(pElement);

	XEN_LIST_REMOVE(t_ElementList, m_lpElements, pElement);
}

//##############################################################################

//##############################################################################
//
//                             INTERFACE ELEMENT
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         30-Apr-2008
// =============================================================================
CInterfaceElement::CInterfaceElement(t_ElementType iType) :
	m_pExtendedData(NULL),
	m_pParent(NULL),
	m_iType(iType),
	m_bVisible(true),
	m_bEnabled(true)
{
}

// =============================================================================
// Nat Ryall                                                         13-May-2008
// =============================================================================
CInterfaceElement::~CInterfaceElement()
{
}

// =============================================================================
// Nat Ryall                                                          2-May-2008
// =============================================================================
void CInterfaceElement::Move(xpoint xOffset)
{
	m_xPosition += xOffset;

	XEN_LIST_FOREACH(t_ElementList, ppElement, m_lpChildElements)
		(*ppElement)->Move(xOffset);
}

// =============================================================================
// Nat Ryall                                                         30-Apr-2008
// =============================================================================
void CInterfaceElement::Attach(CInterfaceElement* pElement)
{
	pElement->m_pParent = this;

	m_lpChildElements.push_back(pElement);

	Interface.RegisterElement(this);
}

// =============================================================================
// Nat Ryall                                                         30-Apr-2008
// =============================================================================
void CInterfaceElement::Detach(CInterfaceElement* pElement)
{
	pElement->m_pParent = NULL;

	XEN_LIST_REMOVE(t_ElementList, m_lpChildElements, pElement);

	if (pElement == Interface.m_pActiveElement)
		Interface.m_pActiveElement = NULL;

	if (pElement == Interface.m_pFocusedElement)
		Interface.m_pFocusedElement = NULL;

	Interface.DeregisterElement(this);
}

// =============================================================================
// Nat Ryall                                                         15-Jun-2008
// =============================================================================
void CInterfaceElement::DetachAll()
{
	while (m_lpChildElements.size())
		Detach(m_lpChildElements.front());
}

// =============================================================================
// Nat Ryall                                                         15-Jun-2008
// =============================================================================
xbool CInterfaceElement::IsAttached(CInterfaceElement* pElement)
{
	XEN_LIST_FOREACH(t_ElementList, ppElement, m_lpChildElements)
	{
		if (pElement == *ppElement)
			return true;
	}

	return false;
}

// =============================================================================
// Nat Ryall                                                          2-May-2008
// =============================================================================
void CInterfaceElement::ToFront()
{
	if (m_pParent)
	{
		XEN_LIST_REMOVE(t_ElementList, m_pParent->m_lpChildElements, this);
		m_pParent->m_lpChildElements.push_back(this);
	}
}

//##############################################################################