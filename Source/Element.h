//##############################################################################
//
//                               CONTROL ELEMENT
//
//##############################################################################
class CControlElement : public CInterfaceElement
{
public:
	// Virtual destructor to ensure proper cleanup of all child classes.
	virtual ~CWindowElement();

	// Get the width including all borders.
	inline virtual xint GetWidth()
	{
		return GetInnerWidth() + GetBorderWidth();
	}

	// Get the height including all borders.
	inline virtual xint GetHeight() = 0;

	// Get the width excluding any borders.
	inline xint GetInnerWidth()
	{
		return m_iWidth;
	}

	// Get the border width including both the left and right borders.
	inline xint GetBorderWidth()
	{
		return m_xFrameSize.iLeft + m_xFrameSize.iRight;
	}

	// Get the width and height as a border inclusive size.
	inline xpoint GetSize()
	{
		return xpoint(GetWidth(), GetHeight());
	}

	// Set the width using a border inclusive size.
	inline void SetWidth(xint iWidth)
	{
		SetInnerWidth(iWidth - GetBorderWidth());
	}

	// Set the inner width using a border exclusive size.
	inline void SetInnerWidth(xint iWidth)
	{
		m_iWidth = Math::Clamp(iWidth, 0, abs(iWidth));
	}

protected:
	// Internal constructor to prevent instantiation of this class.
	CControlElement(t_ElementType iElementType) : CInterfaceElement(iElementType) {}

	// Render a control at the current position and current size using the specified rects for each tile.
	void Render(xrect& xL, xrect& xC, xrect& xR, xint iVertOffset = 0);

	// The element sprite.
	CBasicSprite* m_pSprite;

	// The frame size from all directions.
	xrect m_xFrameSize;

	// The width, in pixels, of the inner container.
	xint m_iWidth;
};

//##############################################################################

//##############################################################################
//
//                               WINDOW ELEMENT
//
//##############################################################################
class CWindowElement : public CControlElement
{
public:
	// Get the width including all borders.
	inline virtual xint GetWidth()
	{
		return GetInnerWidth() + GetBorderWidth();
	}

	// Get the height including all borders.
	inline virtual xint GetHeight()
	{
		return GetInnerHeight() + GetBorderHeight();
	}

	// Get the height excluding any borders.
	inline xint GetInnerHeight()
	{
		return m_iHeight;
	}

	// Get the border width including both the left and right borders.
	inline xint GetBorderWidth()
	{
		return m_xFrameSize.iLeft + m_xFrameSize.iRight;
	}

	// Get the border height including both the top and bottom borders.
	inline xint GetBorderHeight()
	{
		return m_xFrameSize.iTop + m_xFrameSize.iBottom;
	}

	// Get the inner width and height as a border exclusive size.
	inline xpoint GetInnerSize()
	{
		return xpoint(GetInnerWidth(), GetInnerHeight());
	}

	// Get the screen position of the inner-area.
	inline xpoint GetInnerPosition()
	{
		return GetPosition() + xpoint(m_xFrameSize.iLeft, m_xFrameSize.iTop);
	}

	// Set the height using a border inclusive size.
	inline void SetHeight(xint iHeight)
	{
		SetInnerHeight(iHeight - GetBorderHeight());
	}

	// Set the inner height using a border exclusive size.
	inline void SetInnerHeight(xint iHeight)
	{
		m_iHeight = Math::Clamp(iHeight, 0, abs(iHeight));
	}

	// Set the width and height using a border inclusive size.
	inline void SetSize(xint iWidth, xint iHeight)
	{
		SetWidth(iWidth);
		SetHeight(iHeight);
	}

	// Set the inner width and height using a border exclusive size.
	inline void SetInnerSize(xint iWidth, xint iHeight)
	{
		SetInnerWidth(iWidth);
		SetInnerHeight(iHeight);
	}

	// Get the screen rect for the inner, focus area.
	virtual xrect GetFocusArea()
	{
		return xrect(GetInnerPosition(), GetInnerPosition() + GetInnerSize());
	}

	// Get the size of all surrounding borders.
	inline xrect GetFrameSize()
	{
		return m_xFrameSize;
	}

	// Allow or prevent the window from being draggable.
	void SetMoveable(xbool bMoveable)
	{
		m_bMoveable = bMoveable;
	}

	// Determine if the window is draggable.
	xbool IsMoveable()
	{
		return m_bMoveable;
	}

protected:
	// Internal constructor to prevent instantiation of this class.
	CWindowElement(t_ElementType iElementType) : CControlElement(iElementType) {}

	// Render a window at the current position and current size using the specified rects for each tile.
	inline void Render(xrect& xTL, xrect& xTC, xrect& xTR, xrect& xML, xrect& xMC, xrect& xMR, xrect& xBL, xrect& xBC, xrect& xBR)
	{
		Render(xTL, xTC, xTR, 0);
		Render(xML, xMC, xMR, xTC.GetHeight());
		Render(xBL, xBC, xBR, xTC.GetHeight() + xMC.GetHeight());
	}

	// The height, in pixels, of the inner container.
	xint m_iHeight;

	// Specifies if the window can be dragged around the screen.
	xbool m_bMoveable;

	// Specifies if the window is being dragged.
	xbool m_bDragging;
};

//##############################################################################