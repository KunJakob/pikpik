
//##############################################################################
//
//                                 CONTAINER
//
//##############################################################################
class CContainer : public CInterfaceElement
{
public:
  /**
  * 
  */
  CContainer();

  /**
  * 
  */
  virtual void Render() {}

  /**
  * 
  */
  void SetSize(XUINT iWidth, XUINT iHeight)
  {
    m_iWidth = iWidth;
    m_iHeight = iHeight;
  }

  /**
  * 
  */
  virtual XUINT GetWidth()
  {
    return m_iWidth;
  }

  /**
  * 
  */
  virtual XUINT GetHeight()
  {
    return m_iHeight;
  }

protected:
  // The width, in pixels, of the container.
  XUINT m_iWidth;

  // The height, in pixels, of the container.
  XUINT m_iHeight;
};

//##############################################################################

//##############################################################################
//
//                                   WINDOW
//
//##############################################################################
class CWindow : public CInterfaceElement
{
public:
  /**
  * 
  */
  CWindow(CSpriteMetadata* pMetadata);

  /**
  * 
  */
  virtual ~CWindow();

  /**
  * 
  */
  virtual void Render();

	/**
	* 
	*/
	virtual XUINT GetWidth()
	{
		return GetInnerWidth() + m_xFrameSize.iLeft + m_xFrameSize.iRight;
	}

	/**
	* 
	*/
	virtual XUINT GetHeight()
	{
		return GetInnerHeight() + m_xFrameSize.iTop + m_xFrameSize.iBottom;
	}

  /**
  * Set the size of the window including the border.
  */
  void SetSize(XUINT iWidth, XUINT iHeight);

  /**
  * Set the size of the inner-area of the window excluding the border.
  */
  void SetInnerSize(XUINT iWidth, XUINT iHeight)
  {
    m_iWidth = iWidth; 
    m_iHeight = iHeight;
  }

	/**
	* 
	*/
	XUINT GetInnerWidth()
	{
		return m_iWidth;
	}

	/**
	* 
	*/
	XUINT GetInnerHeight()
	{
		return m_iHeight;
	}

	/**
	* 
	*/
	XPOINT GetInnerPosition()
	{
		return GetPosition() + XPOINT(m_xFrameSize.iLeft, m_xFrameSize.iTop);
	}

	/**
	* 
	*/
	XRECT GetFrameSize()
	{
		return m_xFrameSize;
	}

	/**
	* 
	*/
	virtual XRECT GetFocusArea()
	{
		return XRECT(0, 0, GetInnerWidth(), GetInnerHeight()) + GetInnerPosition();
	}

  /**
  * 
  */
  void SetMoveable(XBOOL bMoveable)
  {
    m_bMoveable = bMoveable;
  }

  /**
  * 
  */
  XBOOL IsMoveable()
  {
    return m_bMoveable;
  }

protected:
  // Internal types.
  enum t_AreaIndex
  {
    AreaIndex_TopLeft,
    AreaIndex_TopMiddle,
    AreaIndex_TopRight,
    AreaIndex_MiddleLeft,
    AreaIndex_Middle,
    AreaIndex_MiddleRight,
    AreaIndex_BottomLeft,
    AreaIndex_BottomMiddle,
    AreaIndex_BottomRight,
    /*MAX*/AreaIndex_Max,
  };

  /**
  * 
  */
  void InternalRender(XRECT& xRect, XPOINT xOffset);

  /**
  * 
  */
  virtual void OnMouseMove(XPOINT xDifference) 
  {
    if (m_bDragging)
      Move(xDifference);
  }

  /**
  * 
  */
  virtual void OnMouseDown(XPOINT xPosition) 
  {
    if (m_bMoveable && Math::Intersect(xPosition, XRECT(m_xFrameSize.iLeft, 0, GetInnerWidth(), m_xFrameSize.iTop) + GetPosition()))
      m_bDragging = true;
  }

  /**
  * 
  */
  virtual void OnMouseUp(XPOINT xPosition) 
  {
    m_bDragging = false;
  }

  // The element sprite.
  CBasicSprite* m_pSprite;

  // The element area list.
  CSpriteMetadata::CArea* m_pAreas[AreaIndex_Max];

  // The width, in pixels, of the inner container.
  XUINT m_iWidth;

  // The height, in pixels, of the inner container.
  XUINT m_iHeight;

	// The frame size from all directions.
	XRECT m_xFrameSize;

	// Specifies if the window is being dragged.
	XBOOL m_bDragging;

  // Specifies if the window can be dragged around the screen or if it is static.
  XBOOL m_bMoveable;
};

//##############################################################################

//##############################################################################
//
//                                   LABEL
//
//##############################################################################
class CLabel : public CInterfaceElement
{
public:
  /**
  * 
  */
  CLabel(CFontMetadata* pFont);

  /**
  * 
  */
  virtual ~CLabel();

  /**
  * 
  */
  virtual void Render();

  /**
  * 
  */
  virtual XUINT GetWidth()
  {
    return m_pFont->GetStringWidth(m_xText.c_str());
  }

  /**
  * 
  */
  virtual XUINT GetHeight()
  {
    return m_pFont->GetFontHeight();
  }

  /**
  * 
  */
  void SetText(const XCHAR* pText) 
  {
    m_xText = pText;
  }

  /**
  * 
  */
  const XCHAR* GetText() 
  {
    return m_xText.c_str();
  }

protected:
  // The element font.
  CFont* m_pFont;

  // The text string.
  XSTRING m_xText;
};

//##############################################################################

//##############################################################################
//
//                                   BUTTON
//
//##############################################################################
class CButton : public CInterfaceElement
{
public:
	// Callbacks.
	typedef void (*t_OnClickCallback)(XPOINT /*Offset*/);

  /**
  * 
  */
  CButton(CSpriteMetadata* pSprite, CFontMetadata* pFont = NULL);

  /**
  * 
  */
  virtual ~CButton();

  /**
  * 
  */
  virtual void Render();

	/**
	* 
	*/
	virtual XUINT GetWidth()
	{
		return m_iWidth + m_pAreas[m_iButtonState - 1]->xRect.GetWidth() + m_pAreas[m_iButtonState + 1]->xRect.GetWidth();
	}

	/**
	* 
	*/
	virtual XUINT GetHeight()
	{
		return m_pAreas[m_iButtonState]->xRect.GetHeight();
	}

  /**
  * 
  */
  void SetWidth(XUINT iWidth);

  /**
  * 
  */
  void SetInnerWidth(XUINT iWidth)
  {
    m_iWidth = iWidth;
  }

  /**
  * 
  */
  XUINT GetInnerWidth()
  {
    return m_iWidth;
  }

  /**
  * 
  */
  void SetClickCallback(t_OnClickCallback fpCallback)
  {
    m_fpOnClickCallback = fpCallback;
  }

  /**
  * 
  */
  void SetText(const XCHAR* pLabel) 
  {
    m_xLabel = pLabel;
  }

  /**
  * 
  */
  const XCHAR* GetLabel() 
  {
    return m_xLabel.c_str();
  }

protected:
  // Internal types.
  enum t_AreaIndex
  {
    AreaIndex_NormalLeft,
    AreaIndex_NormalMiddle,
    AreaIndex_NormalRight,
    AreaIndex_OverLeft,
    AreaIndex_OverMiddle,
    AreaIndex_OverRight,
    AreaIndex_DownLeft,
    AreaIndex_DownMiddle,
    AreaIndex_DownRight,
    /*MAX*/AreaIndex_Max,
  };

  /**
  * 
  */
  void InternalRender(XRECT& xRect, XPOINT xOffset);

  /**
  * 
  */
  virtual void OnMouseEnter() 
  {
    m_iButtonState = InterfaceManager.IsMouseDown() ? AreaIndex_DownMiddle : AreaIndex_OverMiddle;
  }

  /**
  * 
  */
  virtual void OnMouseLeave()
  {
    m_iButtonState = AreaIndex_NormalMiddle;
  }

  /**
  * 
  */
  virtual void OnMouseDown(XPOINT xPosition)
  {
    m_iButtonState = AreaIndex_DownMiddle;
  }

  /**
  * 
  */
  virtual void OnMouseUp(XPOINT xPosition) 
  {
    m_iButtonState = AreaIndex_OverMiddle;

    if (m_fpOnClickCallback)
      m_fpOnClickCallback(InterfaceManager.GetMousePosition() - GetPosition());
  }

  // The element sprite.
  CBasicSprite* m_pSprite;

  // The element font.
  CFont* m_pFont;

  // The element area list.
  CSpriteMetadata::CArea* m_pAreas[AreaIndex_Max];

  // The width, in pixels, of the button area.
  XUINT m_iWidth;

  // The button state.
  t_AreaIndex m_iButtonState;

  // The button label string.
  XSTRING m_xLabel;

  // The callback to execute if the button is clicked.
  t_OnClickCallback m_fpOnClickCallback;
};

//##############################################################################

//##############################################################################
//
//                                 INPUT BOX
//
//##############################################################################
class CInputBox : public CInterfaceElement
{
public:
	/**
	* 
	*/
	CInputBox(CSpriteMetadata* pMetadata, CFontMetadata* pFont);

	/**
	* 
	*/
	virtual ~CInputBox();

  /**
  * 
  */
  virtual void Update();

	/**
	* 
	*/
	virtual void Render();

  /**
  * 
  */
  virtual XUINT GetWidth()
  {
    return GetInnerWidth() + m_pAreas[AreaIndex_Left]->xRect.GetWidth() + m_pAreas[AreaIndex_Right]->xRect.GetWidth();
  }

  /**
  * 
  */
  virtual XUINT GetHeight()
  {
    return m_pAreas[AreaIndex_Middle]->xRect.GetHeight();
  }

  /**
  * 
  */
  void SetWidth(XUINT iWidth);

  /**
  * 
  */
  void SetInnerWidth(XUINT iWidth)
  {
    m_iWidth = iWidth;
  }

  /**
  * 
  */
  XUINT GetInnerWidth()
  {
    return m_iWidth;
  }

  /**
  * The total number of characters allowed in the input box.
  * Set this to 0 for unlimited length.
  */
  void SetCharLimit(XUINT iLimit)
  {
    m_iCharLimit = iLimit;
  }

  /**
  * 
  */
  XUINT GetCharLimit()
  {
    return m_iCharLimit;
  }

  /**
  * 
  */
  void SetText(const XCHAR* pText)
  {
    m_xText = pText;
  }

  /**
  * 
  */
  const XCHAR* GetText()
  {
    return m_xText.c_str();
  }

protected:
  /**
  * 
  */
  void InternalRender(XRECT& xRect, XPOINT xOffset);

  /**
  * 
  */
  virtual void OnFocus()
  {
    m_iFlashTimer = 0;
  }

  /**
  * 
  */
  virtual void OnMouseDown(XPOINT xPosition);

  /**
  * 
  */
  virtual void OnKeyChar(XINT iChar);

  /**
  * 
  */
  virtual void OnKeyDown(XUINT iVirtualKey);

	// Internal types.
	enum t_AreaIndex
	{
		AreaIndex_Left,
		AreaIndex_Middle,
		AreaIndex_Right,
		/*MAX*/AreaIndex_Max,
	};

	// The element sprite.
	CBasicSprite* m_pSprite;

  // The element font.
  CFont* m_pFont;

	// The element area list.
	CSpriteMetadata::CArea* m_pAreas[AreaIndex_Max];

  // The width, in pixels, of the input area.
  XUINT m_iWidth;

  // The total number of input characters allowed with zero being unlimited.
  XUINT m_iCharLimit;

  // The current input-box text.
  XSTRING m_xText;

  // The input cursor text character offset.
  XUINT m_iCharOffset;

  // The cursor flash timer.
  XUINT m_iFlashTimer;
};

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