#pragma once

/**
* @file Element.h
* @author Nat Ryall
* @date 8/05/2008
* @brief Basic element types for use as interface objects.
*
* Copyright © SAPIAN
*/

//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Global.
#include <Global.h>

// Other.
#include <Interface.h>
#include <Sprite.h>
#include <Font.h>

//##############################################################################

//##############################################################################
//
//                               LABEL ELEMENT
//
//##############################################################################
class CLabelElement : public CInterfaceElement
{
public:
	// Virtual destructor to ensure proper cleanup of all child classes.
	virtual ~CLabelElement();

	// Get the width.
	virtual xint GetWidth()
	{
		return m_pFont->GetStringWidth(GetString());
	}

	// Get the height.
	virtual xint GetHeight()
	{
		return m_pFont->GetFontHeight();
	}

	// Set the text string to render.
	inline void SetString(const xchar* pString)
	{
		m_xString = pString;
	}

	// Get the text string to be rendered.
	inline const xchar* GetString()
	{
		return m_xString.c_str();
	}

	// Set the text alignment. Use HGETEXT_*.
	inline void SetAlignment(xuint iAlignment)
	{
		m_iAlignment = iAlignment;
	}

	// Get the text alignment.
	inline xuint GetAlignment()
	{
		return m_iAlignment;
	}

protected:
	// Internal constructor to prevent instantiation of this class.
	CLabelElement(t_ElementType iElementType, CFontMetadata* pFont);

	// The element font.
	CFont* m_pFont;

	// The element text string.
	xstring m_xString;

	// The text alignment relative to the position.
	xuint m_iAlignment;
};

//##############################################################################

//##############################################################################
//
//                               IMAGE ELEMENT
//
//##############################################################################
class CImageElement : public CInterfaceElement
{
public:
	// Virtual destructor to ensure proper cleanup of all child classes.
	virtual ~CImageElement();

	// Get the width including all borders.
	virtual xint GetWidth()
	{
		return m_pArea ? m_pArea->xRect.GetWidth() : m_pSprite->GetImageWidth();
	}

	// Get the height including all borders.
	virtual xint GetHeight()
	{
		return m_pArea ? m_pArea->xRect.GetHeight() : m_pSprite->GetImageHeight();
	}

protected:
	// Internal constructor to prevent instantiation of this class.
	CImageElement(t_ElementType iElementType, CSpriteMetadata* pSprite, CSpriteMetadata::CArea* pArea = NULL);

	// The element sprite.
	CBasicSprite* m_pSprite;

	// The element image render area.
	CSpriteMetadata::CArea* m_pArea;
};

//##############################################################################

//##############################################################################
//
//                                 ROW ELEMENT
//
//##############################################################################
class CRowElement : public CInterfaceElement
{
public:
	// Virtual destructor to ensure proper cleanup of all child classes.
	virtual ~CRowElement();

	// Get the width including all borders.
	inline virtual xint GetWidth()
	{
		return GetInnerWidth() + GetBorderWidth();
	}

	// Get the height including all borders.
	virtual xint GetHeight() = 0;

	// Get the width excluding any borders.
	xint GetInnerWidth()
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
	CRowElement(t_ElementType iElementType, CSpriteMetadata* pSprite);

	// Render a control at the current position and current size using the specified rects for each tile.
	void Render(xrect& xLeft, xrect& xCentre, xrect& xRight, xint iVertOffset = 0);

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
//                             CONTAINER ELEMENT
//
//##############################################################################
class CContainerElement : public CRowElement
{
public:
	// Virtual destructor to ensure proper cleanup of all child classes.
	virtual ~CContainerElement();

	// Get the width including all borders.
	virtual xint GetWidth()
	{
		return GetInnerWidth() + GetBorderWidth();
	}

	// Get the height including all borders.
	virtual xint GetHeight()
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

protected:
	// Internal constructor to prevent instantiation of this class.
	CContainerElement(t_ElementType iElementType, CSpriteMetadata* pSprite);

	// Render a window at the current position and current size using the specified rects for each tile.
	void Render(xrect& xTL, xrect& xTC, xrect& xTR, xrect& xML, xrect& xMC, xrect& xMR, xrect& xBL, xrect& xBC, xrect& xBR);

	// The height, in pixels, of the inner container.
	xint m_iHeight;
};

//##############################################################################

//##############################################################################
//
//                               CHECK ELEMENT
//
//##############################################################################
class CCheckElement : public CInterfaceElement
{
public:
	// Virtual destructor to ensure proper cleanup of all child classes.
	virtual ~CCheckElement();

	// Get the width including the label.
	virtual xint GetWidth() = 0;

	// Get the height including the label.
	virtual xint GetHeight() = 0;

	// Set the element checked status.
	inline void SetChecked(xbool bChecked)
	{
		m_bChecked = bChecked;
	}

	// See if the element is checked.
	inline xbool IsChecked()
	{
		return m_bChecked;
	}

protected:
	// Internal constructor to prevent instantiation of this class.
	CCheckElement(t_ElementType iElementType, CSpriteMetadata* pSprite, CLabelElement* pLabel);

	// Render the element at the current position using the specified rect area.
	void Render(xrect& xArea);

	// The element sprite.
	CBasicSprite* m_pSprite;

	// The label element, can be NULL.
	CLabelElement* m_pLabel;

	// The element's checked status.
	xbool m_bChecked;
};

//##############################################################################