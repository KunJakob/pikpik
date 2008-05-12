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
CButtonComponent::CButtonComponent(CSpriteMetadata* pSprite, CLabelElement* pLabel) : CRowElement(ElementType_Button, pSprite),
  m_iButtonState(ButtonState_Normal),
  m_pLabel(pLabel),
  m_fpOnClickCallback(NULL)
{
  m_pLeft[ButtonState_Normal]			= pSprite->FindArea("NormalLeft");
	m_pLeft[ButtonState_Over]				= pSprite->FindArea("OverLeft");
	m_pLeft[ButtonState_Down]				= pSprite->FindArea("DownLeft"); 

  m_pCentre[ButtonState_Normal]		= pSprite->FindArea("NormalCentre"); 
  m_pCentre[ButtonState_Over]			= pSprite->FindArea("OverCentre");
  m_pCentre[ButtonState_Down]			= pSprite->FindArea("DownCentre"); 

	m_pRight[ButtonState_Normal]		= pSprite->FindArea("NormalRight");
	m_pRight[ButtonState_Over]			= pSprite->FindArea("OverRight"); 
  m_pRight[ButtonState_Down]			= pSprite->FindArea("DownRight");

	m_xFrameSize = xrect(m_pLeft[0]->xRect.GetWidth(), 0, m_pRight[0]->xRect.GetWidth(), 0);
}

// =============================================================================
// Nat Ryall                                                         11-May-2008
// =============================================================================
CButtonComponent::~CButtonComponent()
{
}

// =============================================================================
// Nat Ryall                                                         11-May-2008
// =============================================================================
void CButtonComponent::Render()
{
  CRowElement::Render(m_pLeft[m_iButtonState]->xRect, m_pCentre[m_iButtonState]->xRect, m_pRight[m_iButtonState]->xRect);

  //if (m_pLabel)
  //  m_pFont->Render(m_xLabel.c_str(), GetPosition() + (XPOINT(GetWidth(), GetHeight() - m_pFont->GetFontHeight()) / 2), HGETEXT_CENTER);  }
}

//##############################################################################
//
//                                  TEXT BOX
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         12-May-2008
// =============================================================================
CInputComponent::CInputComponent(CSpriteMetadata* pSprite, CFontMetadata* pFont) : CRowElement(ElementType_Input, pSprite),
	m_bMasked(false),
	m_iCharLimit(32),
	m_iCharOffset(0),
	m_iFlashTimer(0)
{
	m_pLeft = pSprite->FindArea("Left"); 
	m_pCentre = pSprite->FindArea("Centre"); 
	m_pRight = pSprite->FindArea("Right");

	m_xFrameSize = xrect(m_pLeft->xRect.GetWidth(), 0, m_pRight->xRect.GetWidth(), 0);

	m_pFont = new CFont(pFont);
}

// =============================================================================
// Nat Ryall                                                         12-May-2008
// =============================================================================
CInputComponent::~CInputComponent()
{
	if (m_pFont)
		delete m_pFont;
}

// =============================================================================
// Nat Ryall                                                         12-May-2008
// =============================================================================
void CInputComponent::Update()
{
	m_iFlashTimer = (m_iFlashTimer + _TIMEDELTA) % 1000;
}

// =============================================================================
// Nat Ryall                                                         12-May-2008
// =============================================================================
void CInputComponent::Render()
{
	// Render the element area.
	CRowElement::Render(m_pLeft->xRect, m_pCentre->xRect, m_pRight->xRect);

	// Get the render text and render text offset.
	xstring xRenderText = m_xText;
	
	if (m_bMasked)
		xRenderText.replace(0, xRenderText.length(), xRenderText.length(), '*');

	xpoint xTextOffset(m_xFrameSize.iLeft + 1, (GetHeight() - m_pFont->GetFontHeight()) / 2);

	// Render the text.
	m_pFont->Render(xRenderText.c_str(), GetPosition() + xTextOffset, HGETEXT_LEFT);

	// Render the cursor if we're focused.
	if (InterfaceManager.IsFocusedElement(this) && m_iFlashTimer < 500)
	{
		xRenderText = xRenderText.substr(0, m_iCharOffset);

		xint iCursorX = xTextOffset.iX + m_pFont->GetStringWidth(xRenderText.c_str());
		xrect iLinePoints = xrect(iCursorX, xTextOffset.iY, iCursorX, GetHeight() - xTextOffset.iY) + GetPosition();

		_HGE->Gfx_RenderLine((float)iLinePoints.iLeft, (float)iLinePoints.iTop, (float)iLinePoints.iRight, (float)iLinePoints.iBottom, 0xFF000000); 
	}
}

// =============================================================================
// Nat Ryall                                                          4-May-2008
// =============================================================================
void CInputComponent::OnMouseDown(xpoint xPosition)
{
	if (m_xText.length())
	{
		xstring xCheckString;

		xint iTarget = xPosition.iX - GetPosition().iX;
		xint iWidth = 0;
		xint iLastWidth = 0;

		for (xint iA = 0; iA < (xint)m_xText.length(); ++iA)
		{
			xCheckString += m_bMasked ? '*' : m_xText[iA];
			iWidth = m_pFont->GetStringWidth(xCheckString.c_str()) + m_pLeft->xRect.GetWidth();

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
void CInputComponent::OnKeyDown(xint iVirtualKey)
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
			if (m_iCharOffset < (xint)m_xText.length())
				m_iCharOffset++;
		}
		break;
	}

	m_iFlashTimer = 0;
}

// =============================================================================
// Nat Ryall                                                          4-May-2008
// =============================================================================
void CInputComponent::OnKeyChar(xchar cChar)
{
	if (m_iCharLimit > (xint)m_xText.length())
		m_xText.insert(m_iCharOffset++, 1, cChar);
}

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
CWindowComponent::CWindowComponent(CSpriteMetadata* pSprite, CLabelElement* pLabel) : CContainerElement(ElementType_Window, pSprite),
	m_pLabel(pLabel),
	m_bMoveable(false),
	m_bDragging(false)
{
  m_pTL = pSprite->FindArea("TopLeft"); 
  m_pTC = pSprite->FindArea("TopCentre"); 
  m_pTR = pSprite->FindArea("TopRight"); 
  m_pML = pSprite->FindArea("MiddleLeft"); 
  m_pMC = pSprite->FindArea("MiddleCentre"); 
  m_pMR = pSprite->FindArea("MiddleRight"); 
  m_pBL = pSprite->FindArea("BottomLeft"); 
  m_pBC = pSprite->FindArea("BottomCentre"); 
  m_pBR = pSprite->FindArea("BottomRight");

  m_xFrameSize = xrect
  (
    m_pML->xRect.GetWidth(),
    m_pTC->xRect.GetHeight(),
    m_pMR->xRect.GetWidth(),
    m_pBC->xRect.GetHeight()
  );
}

// =============================================================================
// Nat Ryall                                                         11-May-2008
// =============================================================================
CWindowComponent::~CWindowComponent()
{
}

// =============================================================================
// Nat Ryall                                                         11-May-2008
// =============================================================================
void CWindowComponent::Render()
{
	CContainerElement::Render(m_pTL->xRect, m_pTC->xRect, m_pTR->xRect, m_pML->xRect, m_pMC->xRect, m_pMR->xRect, m_pBL->xRect, m_pBC->xRect, m_pBR->xRect);
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