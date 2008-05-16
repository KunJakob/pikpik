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
  m_pScreen(NULL),
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
CInterfaceManager::~CInterfaceManager()
{
	delete m_pScreen;

  for (xint iA = 0; iA < ElementType_Max; ++iA)
  {
    if (m_pCursor[iA])
      delete m_pCursor[iA];
  }

	XEN_LIST_ERASE_ALL(m_lpElements);
}

// =============================================================================
// Nat Ryall                                                          2-May-2008
// =============================================================================
void CInterfaceManager::Reset()
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
void CInterfaceManager::Update()
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
			pElement->ToFront();
		while (pElement = pElement->m_pParent);

    // Check key presses.
    if (xint iChar = _HGE->Input_GetChar())
		{
      if (iChar >= ' ' && iChar <= '~')
				m_pFocusedElement->OnKeyChar((xchar)iChar);
		}

    if (xint iKey = _HGE->Input_GetKey())
    {
      if (_HGE->Input_GetKeyState(iKey))
        m_pFocusedElement->OnKeyDown(iKey);
      else
        m_pFocusedElement->OnKeyUp(iKey);
    }
	}
}

// =============================================================================
// Nat Ryall                                                          1-May-2008
// =============================================================================
void CInterfaceManager::Render()
{
  RenderElement(m_pScreen);

	if (m_bDebugRender)
  {
    if (m_pActiveElement)
    {
      xrect xRect = m_pActiveElement->GetArea();
      xuint iColour = ARGB(255, 32, 32, 32);

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
      xrect xRect = m_pFocusedElement->GetFocusArea() + xrect(2, 2, -1, -1);
      xuint iColour = ARGB(255, 255, 0, 0);

      _HGE->Gfx_RenderLine((float)xRect.iLeft, (float)xRect.iTop, (float)xRect.iRight, (float)xRect.iTop, iColour);
      _HGE->Gfx_RenderLine((float)xRect.iRight, (float)xRect.iTop, (float)xRect.iRight, (float)xRect.iBottom, iColour);
      _HGE->Gfx_RenderLine((float)xRect.iRight, (float)xRect.iBottom, (float)xRect.iLeft, (float)xRect.iBottom, iColour);
      _HGE->Gfx_RenderLine((float)xRect.iLeft, (float)xRect.iBottom, (float)xRect.iLeft, (float)xRect.iTop, iColour);
    }
  }

  if (_HGE->Input_IsMouseOver() && m_pCursor[ElementType_Unknown])
  {
    CBasicSprite* pCursor = m_pCursor[ElementType_Unknown];

    if (m_pActiveElement && m_pCursor[m_pActiveElement->GetType()])
      pCursor = m_pCursor[m_pActiveElement->GetType()];

    pCursor->Render(pCursor->GetImageRect(), m_xMousePos);
  }
}

// =============================================================================
// Nat Ryall                                                          3-May-2008
// =============================================================================
void CInterfaceManager::SetCursor(t_ElementType iType, CSpriteMetadata* pMetadata)
{
  if (m_pCursor[iType])
    delete m_pCursor[iType];

  m_pCursor[iType] = new CBasicSprite(pMetadata);
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
  // Iterate through all children in reverse-render order.
  XEN_LIST_FOREACH_R(t_ElementList, ppElement, pElement->m_lpChildElements)
    UpdateElement(*ppElement);

  if (pElement->IsVisible() && pElement->IsEnabled())
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

  pElement->Update();
}

// =============================================================================
// Nat Ryall                                                          1-May-2008
// =============================================================================
void CInterfaceManager::RenderElement(CInterfaceElement* pElement)
{
  if (pElement->IsVisible())
  {
    pElement->Render();

    XEN_LIST_FOREACH(t_ElementList, ppElement, pElement->m_lpChildElements)
      RenderElement(*ppElement);
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
	InterfaceManager.RegisterElement(this);
}

// =============================================================================
// Nat Ryall                                                         13-May-2008
// =============================================================================
CInterfaceElement::~CInterfaceElement()
{
	InterfaceManager.DeregisterElement(this);
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