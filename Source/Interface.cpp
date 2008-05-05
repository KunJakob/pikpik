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
  m_pContainer(NULL),
  m_pActiveElement(NULL),
  m_pFocusedElement(NULL),
  m_bFoundActive(false),
  m_bDebugRender(false)
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

  for (XUINT iA = 0; iA < ElementType_Max; ++iA)
  {
    if (m_pCursor[iA])
      delete m_pCursor[iA];
  }
}

// =============================================================================
// Nat Ryall                                                          2-May-2008
// =============================================================================
void CInterfaceManager::Reset()
{
	m_pActiveElement = NULL;
	m_pFocusedElement = NULL;

	m_pContainer->DetachAll();

  for (XUINT iA = 0; iA < ElementType_Max; ++iA)
    m_pCursor[iA] = NULL;
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
		m_pActiveElement->OnMouseMove(m_xMousePos - m_xLastMousePos);

  m_bFoundActive = false;

  UpdateElement(m_pContainer);

	if (m_pFocusedElement)
	{
		CInterfaceElement* pElement = m_pFocusedElement;

		do 
			pElement->ToFront();
		while (pElement = pElement->m_pParent);

    // Check key presses.
    if (XINT iChar = _HGE->Input_GetChar())
      m_pFocusedElement->OnKeyChar(iChar);

    if (XINT iKey = _HGE->Input_GetKey())
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
  RenderElement(m_pContainer);

	if (m_bDebugRender)
  {
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

  if (_HGE->Input_IsMouseOver() && m_pCursor[ElementType_Unknown])
  {
    CBasicSprite* pCursor = m_pCursor[ElementType_Unknown];

    if (m_pActiveElement && m_pCursor[m_pActiveElement->GetType()])
      pCursor = m_pCursor[m_pActiveElement->GetType()];

    pCursor->Render(pCursor->GetImageRect(), XPOINT(), m_xMousePos, 1.f, 0.f);
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
  if (pElement->IsVisible())
  {
    pElement->Render();

    XEN_LIST_FOREACH(t_ElementList, ppElement, pElement->m_lpChildElements)
      RenderElement(*ppElement);
  }
}

// =============================================================================
// Nat Ryall                                                          1-May-2008
// =============================================================================
void CInterfaceManager::CheckIntersection(CInterfaceElement* pElement)
{
	// Iterate through all children in reverse-render order.
  XEN_LIST_FOREACH_R(t_ElementList, ppElement, pElement->m_lpChildElements)
		CheckIntersection(*ppElement);

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
CWindow::CWindow(CSpriteMetadata* pMetadata) : CInterfaceElement(ElementType_Window),
  m_pSprite(NULL),
	m_bDragging(false),
  m_bMoveable(true)
{
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
	XRECT xOffset
	(
		0, 0, GetWidth() - m_xFrameSize.iRight, GetHeight() - m_xFrameSize.iBottom
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
// Nat Ryall                                                          3-May-2008
// =============================================================================
void CWindow::SetSize(XUINT iWidth, XUINT iHeight)
{
  XUINT iFrameWidth = m_xFrameSize.iLeft + m_xFrameSize.iRight;
  XUINT iFrameHeight = m_xFrameSize.iTop + m_xFrameSize.iBottom;

  m_iWidth = (iWidth < iFrameWidth) ? 0 : iWidth - iFrameWidth;
  m_iHeight = (iHeight < iFrameHeight) ? 0 : iHeight - iFrameHeight;
}

// =============================================================================
// Nat Ryall                                                         30-Apr-2008
// =============================================================================
void CWindow::InternalRender(XRECT& xRect, XPOINT xOffset)
{
  m_pSprite->Render(xRect, GetPosition() + xOffset);
}

//##############################################################################

//##############################################################################
//
//                                   LABEL
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                          5-May-2008
// =============================================================================
CLabel::CLabel(CFontMetadata* pFont) : CInterfaceElement(ElementType_Label)
{
  m_pFont = new CFont(pFont);
}

// =============================================================================
// Nat Ryall                                                          5-May-2008
// =============================================================================
CLabel::~CLabel()
{
  delete m_pFont;
}

// =============================================================================
// Nat Ryall                                                          5-May-2008
// =============================================================================
void CLabel::Render()
{
  m_pFont->Render(m_xText.c_str(), GetPosition(), HGETEXT_LEFT);
}

//##############################################################################

//##############################################################################
//
//                                   BUTTON
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         30-Apr-2008
// =============================================================================
CButton::CButton(CSpriteMetadata* pSprite, CFontMetadata* pFont) : CInterfaceElement(ElementType_Button),
  m_pSprite(NULL),
  m_pFont(NULL),
  m_iWidth(0),
  m_iButtonState(AreaIndex_NormalMiddle),
	m_fpOnClickCallback(NULL)
{
  m_pSprite = new CBasicSprite(pSprite);

  if (pFont)
    m_pFont = new CFont(pFont);

  m_pAreas[AreaIndex_NormalLeft]    = pSprite->FindArea("NormalLeft"); 
  m_pAreas[AreaIndex_NormalMiddle]  = pSprite->FindArea("NormalMiddle"); 
  m_pAreas[AreaIndex_NormalRight]   = pSprite->FindArea("NormalRight"); 
  m_pAreas[AreaIndex_OverLeft]      = pSprite->FindArea("OverLeft"); 
  m_pAreas[AreaIndex_OverMiddle]    = pSprite->FindArea("OverMiddle"); 
  m_pAreas[AreaIndex_OverRight]     = pSprite->FindArea("OverRight"); 
  m_pAreas[AreaIndex_DownLeft]      = pSprite->FindArea("DownLeft"); 
  m_pAreas[AreaIndex_DownMiddle]    = pSprite->FindArea("DownMiddle"); 
  m_pAreas[AreaIndex_DownRight]     = pSprite->FindArea("DownRight"); 
}

// =============================================================================
// Nat Ryall                                                         30-Apr-2008
// =============================================================================
CButton::~CButton()
{
  if (m_pFont)
    delete m_pFont;

  delete m_pSprite;
}

// =============================================================================
// Nat Ryall                                                         30-Apr-2008
// =============================================================================
void CButton::Render()
{
  XUINT iLeftWidth = m_pAreas[m_iButtonState - 1]->xRect.GetWidth();
  XUINT iMiddleWidth = m_pAreas[m_iButtonState]->xRect.GetWidth();

  // Ends.
  InternalRender(m_pAreas[m_iButtonState - 1]->xRect, XPOINT(0, 0));
  InternalRender(m_pAreas[m_iButtonState + 1]->xRect, XPOINT(iLeftWidth + m_iWidth, 0));

  // Middle.
  for (XUINT iX = 0; iX < m_iWidth;)
  {
    XUINT iDrawWidth = Math::Clamp<XUINT>(m_iWidth - iX, 0, iMiddleWidth);

    XRECT xRect = m_pAreas[m_iButtonState]->xRect;
    xRect.iRight = xRect.iLeft + iDrawWidth;

    InternalRender(xRect, XPOINT(iX + iLeftWidth, 0));

    iX += iDrawWidth;
  }

  // Label.
  if (m_pFont)
    m_pFont->Render(m_xLabel.c_str(), GetPosition() + XPOINT(GetWidth() / 2, (GetHeight() - m_pFont->GetFontHeight()) / 2), HGETEXT_CENTER);
}

// =============================================================================
// Nat Ryall                                                          4-May-2008
// =============================================================================
void CButton::SetWidth(XUINT iWidth)
{
  XUINT iFrameWidth = m_pAreas[m_iButtonState - 1]->xRect.GetWidth() + m_pAreas[m_iButtonState + 1]->xRect.GetWidth();

  m_iWidth = (iWidth < iFrameWidth) ? 0 : iWidth - iFrameWidth;
}

// =============================================================================
// Nat Ryall                                                          4-May-2008
// =============================================================================
void CButton::InternalRender(XRECT& xRect, XPOINT xOffset)
{
  m_pSprite->Render(xRect, GetPosition() + xOffset);
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
CInputBox::CInputBox(CSpriteMetadata* pSprite, CFontMetadata* pFont) : CInterfaceElement(ElementType_InputBox),
	m_pSprite(NULL),
  m_pFont(NULL),
  m_iWidth(0),
  m_iCharLimit(0),
  m_iCharOffset(0),
  m_iFlashTimer(0)
{
  m_pSprite = new CBasicSprite(pSprite);
  m_pFont = new CFont(pFont);

  m_pAreas[AreaIndex_Left]    = pSprite->FindArea("Left"); 
  m_pAreas[AreaIndex_Middle]  = pSprite->FindArea("Middle"); 
  m_pAreas[AreaIndex_Right]   = pSprite->FindArea("Right"); 
}

// =============================================================================
// Nat Ryall                                                          2-May-2008
// =============================================================================
CInputBox::~CInputBox()
{
  delete m_pFont;
  delete m_pSprite;
}

// =============================================================================
// Nat Ryall                                                          3-May-2008
// =============================================================================
void CInputBox::Update()
{
  m_iFlashTimer = (m_iFlashTimer + _TIMEDELTA) % 1000;
}

// =============================================================================
// Nat Ryall                                                          2-May-2008
// =============================================================================
void CInputBox::Render()
{
  XUINT iLeftWidth = m_pAreas[AreaIndex_Left]->xRect.GetWidth();
  XUINT iMiddleWidth = m_pAreas[AreaIndex_Middle]->xRect.GetWidth();

  // Ends.
  InternalRender(m_pAreas[AreaIndex_Left]->xRect, XPOINT(0, 0));
  InternalRender(m_pAreas[AreaIndex_Right]->xRect, XPOINT(iLeftWidth + m_iWidth, 0));

  // Middle.
  for (XUINT iX = 0; iX < m_iWidth;)
  {
    XUINT iDrawWidth = Math::Clamp<XUINT>(m_iWidth - iX, 0, iMiddleWidth);

    XRECT xRect = m_pAreas[AreaIndex_Middle]->xRect;
    xRect.iRight = xRect.iLeft + iDrawWidth;

    InternalRender(xRect, XPOINT(iX + iLeftWidth, 0));

    iX += iDrawWidth;
  }

  XUINT iFontHeight = m_pFont->GetFontHeight();
  XUINT iOffsetY = (GetHeight() - iFontHeight) / 2;

  // Input cursor.
  if (InterfaceManager.IsFocusedElement(this) && m_iFlashTimer < 500)
  {
    XSTRING xString = m_xText.substr(0, m_iCharOffset);
    XUINT iOffsetX = iLeftWidth + 1 + m_pFont->GetStringWidth(xString.c_str());

    XRECT iLinePoints(iOffsetX, iOffsetY, iOffsetX, GetHeight() - iOffsetY);
    iLinePoints += GetPosition();

    _HGE->Gfx_RenderLine((float)iLinePoints.iLeft, (float)iLinePoints.iTop, (float)iLinePoints.iRight, (float)iLinePoints.iBottom, 0xFF000000); 
  }

  m_pFont->Render(GetText(), GetPosition() + XPOINT(iLeftWidth + 1, iOffsetY), HGETEXT_LEFT);
}

// =============================================================================
// Nat Ryall                                                          3-May-2008
// =============================================================================
void CInputBox::SetWidth(XUINT iWidth)
{
  XUINT iFrameWidth = m_pAreas[AreaIndex_Left]->xRect.GetWidth() + m_pAreas[AreaIndex_Right]->xRect.GetWidth();

  m_iWidth = (iWidth < iFrameWidth) ? 0 : iWidth - iFrameWidth;
}

// =============================================================================
// Nat Ryall                                                          3-May-2008
// =============================================================================
void CInputBox::InternalRender(XRECT& xRect, XPOINT xOffset)
{
  m_pSprite->Render(xRect, GetPosition() + xOffset);
}

// =============================================================================
// Nat Ryall                                                          4-May-2008
// =============================================================================
void CInputBox::OnMouseDown(XPOINT xPosition)
{
  if (m_xText.length())
  {
    XSTRING xCheckString;

    XUINT iTarget = xPosition.iX - GetPosition().iX;
    XUINT iWidth = 0;
    XUINT iLastWidth = 0;

    for (XUINT iA = 0; iA < m_xText.length(); ++iA)
    {
      xCheckString += m_xText[iA];
      iWidth = m_pFont->GetStringWidth(xCheckString.c_str()) + m_pAreas[AreaIndex_Left]->xRect.GetWidth();

      if (iWidth > iTarget)
        break;

      iLastWidth = iWidth;
    }

    if (iWidth > iTarget && iTarget - iLastWidth < iWidth - iTarget)
      m_iCharOffset = xCheckString.length() - 1;
    else
      m_iCharOffset = xCheckString.length();
  }
}

// =============================================================================
// Nat Ryall                                                          4-May-2008
// =============================================================================
void CInputBox::OnKeyChar(XINT iChar)
{
  if (iChar >= ' ' && iChar <= '~' && (m_iCharLimit == 0 || m_xText.length() < m_iCharLimit))
    m_xText.insert(m_iCharOffset++, 1, iChar);
}

// =============================================================================
// Nat Ryall                                                          4-May-2008
// =============================================================================
void CInputBox::OnKeyDown(XUINT iVirtualKey)
{
  switch (iVirtualKey)
  {
  case HGEK_BACKSPACE:
    {
      if (m_iCharOffset)
        m_xText.erase(--m_iCharOffset, 1);
    }
    break;

  case HGEK_LEFT:
    {
      if (m_iCharOffset)
        m_iCharOffset--;
    }
    break;

  case HGEK_RIGHT:
    {
      if (m_iCharOffset < m_xText.length())
        m_iCharOffset++;
    }
    break;
  }

  m_iFlashTimer = 0;
}

//##############################################################################