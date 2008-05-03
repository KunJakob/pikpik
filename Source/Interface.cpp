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
CInterfaceManager::CInterfaceManager() :
  m_pContainer(NULL)
{
  m_pContainer = new CContainer();
	m_pContainer->SetSize(_SWIDTH, _SHEIGHT);

	Reset();
}

// =============================================================================
// Nat Ryall                                                          1-May-2008
// =============================================================================
CInterfaceManager::~CInterfaceManager()
{
  delete m_pContainer;
}

// =============================================================================
// Nat Ryall                                                          2-May-2008
// =============================================================================
void CInterfaceManager::Reset()
{
	m_pActiveElement = NULL;
	m_pFocusedElement = NULL;

	m_pContainer->DetachAll();
}

// =============================================================================
// Nat Ryall                                                          1-May-2008
// =============================================================================
void CInterfaceManager::Update()
{
  m_xLastMousePos = m_xMousePos;

  XFLOAT fX;
  XFLOAT fY;

  _HGE->Input_GetMousePos(&fX, &fY);
  m_xMousePos = XPOINT((XINT)fX, (XINT)fY);

	if (m_pActiveElement)
	{
		m_pActiveElement->OnMouseMove(m_xMousePos - m_xLastMousePos);
		//m_pActiveElement = NULL;
	}

  m_bFoundActive = false;

  UpdateElement(m_pContainer);

	if (m_pFocusedElement)
	{
		CInterfaceElement* pElement = m_pFocusedElement;

		do 
			pElement->ToFront();
		while (pElement = pElement->m_pParent);
	}
}

// =============================================================================
// Nat Ryall                                                          1-May-2008
// =============================================================================
void CInterfaceManager::Render()
{
  RenderElement(m_pContainer);

	if (m_pActiveElement)
	{
		XRECT xRect = m_pActiveElement->GetArea();
		XUINT iColour = ARGB(255, 32, 32, 32);
		
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

	if (m_pFocusedElement)
	{
		XRECT xRect = m_pFocusedElement->GetFocusArea() + XRECT(2, 2, -1, -1);
		XUINT iColour = ARGB(255, 255, 0, 0);

		_HGE->Gfx_RenderLine((float)xRect.iLeft, (float)xRect.iTop, (float)xRect.iRight, (float)xRect.iTop, iColour);
		_HGE->Gfx_RenderLine((float)xRect.iRight, (float)xRect.iTop, (float)xRect.iRight, (float)xRect.iBottom, iColour);
		_HGE->Gfx_RenderLine((float)xRect.iRight, (float)xRect.iBottom, (float)xRect.iLeft, (float)xRect.iBottom, iColour);
		_HGE->Gfx_RenderLine((float)xRect.iLeft, (float)xRect.iBottom, (float)xRect.iLeft, (float)xRect.iTop, iColour);
	}
}

// =============================================================================
// Nat Ryall                                                          2-May-2008
// =============================================================================
void CInterfaceManager::SetFocus(CInterfaceElement* pElement)
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
XBOOL CInterfaceManager::IsMouseOver(CInterfaceElement* pElement)
{
	return Math::Intersect(m_xMousePos, pElement->GetArea());
}

// =============================================================================
// Nat Ryall                                                          1-May-2008
// =============================================================================
void CInterfaceManager::UpdateElement(CInterfaceElement* pElement)
{
  CheckIntersection(pElement);

  pElement->Update();

	XEN_LIST_FOREACH(t_ElementList, ppElement, pElement->m_lpChildElements)
		UpdateElement(*ppElement);
}

// =============================================================================
// Nat Ryall                                                          1-May-2008
// =============================================================================
void CInterfaceManager::RenderElement(CInterfaceElement* pElement)
{
  pElement->Render();

  XEN_LIST_FOREACH(t_ElementList, ppElement, pElement->m_lpChildElements)
    RenderElement(*ppElement);
}

// =============================================================================
// Nat Ryall                                                          1-May-2008
// =============================================================================
void CInterfaceManager::CheckIntersection(CInterfaceElement* pElement)
{
	// Iterate through all children in reverse-render order.
  XEN_LIST_FOREACH_R(t_ElementList, ppElement, pElement->m_lpChildElements)
		CheckIntersection(*ppElement);

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
  m_pParent(NULL),
  m_iType(iType),
  m_bVisible(true),
  m_bEnabled(true)
{
}

// =============================================================================
// Nat Ryall                                                          2-May-2008
// =============================================================================
void CInterfaceElement::Move(XPOINT xOffset)
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
}

// =============================================================================
// Nat Ryall                                                         30-Apr-2008
// =============================================================================
void CInterfaceElement::Detach(CInterfaceElement* pElement)
{
  XEN_LIST_REMOVE(t_ElementList, m_lpChildElements, pElement);
}

// =============================================================================
// Nat Ryall                                                          2-May-2008
// =============================================================================
void CInterfaceElement::ToFront()
{
	if (m_pParent)
	{
		m_pParent->Detach(this);
		m_pParent->Attach(this);
	}
}

//##############################################################################

//##############################################################################
//
//                                 CONTAINER
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         30-Apr-2008
// =============================================================================
CContainer::CContainer() : CInterfaceElement(ElementType_Container),
  m_iWidth(0),
  m_iHeight(0)
{
}

//##############################################################################

//##############################################################################
//
//                                   WINDOW
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         30-Apr-2008
// =============================================================================
CWindow::CWindow(CSpriteMetadata* pMetadata) :
  m_pSprite(NULL),
	m_bDragging(false),
  m_bMoveable(true)
{
  m_iType = ElementType_Dialog;
  m_pSprite = new CBasicSprite(pMetadata);

  m_pAreas[AreaIndex_TopLeft]       = pMetadata->FindArea("TopLeft"); 
  m_pAreas[AreaIndex_TopMiddle]     = pMetadata->FindArea("TopMiddle"); 
  m_pAreas[AreaIndex_TopRight]      = pMetadata->FindArea("TopRight"); 
  m_pAreas[AreaIndex_MiddleLeft]    = pMetadata->FindArea("MiddleLeft"); 
  m_pAreas[AreaIndex_Middle]        = pMetadata->FindArea("Middle"); 
  m_pAreas[AreaIndex_MiddleRight]   = pMetadata->FindArea("MiddleRight"); 
  m_pAreas[AreaIndex_BottomLeft]    = pMetadata->FindArea("BottomLeft"); 
  m_pAreas[AreaIndex_BottomMiddle]  = pMetadata->FindArea("BottomMiddle"); 
  m_pAreas[AreaIndex_BottomRight]   = pMetadata->FindArea("BottomRight");

	m_xFrameSize = XRECT
	(
		m_pAreas[AreaIndex_MiddleLeft]->xRect.GetWidth(),
		m_pAreas[AreaIndex_TopMiddle]->xRect.GetHeight(),
		m_pAreas[AreaIndex_MiddleRight]->xRect.GetWidth(),
		m_pAreas[AreaIndex_BottomMiddle]->xRect.GetHeight()
	);
}

// =============================================================================
// Nat Ryall                                                         30-Apr-2008
// =============================================================================
CWindow::~CWindow()
{
  delete m_pSprite;
}

// =============================================================================
// Nat Ryall                                                         30-Apr-2008
// =============================================================================
void CWindow::Render()
{
	XUINT iWindowWidth = GetWidth();
	XUINT iWindowHeight = GetHeight();

	XRECT xOffset
	(
		0, 0, iWindowWidth - m_xFrameSize.iRight, iWindowHeight - m_xFrameSize.iBottom
	);

  // Draw corners.
  InternalRender(m_pAreas[AreaIndex_TopLeft]->xRect, XPOINT(xOffset.iLeft, xOffset.iTop));
  InternalRender(m_pAreas[AreaIndex_TopRight]->xRect, XPOINT(xOffset.iRight, xOffset.iTop));
  InternalRender(m_pAreas[AreaIndex_BottomLeft]->xRect, XPOINT(xOffset.iLeft, xOffset.iBottom));
  InternalRender(m_pAreas[AreaIndex_BottomRight]->xRect, XPOINT(xOffset.iRight, xOffset.iBottom));

  XUINT iCentreWidth = m_pAreas[AreaIndex_Middle]->xRect.GetWidth();
  XUINT iCentreHeight = m_pAreas[AreaIndex_Middle]->xRect.GetHeight();

  // Draw vertical.
  for (XUINT iY = 0; iY < m_iHeight;)
  {
    XUINT iDrawHeight = Math::Clamp<XUINT>(m_iHeight - iY, 0, iCentreHeight);

		// Left.
    XRECT xLeftRect = m_pAreas[AreaIndex_MiddleLeft]->xRect;
    xLeftRect.iBottom = xLeftRect.iTop + iDrawHeight;

    InternalRender(xLeftRect, XPOINT(xOffset.iLeft, iY + xOffset.iTop + m_xFrameSize.iTop));

		// Right.
    XRECT xRightRect = m_pAreas[AreaIndex_MiddleLeft]->xRect;
    xRightRect.iBottom = xRightRect.iTop + iDrawHeight;

    InternalRender(xRightRect, XPOINT(xOffset.iRight, iY + xOffset.iTop + m_xFrameSize.iTop));

		// Draw horizontal.
		for (XUINT iX = 0; iX < m_iWidth;)
		{
			XUINT iDrawWidth = Math::Clamp<XUINT>(m_iWidth - iX, 0, iCentreWidth);

			// Top.
			XRECT xTopRect = m_pAreas[AreaIndex_TopMiddle]->xRect;
			xTopRect.iRight = xTopRect.iLeft + iDrawWidth;

			InternalRender(xTopRect, XPOINT(iX + xOffset.iLeft + m_xFrameSize.iLeft, xOffset.iTop));

			// Bottom.
			XRECT xBottomRect = m_pAreas[AreaIndex_BottomMiddle]->xRect;
			xBottomRect.iRight = xBottomRect.iLeft + iDrawWidth;

			InternalRender(xBottomRect, XPOINT(iX + xOffset.iLeft + m_xFrameSize.iLeft, xOffset.iBottom));

			// Fill.
			XRECT xRect = m_pAreas[AreaIndex_Middle]->xRect;
			xRect.iRight = xRect.iLeft + iDrawWidth;
			xRect.iBottom = xRect.iTop + iDrawHeight;

			InternalRender(xRect, XPOINT(iX + xOffset.iLeft + m_xFrameSize.iLeft, iY + xOffset.iTop + m_xFrameSize.iTop));

			iX += iDrawWidth;
		}

    iY += iDrawHeight;
  }
}

// =============================================================================
// Nat Ryall                                                         30-Apr-2008
// =============================================================================
void CWindow::InternalRender(XRECT& xRect, XPOINT xOffset)
{
  m_pSprite->Render(xRect, XPOINT(), GetPosition() + xOffset, 1.f, 0.f);
}

//##############################################################################

//##############################################################################
//
//                                   LABEL
//
//##############################################################################


//##############################################################################

//##############################################################################
//
//                                   BUTTON
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         30-Apr-2008
// =============================================================================
CButton::CButton(CSpriteMetadata* pMetadata) : CInterfaceElement(ElementType_Button),
  m_pSprite(NULL),
  m_iButtonState(AreaIndex_Normal),
  m_pLabel(NULL),
	m_fpOnClickCallback(NULL)
{
  m_pSprite = new CBasicSprite(pMetadata);

  m_pAreas[AreaIndex_Normal]  = pMetadata->FindArea("Normal"); 
  m_pAreas[AreaIndex_Over]    = pMetadata->FindArea("Over"); 
  m_pAreas[AreaIndex_Down]    = pMetadata->FindArea("Down"); 
}

// =============================================================================
// Nat Ryall                                                         30-Apr-2008
// =============================================================================
CButton::~CButton()
{
  delete m_pSprite;
}

// =============================================================================
// Nat Ryall                                                         30-Apr-2008
// =============================================================================
void CButton::Render()
{
  m_pSprite->Render(m_pAreas[m_iButtonState]->xRect, XPOINT(), GetPosition(), 1.f, 0.f);
}

//##############################################################################

//##############################################################################
//
//                                 INPUT BOX
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                          2-May-2008
// =============================================================================
CInputBox::CInputBox(CSpriteMetadata* pMetadata) : CInterfaceElement(ElementType_InputBox),
	m_pSprite(NULL)
{

}

// =============================================================================
// Nat Ryall                                                          2-May-2008
// =============================================================================
CInputBox::~CInputBox()
{

}

// =============================================================================
// Nat Ryall                                                          2-May-2008
// =============================================================================
void CInputBox::Render()
{

}

//##############################################################################