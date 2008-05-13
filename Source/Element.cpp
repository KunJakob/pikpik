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

//##############################################################################

//##############################################################################
//
//                                 ROW ELEMENT
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                          9-May-2008
// =============================================================================
CRowElement::CRowElement(t_ElementType iElementType, CSpriteMetadata* pSprite) : CInterfaceElement(iElementType),
	m_pSprite(NULL),
	m_iWidth(0)
{
	m_pSprite = new CBasicSprite(pSprite);
}

// =============================================================================
// Nat Ryall                                                          9-May-2008
// =============================================================================
CRowElement::~CRowElement()
{
	delete m_pSprite;
}

// =============================================================================
// Nat Ryall                                                         11-May-2008
// =============================================================================
void CRowElement::Render(xrect& xLeft, xrect& xCentre, xrect& xRight, xint iVertOffset)
{
  // Ends.
  m_pSprite->Render(xLeft, GetPosition() + xpoint(0, iVertOffset));
  m_pSprite->Render(xRight, GetPosition() + xpoint(xLeft.Width() + m_iWidth, iVertOffset));

  // Centre.
  for (xint iX = 0, iDrawWidth = 0; iX < m_iWidth; iX += iDrawWidth)
  {
    iDrawWidth = Math::Clamp<xuint>(m_iWidth - iX, 0, xCentre.Width());

    xrect xTileRect = xCentre;
    xTileRect.iRight = xTileRect.iLeft + iDrawWidth;

    m_pSprite->Render(xTileRect, GetPosition() + xpoint(xLeft.Width() + iX, iVertOffset));
  }
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
CContainerElement::CContainerElement(t_ElementType iElementType, CSpriteMetadata* pSprite) : CRowElement(iElementType, pSprite),
	m_iHeight(0)
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
  // Top.
  CRowElement::Render(xTL, xTC, xTR, 0);

  // Middle
  for (xint iY = 0, iDrawHeight = 0; iY < m_iHeight; iY += iDrawHeight)
  {
    iDrawHeight = Math::Clamp<xuint>(m_iHeight - iY, 0, xMC.Height());

    xint iAreaBottom = xMC.iTop + iDrawHeight;

    xrect xLeft = xML;
    xLeft.iBottom = iAreaBottom;

    xrect xCentre = xMC;
    xCentre.iBottom = iAreaBottom;

    xrect xRight = xMR;
    xRight.iBottom = iAreaBottom;

    CRowElement::Render(xLeft, xCentre, xRight, xTC.Height() + iY);
  }

  // Bottom.
  CRowElement::Render(xBL, xBC, xBR, xTC.Height() + m_iHeight);
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
CCheckElement::CCheckElement(t_ElementType iElementType, CSpriteMetadata* pSprite, CLabelElement* pLabel) : CInterfaceElement(iElementType),
	m_pSprite(NULL),
	m_pLabel(pLabel),
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
	m_pSprite->Render(xArea, GetPosition());
}

//##############################################################################