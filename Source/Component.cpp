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

//##############################################################################
//
//                                   LABEL
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         13-May-2008
// =============================================================================
CLabelComponent::CLabelComponent(CFontMetadata* pMetaFont) : CLabelElement(ElementType_Label, pMetaFont)
{
}

// =============================================================================
// Nat Ryall                                                         13-May-2008
// =============================================================================
CLabelComponent::~CLabelComponent()
{
}

// =============================================================================
// Nat Ryall                                                         13-May-2008
// =============================================================================
void CLabelComponent::OnRender()
{
	CLabelElement::OnRender();
}

//##############################################################################

//##############################################################################
//
//                                  HYPERLINK
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         10-Jul-2008
// =============================================================================
CHyperlinkComponent::CHyperlinkComponent(CFontMetadata* pFont, const xchar* pText, t_fpLinkSelectedCallback fpCallback) : CLabelElement(ElementType_Hyperlink, pFont),
	m_fpLinkSelectedCallback(fpCallback)
{
	SetText(pText);
}

// =============================================================================
// Nat Ryall                                                         10-Jul-2008
// =============================================================================
void CHyperlinkComponent::OnRender()
{
	CLabelElement::Render();
}

//##############################################################################

//##############################################################################
//
//                                 IMAGE BOX
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         13-May-2008
// =============================================================================
CImageComponent::CImageComponent(CSpriteMetadata* pMetaSprite) : CImageElement(ElementType_Image, pMetaSprite)
{
}

// =============================================================================
// Nat Ryall                                                         13-May-2008
// =============================================================================
CImageComponent::~CImageComponent()
{
}

// =============================================================================
// Nat Ryall                                                         13-May-2008
// =============================================================================
void CImageComponent::OnRender()
{
	CImageElement::Render();
}

//##############################################################################

//##############################################################################
//
//                                   BUTTON
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         11-May-2008
// =============================================================================
CButtonComponent::CButtonComponent(CSpriteMetadata* pMetaSprite, CFontMetadata* pMetaFont) : CRowElement(ElementType_Button, pMetaSprite),
	m_iButtonState(ButtonState_Normal),
	m_pFont(NULL),
	m_fpOnClickCallback(NULL)
{
	m_pL[ButtonState_Normal]		= pMetaSprite->FindArea("NormalLeft");
	m_pC[ButtonState_Normal]		= pMetaSprite->FindArea("NormalCentre"); 
	m_pR[ButtonState_Normal]		= pMetaSprite->FindArea("NormalRight");
	m_pL[ButtonState_Over]			= pMetaSprite->FindArea("OverLeft");
	m_pC[ButtonState_Over]			= pMetaSprite->FindArea("OverCentre");
	m_pR[ButtonState_Over]			= pMetaSprite->FindArea("OverRight"); 
	m_pL[ButtonState_Down]			= pMetaSprite->FindArea("DownLeft"); 
	m_pC[ButtonState_Down]			= pMetaSprite->FindArea("DownCentre"); 
	m_pR[ButtonState_Down]			= pMetaSprite->FindArea("DownRight");

	m_xFrameSize = xrect(m_pL[0]->m_xRect.Width(), 0, m_pR[0]->m_xRect.Width(), 0);

	if (pMetaFont)
		m_pFont = new CFont(pMetaFont);
}

// =============================================================================
// Nat Ryall                                                         11-May-2008
// =============================================================================
CButtonComponent::~CButtonComponent()
{
	if (m_pFont)
		delete m_pFont;
}

// =============================================================================
// Nat Ryall                                                         11-May-2008
// =============================================================================
void CButtonComponent::OnRender()
{
	CRowElement::Render(m_pL[m_iButtonState]->m_xRect, m_pC[m_iButtonState]->m_xRect, m_pR[m_iButtonState]->m_xRect);

	if (m_pFont)
		m_pFont->Render(m_xText.c_str(), xrect(0, 0, GetSize()) + GetPosition(), HGETEXT_CENTER | HGETEXT_MIDDLE);
}

//##############################################################################

//##############################################################################
//
//                                  TEXT BOX
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         12-May-2008
// =============================================================================
CInputComponent::CInputComponent(CSpriteMetadata* pMetaSprite, CFontMetadata* pMetaFont) : CRowElement(ElementType_Input, pMetaSprite),
	m_bMasked(false),
	m_iCharLimit(32),
	m_iCharOffset(0),
	m_iFlashTimer(0)
{
	m_pL = pMetaSprite->FindArea("Left"); 
	m_pC = pMetaSprite->FindArea("Centre"); 
	m_pR = pMetaSprite->FindArea("Right");

	m_xFrameSize = xrect(m_pL->m_xRect.Width(), 0, m_pR->m_xRect.Width(), 0);

	m_pFont = new CFont(pMetaFont);
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
void CInputComponent::OnUpdate()
{
	m_iFlashTimer = (m_iFlashTimer + _TIMEDELTA) % 1000;
}

// =============================================================================
// Nat Ryall                                                         12-May-2008
// =============================================================================
void CInputComponent::OnRender()
{
	// Render the element area.
	CRowElement::Render(m_pL->m_xRect, m_pC->m_xRect, m_pR->m_xRect);

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
			iWidth = m_pFont->GetStringWidth(xCheckString.c_str()) + m_pL->m_xRect.Width();

			if (iWidth > iTarget)
				break;

			iLastWidth = iWidth;
		}

		if (iWidth > iTarget && iTarget - iLastWidth < iWidth - iTarget)
			m_iCharOffset = (xint)xCheckString.length() - 1;
		else
			m_iCharOffset = (xint)xCheckString.length();
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

//##############################################################################
//
//                                PROGRESS BAR
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         13-May-2008
// =============================================================================
CProgressComponent::CProgressComponent(CSpriteMetadata* pMetaSprite) : CRowElement(ElementType_Progress, pMetaSprite),
	m_fProgress(0.f)
{
	m_pL = pMetaSprite->FindArea("Left");
	m_pC = pMetaSprite->FindArea("Centre");
	m_pR = pMetaSprite->FindArea("Right");

	m_pProgress = pMetaSprite->FindArea("Progress");

	m_xFrameSize = xrect(m_pL->m_xRect.Width(), 0, m_pR->m_xRect.Width(), 0);
}

// =============================================================================
// Nat Ryall                                                         13-May-2008
// =============================================================================
CProgressComponent::~CProgressComponent()
{
}

// =============================================================================
// Nat Ryall                                                         13-May-2008
// =============================================================================
void CProgressComponent::OnRender()
{
	CRowElement::Render(m_pL->m_xRect, m_pC->m_xRect, m_pR->m_xRect);

	if (m_fProgress)
		m_pSprite->RenderTiled(GetPosition() + xpoint(m_xFrameSize.iLeft, 0), xpoint((xint)((xfloat)m_iWidth * m_fProgress), 0), m_pProgress->m_xRect);
}

//##############################################################################

//##############################################################################
//
//                                   WINDOW
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         11-May-2008
// =============================================================================
CWindowComponent::CWindowComponent(CSpriteMetadata* pMetaSprite, CFontMetadata* pMetaFont) : CContainerElement(ElementType_Window, pMetaSprite),
	m_pFont(NULL),
	m_bMoveable(false),
	m_bDragging(false)
{
	m_pTL = pMetaSprite->FindArea("TopLeft"); 
	m_pTC = pMetaSprite->FindArea("TopCentre"); 
	m_pTR = pMetaSprite->FindArea("TopRight"); 
	m_pML = pMetaSprite->FindArea("MiddleLeft"); 
	m_pMC = pMetaSprite->FindArea("MiddleCentre"); 
	m_pMR = pMetaSprite->FindArea("MiddleRight"); 
	m_pBL = pMetaSprite->FindArea("BottomLeft"); 
	m_pBC = pMetaSprite->FindArea("BottomCentre"); 
	m_pBR = pMetaSprite->FindArea("BottomRight");

	m_xFrameSize = xrect
	(
		m_pML->m_xRect.Width(),
		m_pTC->m_xRect.Height(),
		m_pMR->m_xRect.Width(),
		m_pBC->m_xRect.Height()
	);

	if (pMetaFont)
		m_pFont = new CFont(pMetaFont);
}

// =============================================================================
// Nat Ryall                                                         11-May-2008
// =============================================================================
CWindowComponent::~CWindowComponent()
{
	if (m_pFont)
		delete m_pFont;
}

// =============================================================================
// Nat Ryall                                                         11-May-2008
// =============================================================================
void CWindowComponent::OnRender()
{
	// Render the container.
	CContainerElement::Render(m_pTL->m_xRect, m_pTC->m_xRect, m_pTR->m_xRect, m_pML->m_xRect, m_pMC->m_xRect, m_pMR->m_xRect, m_pBL->m_xRect, m_pBC->m_xRect, m_pBR->m_xRect);

	// Render the window title.
	if (m_pFont)
		m_pFont->Render(m_xTitle.c_str(), xrect(m_xFrameSize.iLeft + 2, 0, m_xFrameSize.iLeft + m_iWidth, m_xFrameSize.iTop) + GetPosition(), HGETEXT_LEFT | HGETEXT_MIDDLE);
}

//##############################################################################

//##############################################################################
//
//                                 GROUP BOX
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         20-May-2008
// =============================================================================
CGroupComponent::CGroupComponent(CSpriteMetadata* pMetaSprite, CFontMetadata* pMetaFont) : CContainerElement(ElementType_Group, pMetaSprite),
	m_pFont(NULL)
{
	m_pTL = pMetaSprite->FindArea("TopLeft"); 
	m_pTC = pMetaSprite->FindArea("TopCentre"); 
	m_pTR = pMetaSprite->FindArea("TopRight"); 
	m_pML = pMetaSprite->FindArea("MiddleLeft"); 
	m_pMC = pMetaSprite->FindArea("MiddleCentre"); 
	m_pMR = pMetaSprite->FindArea("MiddleRight"); 
	m_pBL = pMetaSprite->FindArea("BottomLeft"); 
	m_pBC = pMetaSprite->FindArea("BottomCentre"); 
	m_pBR = pMetaSprite->FindArea("BottomRight");

	m_xFrameSize = xrect
	(
		m_pML->m_xRect.Width(),
		m_pTC->m_xRect.Height(),
		m_pMR->m_xRect.Width(),
		m_pBC->m_xRect.Height()
	);

	if (pMetaFont)
		m_pFont = new CFont(pMetaFont);
}

// =============================================================================
// Nat Ryall                                                         20-May-2008
// =============================================================================
CGroupComponent::~CGroupComponent()
{
	if (m_pFont)
		delete m_pFont;
}

// =============================================================================
// Nat Ryall                                                         20-May-2008
// =============================================================================
void CGroupComponent::OnRender()
{
	bool bTitle = m_pFont && m_xTitle.length();

	// Render the container.
	CContainerElement::Render(m_pTL->m_xRect, bTitle ? xrect() : m_pTC->m_xRect, m_pTR->m_xRect, m_pML->m_xRect, m_pMC->m_xRect, m_pMR->m_xRect, m_pBL->m_xRect, m_pBC->m_xRect, m_pBR->m_xRect);

	// Render the window title.
	if (bTitle)
	{
		xint iTitleWidth = m_pFont->GetStringWidth(m_xTitle.c_str()) + 3;
		xint iTitleHeight = max(m_xFrameSize.Height(), m_pFont->GetFontHeight()) / 2;

		m_pFont->Render(m_xTitle.c_str(), GetPosition() + xpoint(m_xFrameSize.iLeft + 2, -iTitleHeight), HGETEXT_LEFT);
		m_pSprite->RenderTiled(GetPosition() + xpoint(m_xFrameSize.iLeft + iTitleWidth, 0), xpoint(m_iWidth - iTitleWidth, 0), m_pTC->m_xRect);
	}
}

//##############################################################################

//##############################################################################
//
//                                  LIST BOX
//
//##############################################################################

//##############################################################################

//##############################################################################
//
//                                 CHECK BOX
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         13-May-2008
// =============================================================================
CCheckComponent::CCheckComponent(CSpriteMetadata* pMetaSprite, CFontMetadata* pMetaFont) : CCheckElement(ElementType_Check, pMetaSprite),
	m_iCheckState(CheckState_Normal),
	m_pFont(NULL),
	m_fpOnCheckCallback(NULL)
{
	m_pBox[CheckState_Normal]		= pMetaSprite->FindArea("Normal"); 
	m_pBox[CheckState_Over]			= pMetaSprite->FindArea("Over"); 
	m_pBox[CheckState_Down]			= pMetaSprite->FindArea("Down"); 

	m_pCheck = pMetaSprite->FindArea("Check"); 

	if (pMetaFont)
		m_pFont = new CFont(pMetaFont);
}

// =============================================================================
// Nat Ryall                                                         13-May-2008
// =============================================================================
CCheckComponent::~CCheckComponent()
{
	if (m_pFont)
		delete m_pFont;
}

// =============================================================================
// Nat Ryall                                                         13-May-2008
// =============================================================================
void CCheckComponent::OnRender()
{
	xrect xArea = m_pBox[m_iCheckState]->m_xRect;
	CCheckElement::Render(xArea);

	if (m_bChecked)
		CCheckElement::Render(m_pCheck->m_xRect);

	if (m_pFont)
	{		
		xint iX = xArea.Width() + (xArea.Width() / 2);
		m_pFont->Render(m_xText.c_str(), xrect(iX, 0, iX + m_pFont->GetStringWidth(m_xText.c_str()), xArea.Height()) + GetPosition(), HGETEXT_LEFT | HGETEXT_MIDDLE);
	}
}

// =============================================================================
// Nat Ryall                                                         16-May-2008
// =============================================================================
void CCheckComponent::OnMouseUp(xpoint xPosition)
{
	m_bChecked = !m_bChecked;
	m_iCheckState = CheckState_Over;

	if (m_fpOnCheckCallback)
		m_fpOnCheckCallback(this, m_bChecked);
}

//##############################################################################

//##############################################################################
//
//                                RADIO BUTTON
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         13-May-2008
// =============================================================================
CRadioComponent::CRadioComponent(xint iRadioGroup, CSpriteMetadata* pSprite, CFontMetadata* pFont) : CCheckComponent(pSprite, pFont),
	m_iRadioGroup(iRadioGroup)
{
	m_iType = ElementType_Radio;
}

// =============================================================================
// Nat Ryall                                                         13-May-2008
// =============================================================================
CRadioComponent::~CRadioComponent()
{
}

// =============================================================================
// Nat Ryall                                                         13-May-2008
// =============================================================================
void CRadioComponent::OnMouseUp(xpoint xPosition)
{
	CRadioComponent* pRadio = GetChecked(m_iRadioGroup);

	if (pRadio)
		pRadio->SetChecked(false);

	m_bChecked = true;
	m_iCheckState = CheckState_Over;
}

// =============================================================================
// Nat Ryall                                                         21-May-2008
// =============================================================================
CRadioComponent* CRadioComponent::GetChecked(xint iRadioGroup)
{
	XEN_LIST_FOREACH(t_InterfaceElementList, ppElement, InterfaceManager.GetElementList())
	{
		if ((*ppElement)->GetType() == ElementType_Radio)
		{
			CRadioComponent* pRadio = (CRadioComponent*)*ppElement;

			if (pRadio->GetRadioGroup() == iRadioGroup && pRadio->IsChecked())
				return pRadio;
		}
	}

	return NULL;
}

//##############################################################################