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

// Global.
#include <Global.h>

// Other.
#include <Interface.h>
#include <Sprite.h>
#include <Font.h>

//##############################################################################
class CLabelElement : public CInterfaceElement
{
public:
	// Virtual destructor to ensure proper cleanup of all child classes.
	virtual ~CLabelElement();

	// Get the width.
	virtual xint GetWidth()
	{
		return m_pFont->GetStringWidth(GetText());
	}

	// Get the height.
	virtual xint GetHeight()
	{
		return m_pFont->GetFontHeight();
	}

	// Set the text string to render.
	inline void SetText(const xchar* pString)
	{
		m_xText = pString;
	}

	// Get the text string to be rendered.
	inline const xchar* GetText()
	{
		return m_xText.c_str();
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

	// Render the text string using the current font.
	void Render();

	// The element font.
	CFont* m_pFont;

	// The element text string.
	xstring m_xText;

	// The text alignment relative to the position.
	xuint m_iAlignment;
};

//##############################################################################
class CImageElement : public CInterfaceElement
{
public:
	// Virtual destructor to ensure proper cleanup of all child classes.
	virtual ~CImageElement();

	// Get the width including all borders.
	virtual xint GetWidth()
	{
		return m_pArea ? m_pArea->m_xRect.GetWidth() : m_pSprite->GetImageWidth();
	}

	// Get the height including all borders.
	virtual xint GetHeight()
	{
		return m_pArea ? m_pArea->m_xRect.GetHeight() : m_pSprite->GetImageHeight();
	}

protected:
	// Internal constructor to prevent instantiation of this class.
	CImageElement(t_ElementType iElementType, CSpriteMetadata* pSprite, CSpriteMetadata::CArea* pArea = NULL);

	// Render the image element using the current area.
	void Render();

	// The element sprite.
	CBasicSprite* m_pSprite;

	// The element image render area.
	CSpriteMetadata::CArea* m_pArea;
};

//##############################################################################
class CStripElement : public CInterfaceElement
{
public:
	// Virtual destructor to ensure proper cleanup of all child classes.
	virtual ~CStripElement();

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
		return m_xFrameSize.m_tLeft + m_xFrameSize.m_tRight;
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
	CStripElement(t_ElementType iElementType, CSpriteMetadata* pSprite);

	// Render a control at the current position and current size using the specified rects for each tile.
	void Render(xrect& xLeft, xrect& xCentre, xrect& xRight);

	// The element sprite.
	CBasicSprite* m_pSprite;

	// The frame size from all directions.
	xrect m_xFrameSize;

	// The width, in pixels, of the inner container.
	xint m_iWidth;
};

//##############################################################################
class CContainerElement : public CStripElement
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
		return m_xFrameSize.m_tLeft + m_xFrameSize.m_tRight;
	}

	// Get the border height including both the top and bottom borders.
	inline xint GetBorderHeight()
	{
		return m_xFrameSize.m_tTop + m_xFrameSize.m_tBottom;
	}

	// Get the inner width and height as a border exclusive size.
	inline xpoint GetInnerSize()
	{
		return xpoint(GetInnerWidth(), GetInnerHeight());
	}

	// Get the screen position of the inner-area.
	inline xpoint GetInnerPosition()
	{
		return GetPosition() + xpoint(m_xFrameSize.m_tLeft, m_xFrameSize.m_tTop);
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

	// Set the width and height using a border inclusive size.
	inline void SetSize(xpoint xSize)
	{
		SetSize(xSize.m_tX, xSize.m_tY);
	}

	// Set the inner width and height using a border exclusive size.
	inline void SetInnerSize(xint iWidth, xint iHeight)
	{
		SetInnerWidth(iWidth);
		SetInnerHeight(iHeight);
	}

	// Set the inner width and height using a border exclusive size.
	inline void SetInnerSize(xpoint xSize)
	{
		SetInnerSize(xSize.m_tX, xSize.m_tY);
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

	// Resize the container to contain all the child elements completely.
	void ArrangeChildren();

	// Set the auto-sizing element inner padding.
	inline void SetElementPadding(xrect xElementPadding) 
	{ 
		m_xElementPadding = xElementPadding; 
	}

	// Get the auto-sizing element inner padding.
	inline xrect GetElementPadding() 
	{ 
		return m_xElementPadding; 
	}

	// Set the auto-sizing element spacing.
	inline void SetElementSpacing(xint iElementSpacing) 
	{ 
		m_iElementSpacing = iElementSpacing; 
	}

	// Get the auto-sizing element spacing.
	inline xint GetElementSpacing() 
	{ 
		return m_iElementSpacing; 
	}

protected:
	// Internal constructor to prevent instantiation of this class.
	CContainerElement(t_ElementType iElementType, CSpriteMetadata* pSprite);

	// Render a window at the current position and current size using the specified rects for each tile.
	void Render(xrect& xTL, xrect& xTC, xrect& xTR, xrect& xML, xrect& xMC, xrect& xMR, xrect& xBL, xrect& xBC, xrect& xBR);

	// The height, in pixels, of the inner container.
	xint m_iHeight;

	// Specifies the inner padding for elements when auto-size is enabled.
	xrect m_xElementPadding;

	// Specifies the vertical spacing between elements when auto-size is enabled.
	xint m_iElementSpacing;
};

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
	CCheckElement(t_ElementType iElementType, CSpriteMetadata* pSprite);

	// Render the element at the current position using the specified rect area.
	void Render(xrect& xArea);

	// The element sprite.
	CBasicSprite* m_pSprite;

	// The element's checked status.
	xbool m_bChecked;
};

//##############################################################################