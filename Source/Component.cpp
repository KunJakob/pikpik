//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Global.
#include <Global.h>

// Local.
#include <Component.h>

//##############################################################################
//
//                                   LABEL
//
//##############################################################################

//##############################################################################
//
//                                 IMAGE BOX
//
//##############################################################################

//##############################################################################
//
//                                   BUTTON
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         11-May-2008
// =============================================================================
CButton::CButton(CSpriteMetadata* pSprite, CFontMetadata* pFont) : CRowElement(ElementType_Button, pSprite),
  m_iButtonState(ButtonState_Normal),
  m_pFont(NULL),
  m_fpOnClickCallback(NULL)
{
  if (pFont)
    m_pFont = new CFont(pFont);

  m_pNL = pSprite->FindArea("NormalLeft"); 
  m_pNM = pSprite->FindArea("NormalMiddle"); 
  m_pNR = pSprite->FindArea("NormalRight"); 
  m_pOL = pSprite->FindArea("OverLeft"); 
  m_pOM = pSprite->FindArea("OverMiddle"); 
  m_pOR = pSprite->FindArea("OverRight"); 
  m_pDL = pSprite->FindArea("DownLeft"); 
  m_pDM = pSprite->FindArea("DownMiddle"); 
  m_pDR = pSprite->FindArea("DownRight"); 
}

// =============================================================================
// Nat Ryall                                                         11-May-2008
// =============================================================================
CButton::~CButton()
{
  if (m_pFont)
    delete m_pFont;
}

// =============================================================================
// Nat Ryall                                                         11-May-2008
// =============================================================================
void CButton::Render()
{
  switch (m_iButtonState)
  {
  case ButtonState_Normal:
    CRowElement::Render(m_pNL->xRect, m_pNM->xRect, m_pNR->xRect);
    break;

  case ButtonState_Over:
    CRowElement::Render(m_pOL->xRect, m_pOM->xRect, m_pOR->xRect);
    break;

  case ButtonState_Down:
    CRowElement::Render(m_pDL->xRect, m_pDM->xRect, m_pDR->xRect);
    break;
  }

  //if (m_pFont)
  //  m_pFont->Render(m_xLabel.c_str(), GetPosition() + (XPOINT(GetWidth(), GetHeight() - m_pFont->GetFontHeight()) / 2), HGETEXT_CENTER);  }
}

//##############################################################################
//
//                                  TEXT BOX
//
//##############################################################################

//##############################################################################
//
//                                PROGRESS BAR
//
//##############################################################################

//##############################################################################
//
//                                   WINDOW
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         11-May-2008
// =============================================================================
CWindow::CWindow(CSpriteMetadata* pSprite) : CContainerElement(ElementType_Window, pSprite, NULL)
{
  m_pTL = pMetadata->FindArea("TopLeft"); 
  m_pTM = pMetadata->FindArea("TopMiddle"); 
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
// Nat Ryall                                                         11-May-2008
// =============================================================================
CWindow::~CWindow()
{

}

// =============================================================================
// Nat Ryall                                                         11-May-2008
// =============================================================================
CWindow::Render()
{

}

//##############################################################################
//
//                                  LIST BOX
//
//##############################################################################

//##############################################################################
//
//                                 CHECK BOX
//
//##############################################################################

//##############################################################################
//
//                                RADIO BUTTON
//
//##############################################################################

//##############################################################################
//
//                                 SCROLL BAR
//
//##############################################################################