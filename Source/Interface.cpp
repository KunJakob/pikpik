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
//                                   STATIC
//
//##############################################################################

// The currently focused element.
CInterfaceElement* CInterfaceElement::s_pFocusedElement = NULL;

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
  m_pContainer(NULL),
  m_pCurrentElement(NULL)
{
  m_pContainer = new CContainer();
}

// =============================================================================
// Nat Ryall                                                          1-May-2008
// =============================================================================
CInterfaceManager::~CInterfaceManager()
{
  delete m_pContainer;
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

  UpdateElement(m_pContainer);
}

// =============================================================================
// Nat Ryall                                                          1-May-2008
// =============================================================================
void CInterfaceManager::Render()
{
  RenderElement(m_pContainer);
}

// =============================================================================
// Nat Ryall                                                          1-May-2008
// =============================================================================
void CInterfaceManager::UpdateElement(CInterfaceElement* pElement)
{
  CheckIntersection(pElement);

  pElement->Update();

  for (XUINT iA = 0; iA < pElement->GetAttachedCount(); ++iA)
    UpdateElement(pElement->GetAttached(iA));
}

// =============================================================================
// Nat Ryall                                                          1-May-2008
// =============================================================================
void CInterfaceManager::RenderElement(CInterfaceElement* pElement)
{
  pElement->Render();

  XEN_LIST_FOREACH(t_ElementList, ppElement, pElement->m_lpChildElements)
  {
    RenderElement(*ppElement);
  }
}

// =============================================================================
// Nat Ryall                                                          1-May-2008
// =============================================================================
bool CInterfaceManager::CheckIntersection(CInterfaceElement* pElement)
{
  XEN_LIST_FOREACH_R(t_ElementList, ppElement, pElement->m_lpChildElements)
  {
    if (CheckIntersection(*ppElement))
      return true;
  }

  if (pElement->IsVisible())
  {
    if (Math::Intersect(m_xMousePos, pElement->m_xArea))
    {
      if (!Math::Intersect(m_xLastMousePos, pElement->m_xArea))
        pElement->OnMouseEnter();

      if (_HGE->Input_KeyDown(HGEK_LBUTTON))
        pElement->OnMouseDown();
      else if (_HGE->Input_KeyUp(HGEK_LBUTTON))
        pElement->OnMouseUp();
    }
    else
    {
      if (Math::Intersect(m_xLastMousePos, pElement->m_xArea))
      {
        pElement->OnMouseLeave();
      }
    }
  }

  return false;
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
  XEN_LIST_ERASE(t_ElementList, m_lpChildElements, pElement);
}

// =============================================================================
// Nat Ryall                                                         30-Apr-2008
// =============================================================================
void CInterfaceElement::SetFocus(CInterfaceElement* pElement)
{
  if (s_pFocusedElement != pElement)
  {
    if (s_pFocusedElement)
      s_pFocusedElement->OnBlur();

    s_pFocusedElement = pElement;
    s_pFocusedElement->OnFocus();
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
  m_bMoveable(false)
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
  XRECT xOffset
  (
    m_pAreas[AreaIndex_MiddleLeft]->xRect.GetWidth() * -1,
    m_pAreas[AreaIndex_TopMiddle]->xRect.GetHeight() * -1,
    m_pAreas[AreaIndex_MiddleRight]->xRect.GetWidth(),
    m_pAreas[AreaIndex_BottomMiddle]->xRect.GetHeight()
  );

  // Draw corners.
  InternalRender(m_pAreas[AreaIndex_TopLeft]->xRect, XPOINT(xOffset.iLeft, xOffset.iTop));
  InternalRender(m_pAreas[AreaIndex_TopRight]->xRect, XPOINT(m_iWidth, xOffset.iTop));
  InternalRender(m_pAreas[AreaIndex_BottomLeft]->xRect, XPOINT(xOffset.iLeft, m_iHeight));
  InternalRender(m_pAreas[AreaIndex_BottomRight]->xRect, XPOINT(m_iWidth, m_iHeight));

  XUINT iCentreWidth = m_pAreas[AreaIndex_Middle]->xRect.GetWidth();
  XUINT iCentreHeight = m_pAreas[AreaIndex_Middle]->xRect.GetHeight();

  // Draw horizontal.
  for (XUINT iX = 0; iX < m_iWidth;)
  {
    XUINT iDrawWidth = Math::Clamp<XUINT>(m_iWidth - iX, 0, iCentreWidth);

    XRECT xTopRect = m_pAreas[AreaIndex_TopMiddle]->xRect;
    xTopRect.iRight = xTopRect.iLeft + iDrawWidth;

    InternalRender(xTopRect, XPOINT(iX, xOffset.iTop));

    XRECT xBottomRect = m_pAreas[AreaIndex_BottomMiddle]->xRect;
    xBottomRect.iRight = xBottomRect.iLeft + iDrawWidth;

    InternalRender(xBottomRect, XPOINT(iX, m_iHeight));

    iX += iDrawWidth;
  }

  // Draw vertical.
  for (XUINT iY = 0; iY < m_iHeight;)
  {
    XUINT iDrawHeight = Math::Clamp<XUINT>(m_iHeight - iY, 0, iCentreHeight);

    XRECT xLeftRect = m_pAreas[AreaIndex_MiddleLeft]->xRect;
    xLeftRect.iBottom = xLeftRect.iTop + iDrawHeight;

    InternalRender(xLeftRect, XPOINT(xOffset.iLeft, iY));

    XRECT xRightRect = m_pAreas[AreaIndex_MiddleLeft]->xRect;
    xRightRect.iBottom = xRightRect.iTop + iDrawHeight;

    InternalRender(xRightRect, XPOINT(m_iWidth, iY));

    iY += iDrawHeight;
  }

  // Draw fill.
  for (XUINT iY = 0; iY < m_iHeight;)
  {
    XUINT iDrawHeight = Math::Clamp<XUINT>(m_iHeight - iY, 0, iCentreHeight);

    for (XUINT iX = 0; iX < m_iWidth;)
    {
      XUINT iDrawWidth = Math::Clamp<XUINT>(m_iWidth - iX, 0, iCentreWidth);
      XRECT xRect = m_pAreas[AreaIndex_Middle]->xRect;

      xRect.iRight = xRect.iLeft + iDrawWidth;
      xRect.iBottom = xRect.iTop + iDrawHeight;

      InternalRender(xRect, XPOINT(iX, iY));

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
  m_pSprite->Render(xRect, XPOINT(), m_xPosition + xOffset, 1.f, 0.f);
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
  m_pLabel(NULL)
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
  m_pSprite->Render(m_pAreas[m_iButtonState]->xRect, XPOINT(), m_xPosition, 1.f, 0.f);
}

//##############################################################################