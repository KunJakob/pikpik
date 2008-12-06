//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Global.
#include <Global.h>

// Local.
#include <Element.h>

// Other.
#include <Resource.h>

//##############################################################################

//##############################################################################
//
//                               LABEL ELEMENT
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                          9-May-2008
// =============================================================================
CLabelElement::CLabelElement(t_ElementType iElementType, CFontMetadata* pFont) : CInterfaceElement(iElementType),
	m_pFont(NULL),
	m_iAlignment(HGETEXT_LEFT)
{
	m_pFont = new CFont(pFont);
}

// =============================================================================
// Nat Ryall                                                          9-May-2008
// =============================================================================
CLabelElement::~CLabelElement()
{
	delete m_pFont;
}

// =============================================================================
// Nat Ryall                                                         13-May-2008
// =============================================================================
void CLabelElement::Render()
{
	m_pFont->Render(m_xText.c_str(), GetPosition(), m_iAlignment);
}

//##############################################################################

//##############################################################################
//
//                               IMAGE ELEMENT
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                          9-May-2008
// =============================================================================
CImageElement::CImageElement(t_ElementType iElementType, CSpriteMetadata* pSprite, CSpriteMetadata::CArea* pArea) : CInterfaceElement(iElementType),
	m_pSprite(NULL),
	m_pArea(pArea)
{
	m_pSprite = new CBasicSprite(pSprite);
}

// =============================================================================
// Nat Ryall                                                          9-May-2008
// =============================================================================
CImageElement::~CImageElement()
{
	delete m_pSprite;
}

// =============================================================================
// Nat Ryall                                                         13-May-2008
// =============================================================================
void CImageElement::Render()
{
	if (m_pArea)
		m_pSprite->Render(GetPosition(), m_pArea->m_xRect);
	else
		m_pSprite->Render(GetPosition());
}

//##############################################################################

//##############################################################################
//
//                                STRIP ELEMENT
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                          9-May-2008
// =============================================================================
CStripElement::CStripElement(t_ElementType iElementType, CSpriteMetadata* pSprite) : CInterfaceElement(iElementType),
	m_pSprite(NULL),
	m_iWidth(0)
{
	m_pSprite = new CBasicSprite(pSprite);
}

// =============================================================================
// Nat Ryall                                                          9-May-2008
// =============================================================================
CStripElement::~CStripElement()
{
	delete m_pSprite;
}

// =============================================================================
// Nat Ryall                                                         11-May-2008
// =============================================================================
void CStripElement::Render(xrect& xLeft, xrect& xCentre, xrect& xRight)
{
	m_pSprite->Render(GetPosition(), xLeft);
	m_pSprite->Render(GetPosition() + xpoint(m_xFrameSize.m_tLeft + m_iWidth, 0), xRight);
	m_pSprite->RenderTiled(GetPosition() + xpoint(m_xFrameSize.m_tLeft, 0), xpoint(m_iWidth, 0), xCentre);
}

//##############################################################################

//##############################################################################
//
//                             CONTAINER ELEMENT
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                          9-May-2008
// =============================================================================
CContainerElement::CContainerElement(t_ElementType iElementType, CSpriteMetadata* pSprite) : CStripElement(iElementType, pSprite),
	m_iHeight(0),
	m_iElementSpacing(0)
{
}

// =============================================================================
// Nat Ryall                                                          9-May-2008
// =============================================================================
CContainerElement::~CContainerElement()
{
}

// =============================================================================
// Nat Ryall                                                         11-May-2008
// =============================================================================
void CContainerElement::Render(xrect& xTL, xrect& xTC, xrect& xTR, xrect& xML, xrect& xMC, xrect& xMR, xrect& xBL, xrect& xBC, xrect& xBR)
{
	xpoint xPosition = GetPosition();

	// Top.
	m_pSprite->Render(xPosition, xTL);
	m_pSprite->RenderTiled(xPosition + xpoint(m_xFrameSize.m_tLeft, 0), xpoint(m_iWidth, 0), xTC);
	m_pSprite->Render(xPosition + xpoint(m_xFrameSize.m_tLeft + m_iWidth, 0), xTR);

	xPosition.m_tY += m_xFrameSize.m_tTop;

	// Middle
	m_pSprite->RenderTiled(xPosition, xpoint(0, m_iHeight), xML);
	m_pSprite->RenderTiled(xPosition + xpoint(m_xFrameSize.m_tLeft, 0), xpoint(m_iWidth, m_iHeight), xMC);
	m_pSprite->RenderTiled(xPosition + xpoint(m_xFrameSize.m_tLeft + m_iWidth, 0), xpoint(0, m_iHeight), xMR);

	xPosition.m_tY += m_iHeight;

	// Bottom.
	m_pSprite->Render(xPosition, xBL);
	m_pSprite->RenderTiled(xPosition + xpoint(m_xFrameSize.m_tLeft, 0), xpoint(m_iWidth, 0), xBC);
	m_pSprite->Render(xPosition + xpoint(m_xFrameSize.m_tLeft + m_iWidth, 0), xBR);
}

// =============================================================================
// Nat Ryall                                                         22-Oct-2008
// =============================================================================
void CContainerElement::ArrangeChildren()
{
	xint iContainerHeight = 0;
	xint iContainerWidth = 0;

	XEN_LIST_FOREACH(t_InterfaceElementList, ppElement, m_lpChildElements)
	{
		CInterfaceElement* pElement = *ppElement;

		// Re-position the element.
		pElement->SetPosition(GetInnerPosition() + m_xElementPadding.GetTopLeft() + xpoint(0, iContainerHeight));

		// Determine the width of the container based on the element area and increase the offset.
		xrect xArea = pElement->GetArea();

		iContainerWidth = Math::Max(iContainerWidth, xArea.GetWidth());
		iContainerHeight += m_iElementSpacing + xArea.GetHeight();
	}

	// Resize the container to the size of the elements.
	if (iContainerHeight != 0)
		iContainerHeight -= m_iElementSpacing;

	SetInnerSize(iContainerWidth + m_xElementPadding.m_tLeft + m_xElementPadding.m_tRight, iContainerHeight + m_xElementPadding.m_tTop + m_xElementPadding.m_tBottom);
}

//##############################################################################

//##############################################################################
//
//                               CHECK ELEMENT
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                          9-May-2008
// =============================================================================
CCheckElement::CCheckElement(t_ElementType iElementType, CSpriteMetadata* pSprite) : CInterfaceElement(iElementType),
	m_pSprite(NULL),
	m_bChecked(false)
{
	m_pSprite = new CBasicSprite(pSprite);
}

// =============================================================================
// Nat Ryall                                                          9-May-2008
// =============================================================================
CCheckElement::~CCheckElement()
{
	delete m_pSprite;
}

// =============================================================================
// Nat Ryall                                                         13-May-2008
// =============================================================================
void CCheckElement::Render(xrect& xArea)
{
	m_pSprite->Render(GetPosition(), xArea);
}

//##############################################################################