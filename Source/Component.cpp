//##############################################################################

// Global.
#include <Global.h>

// Local.
#include <Component.h>

//##############################################################################

// =============================================================================
CLabelComponent::CLabelComponent(CFontMetadata* pMetaFont) : CLabelElement(ElementType_Label, pMetaFont)
{
}

// =============================================================================
CLabelComponent::~CLabelComponent()
{
}

// =============================================================================
void CLabelComponent::OnRender()
{
	CLabelElement::Render();
}

//##############################################################################

// =============================================================================
CHyperlinkComponent::CHyperlinkComponent(CFontMetadata* pFont) : CLabelElement(ElementType_Hyperlink, pFont),
	m_fpOnClickCallback(NULL)
{
}

// =============================================================================
void CHyperlinkComponent::OnRender()
{
	CLabelElement::Render();
}

// =============================================================================
xbool CHyperlinkComponent::OnMouseUp(xpoint xPosition)
{
	if (m_fpOnClickCallback)
		m_fpOnClickCallback();

	return true;
}

//##############################################################################

// =============================================================================
CImageComponent::CImageComponent(CSpriteMetadata* pMetaSprite) : CImageElement(ElementType_Image, pMetaSprite)
{
}

// =============================================================================
CImageComponent::~CImageComponent()
{
}

// =============================================================================
void CImageComponent::OnRender()
{
	CImageElement::Render();
}

//##############################################################################

// =============================================================================
CButtonComponent::CButtonComponent(CSpriteMetadata* pMetaSprite, CFontMetadata* pMetaFont) : CStripElement(ElementType_Button, pMetaSprite),
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

	m_xFrameSize = xrect(m_pL[0]->m_xRect.GetWidth(), 0, m_pR[0]->m_xRect.GetWidth(), 0);

	if (pMetaFont)
		m_pFont = new CFont(pMetaFont);
}

// =============================================================================
CButtonComponent::~CButtonComponent()
{
	if (m_pFont)
		delete m_pFont;
}

// =============================================================================
void CButtonComponent::OnRender()
{
	CStripElement::Render(m_pL[m_iButtonState]->m_xRect, m_pC[m_iButtonState]->m_xRect, m_pR[m_iButtonState]->m_xRect);

	if (m_pFont)
		m_pFont->Render(m_xText.c_str(), xrect(0, 0, GetSize()) + GetPosition(), HGETEXT_CENTER | HGETEXT_MIDDLE);
}

// =============================================================================
xbool CButtonComponent::OnMouseDown(xpoint xPosition)
{
	m_iButtonState = ButtonState_Down;

	return true;
}

// =============================================================================
xbool CButtonComponent::OnMouseUp(xpoint xPosition)
{
	m_iButtonState = ButtonState_Over;

	if (m_fpOnClickCallback)
		m_fpOnClickCallback(this, InterfaceManager.GetMousePosition() - GetPosition());

	return true;
}

//##############################################################################

// =============================================================================
CInputComponent::CInputComponent(CSpriteMetadata* pMetaSprite, CFontMetadata* pMetaFont) : CStripElement(ElementType_Input, pMetaSprite),
	m_bMasked(false),
	m_iCharLimit(32),
	m_iCharOffset(0),
	m_iFlashTimer(0)
{
	m_pL = pMetaSprite->FindArea("Left"); 
	m_pC = pMetaSprite->FindArea("Centre"); 
	m_pR = pMetaSprite->FindArea("Right");

	m_xFrameSize = xrect(m_pL->m_xRect.GetWidth(), 0, m_pR->m_xRect.GetWidth(), 0);

	m_pFont = new CFont(pMetaFont);
}

// =============================================================================
CInputComponent::~CInputComponent()
{
	if (m_pFont)
		delete m_pFont;
}

// =============================================================================
void CInputComponent::OnUpdate()
{
	m_iFlashTimer = (m_iFlashTimer + _TIMEDELTA) % 1000;
}

// =============================================================================
void CInputComponent::OnRender()
{
	// Render the element area.
	CStripElement::Render(m_pL->m_xRect, m_pC->m_xRect, m_pR->m_xRect);

	// Get the render text and render text offset.
	xstring xRenderText = m_xText;

	if (m_bMasked)
		xRenderText.replace(0, xRenderText.length(), xRenderText.length(), '*');

	xpoint xTextOffset(m_xFrameSize.m_tLeft + 1, (GetHeight() - m_pFont->GetFontHeight()) / 2);

	// Render the text.
	m_pFont->Render(xRenderText.c_str(), GetPosition() + xTextOffset, HGETEXT_LEFT);

	// Render the cursor if we're focused.
	if (InterfaceManager.IsFocusedElement(this) && m_iFlashTimer < 500)
	{
		xRenderText = xRenderText.substr(0, m_iCharOffset);

		xint iCursorX = xTextOffset.m_tX + m_pFont->GetStringWidth(xRenderText.c_str());
		xrect iLinePoints = xrect(iCursorX, xTextOffset.m_tY, iCursorX, GetHeight() - xTextOffset.m_tY) + GetPosition();

		_HGE->Gfx_RenderLine((float)iLinePoints.m_tLeft, (float)iLinePoints.m_tTop, (float)iLinePoints.m_tRight, (float)iLinePoints.m_tBottom, 0xFF000000); 
	}
}

// =============================================================================
xbool CInputComponent::OnMouseDown(xpoint xPosition)
{
	if (m_xText.length())
	{
		xstring xCheckString;

		xint iTarget = xPosition.m_tX - GetPosition().m_tX;
		xint iWidth = 0;
		xint iLastWidth = 0;

		for (xint iA = 0; iA < (xint)m_xText.length(); ++iA)
		{
			xCheckString += m_bMasked ? '*' : m_xText[iA];
			iWidth = m_pFont->GetStringWidth(xCheckString.c_str()) + m_pL->m_xRect.GetWidth();

			if (iWidth > iTarget)
				break;

			iLastWidth = iWidth;
		}

		if (iWidth > iTarget && iTarget - iLastWidth < iWidth - iTarget)
			m_iCharOffset = (xint)xCheckString.length() - 1;
		else
			m_iCharOffset = (xint)xCheckString.length();
	}

	return true;
}

// =============================================================================
xbool CInputComponent::OnKeyDown(xint iVirtualKey)
{
	switch (iVirtualKey)
	{
	case HGEK_BACKSPACE:
		{
			if (m_iCharOffset)
				m_xText.erase(--m_iCharOffset, 1);

			return true;
		}
		break;

	case HGEK_LEFT:
		{
			if (m_iCharOffset)
				m_iCharOffset--;

			return true;
		}
		break;

	case HGEK_RIGHT:
		{
			if (m_iCharOffset < (xint)m_xText.length())
				m_iCharOffset++;

			return true;
		}
		break;
	}

	m_iFlashTimer = 0;

	return false;
}

// =============================================================================
void CInputComponent::OnKeyChar(xchar cChar)
{
	if (m_iCharLimit > (xint)m_xText.length())
		m_xText.insert(m_iCharOffset++, 1, cChar);
}

//##############################################################################

// =============================================================================
CProgressComponent::CProgressComponent(CSpriteMetadata* pMetaSprite) : CStripElement(ElementType_Progress, pMetaSprite),
	m_fProgress(0.f)
{
	m_pL = pMetaSprite->FindArea("Left");
	m_pC = pMetaSprite->FindArea("Centre");
	m_pR = pMetaSprite->FindArea("Right");

	m_pProgress = pMetaSprite->FindArea("Progress");

	m_xFrameSize = xrect(m_pL->m_xRect.GetWidth(), 0, m_pR->m_xRect.GetWidth(), 0);
}

// =============================================================================
CProgressComponent::~CProgressComponent()
{
}

// =============================================================================
void CProgressComponent::OnRender()
{
	CStripElement::Render(m_pL->m_xRect, m_pC->m_xRect, m_pR->m_xRect);

	if (m_fProgress)
		m_pSprite->RenderTiled(GetPosition() + xpoint(m_xFrameSize.m_tLeft, 0), xpoint((xint)((xfloat)m_iWidth * m_fProgress), 0), m_pProgress->m_xRect);
}

//##############################################################################

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
		m_pML->m_xRect.GetWidth(),
		m_pTC->m_xRect.GetHeight(),
		m_pMR->m_xRect.GetWidth(),
		m_pBC->m_xRect.GetHeight()
	);

	if (pMetaFont)
		m_pFont = new CFont(pMetaFont);
}

// =============================================================================
CWindowComponent::~CWindowComponent()
{
	if (m_pFont)
		delete m_pFont;
}

// =============================================================================
void CWindowComponent::OnRender()
{
	// Render the container.
	CContainerElement::Render(m_pTL->m_xRect, m_pTC->m_xRect, m_pTR->m_xRect, m_pML->m_xRect, m_pMC->m_xRect, m_pMR->m_xRect, m_pBL->m_xRect, m_pBC->m_xRect, m_pBR->m_xRect);

	// Render the window title.
	if (m_pFont)
		m_pFont->Render(m_xTitle.c_str(), xrect(m_xFrameSize.m_tLeft + 2, 0, m_xFrameSize.m_tLeft + m_iWidth, m_xFrameSize.m_tTop) + GetPosition(), HGETEXT_LEFT | HGETEXT_MIDDLE);
}

// =============================================================================
xbool CWindowComponent::OnMouseDown(xpoint xPosition)
{
	if (m_bMoveable && Math::IsIntersecting(xPosition, xrect(m_xFrameSize.m_tLeft, 0, GetInnerWidth(), m_xFrameSize.m_tTop) + GetPosition()))
		m_bDragging = true;

	return true;
}

// =============================================================================
xbool CWindowComponent::OnMouseUp(xpoint xPosition)
{
	m_bDragging = false;

	return true;
}

// =============================================================================
void CWindowComponent::OnMouseMove(xpoint xDifference, xbool bMouseDown)
{
	if (m_bDragging)
		Move(xDifference);
}

//##############################################################################

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
		m_pML->m_xRect.GetWidth(),
		m_pTC->m_xRect.GetHeight(),
		m_pMR->m_xRect.GetWidth(),
		m_pBC->m_xRect.GetHeight()
	);

	if (pMetaFont)
		m_pFont = new CFont(pMetaFont);
}

// =============================================================================
CGroupComponent::~CGroupComponent()
{
	if (m_pFont)
		delete m_pFont;
}

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
		xint iTitleHeight = max(m_xFrameSize.GetHeight(), m_pFont->GetFontHeight()) / 2;

		m_pFont->Render(m_xTitle.c_str(), GetPosition() + xpoint(m_xFrameSize.m_tLeft + 2, -iTitleHeight), HGETEXT_LEFT);
		m_pSprite->RenderTiled(GetPosition() + xpoint(m_xFrameSize.m_tLeft + iTitleWidth, 0), xpoint(m_iWidth - iTitleWidth, 0), m_pTC->m_xRect);
	}
}

//##############################################################################

// =============================================================================
CCheckComponent::CCheckComponent(CSpriteMetadata* pMetaSprite, CFontMetadata* pMetaFont) : CCheckElement(ElementType_Check, pMetaSprite),
	m_iCheckState(CheckState_Normal),
	m_pFont(NULL),
	m_fpOnCheckCallback(NULL),
	m_fpCheckBinding(NULL)
{
	m_pBox[CheckState_Normal]		= pMetaSprite->FindArea("Normal"); 
	m_pBox[CheckState_Over]			= pMetaSprite->FindArea("Over"); 
	m_pBox[CheckState_Down]			= pMetaSprite->FindArea("Down"); 

	m_pCheck = pMetaSprite->FindArea("Check"); 

	if (pMetaFont)
		m_pFont = new CFont(pMetaFont);
}

// =============================================================================
CCheckComponent::~CCheckComponent()
{
	if (m_pFont)
		delete m_pFont;
}

// =============================================================================
void CCheckComponent::OnRender()
{
	xrect xArea = m_pBox[m_iCheckState]->m_xRect;

	CCheckElement::Render(xArea);

	if (m_bChecked)
		CCheckElement::Render(m_pCheck->m_xRect);

	if (m_pFont)
	{		
		xint iX = xArea.GetWidth() + (xArea.GetWidth() / 2);
		m_pFont->Render(m_xText.c_str(), xrect(iX, 0, iX + m_pFont->GetStringWidth(m_xText.c_str()), xArea.GetHeight()) + GetPosition(), HGETEXT_LEFT | HGETEXT_MIDDLE);
	}
}

// =============================================================================
xbool CCheckComponent::OnMouseDown(xpoint xPosition)
{
	m_iCheckState = CheckState_Down;

	return true;
}

// =============================================================================
xbool CCheckComponent::OnMouseUp(xpoint xPosition)
{
	m_bChecked = !m_bChecked;
	m_iCheckState = CheckState_Over;

	if (m_fpOnCheckCallback)
		m_fpOnCheckCallback(this, m_bChecked);

	if (m_fpCheckBinding)
		m_fpCheckBinding(m_bChecked);

	return true;
}

//##############################################################################

// =============================================================================
CRadioComponent::CRadioComponent(xint iRadioGroup, CSpriteMetadata* pSprite, CFontMetadata* pFont) : CCheckComponent(pSprite, pFont),
	m_iRadioGroup(iRadioGroup)
{
	m_iType = ElementType_Radio;
}

// =============================================================================
CRadioComponent::~CRadioComponent()
{
}

// =============================================================================
xbool CRadioComponent::OnMouseUp(xpoint xPosition)
{
	CRadioComponent* pRadio = GetChecked(m_iRadioGroup);

	if (pRadio)
		pRadio->SetChecked(false);

	m_bChecked = true;
	m_iCheckState = CheckState_Over;

	return true;
}

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

// =============================================================================
CWindowComponent* CComponentFactory::CreateWindow(CMetadata* pMetadata, const char* pName)
{
	CDataset* pDataset = pMetadata->GetDataset("Window", pName);
	
	if (pDataset)
		return CreateWindow(pDataset);

	return NULL;
}

// =============================================================================
void CComponentFactory::GetBasicProperties(CInterfaceElement* pElement, CDataset* pDataset)
{
	if (const xchar* pName = pDataset->GetName())
		pElement->SetName(pName);

	if (XEN_METADATA_PROPERTY_EXISTS(pDataset, "Enabled"))
		pElement->SetEnabled(XEN_METADATA_PROPERTY->GetBool());

	if (XEN_METADATA_PROPERTY_EXISTS(pDataset, "Visible"))
		pElement->SetVisible(XEN_METADATA_PROPERTY->GetBool());

	if (XEN_METADATA_PROPERTY_EXISTS(pDataset, "Position"))
		pElement->SetPosition(XEN_METADATA_PROPERTY->GetPoint());
}

// =============================================================================
void CComponentFactory::GetLabelProperties(CLabelElement* pElement, CDataset* pDataset)
{
	if (XEN_METADATA_PROPERTY_EXISTS(pDataset, "Text"))
		pElement->SetText(Global.GetLocaleFromVar(XEN_METADATA_PROPERTY->GetString()));

	if (XEN_METADATA_PROPERTY_EXISTS(pDataset, "Alignment"))
	{
		if (String::IsMatch(XEN_METADATA_PROPERTY->GetString(), "Left"))
			pElement->SetAlignment(HGETEXT_LEFT);

		else if (String::IsMatch(XEN_METADATA_PROPERTY->GetString(), "Centre"))
			pElement->SetAlignment(HGETEXT_CENTER);

		else if (String::IsMatch(XEN_METADATA_PROPERTY->GetString(), "Right"))
			pElement->SetAlignment(HGETEXT_RIGHT);
	}
}

// =============================================================================
void CComponentFactory::GetImageProperties(CImageElement* pElement, CDataset* pDataset)
{
}

// =============================================================================
void CComponentFactory::GetStripProperties(CStripElement* pElement, CDataset* pDataset)
{
	if (XEN_METADATA_PROPERTY_EXISTS(pDataset, "Width"))
		pElement->SetWidth(XEN_METADATA_PROPERTY->GetInt());

	else if (XEN_METADATA_PROPERTY_EXISTS(pDataset, "InnerWidth"))
		pElement->SetInnerWidth(XEN_METADATA_PROPERTY->GetInt());
}

// =============================================================================
void CComponentFactory::GetContainerProperties(CContainerElement* pElement, CDataset* pDataset)
{
	if (XEN_METADATA_DATASET_EXISTS(pDataset, "AutoArrange"))
	{
		XASSERT(XEN_METADATA_DATASET->GetProperty("Padding"));
		XASSERT(XEN_METADATA_DATASET->GetProperty("Spacing"));

		pElement->SetElementPadding(XEN_METADATA_DATASET->GetProperty("Padding")->GetRect());
		pElement->SetElementSpacing(XEN_METADATA_DATASET->GetProperty("Spacing")->GetInt());

		pElement->ArrangeChildren();
	}
	else
	{
		if (XEN_METADATA_PROPERTY_EXISTS(pDataset, "Size"))
			pElement->SetSize(XEN_METADATA_PROPERTY->GetPoint());

		else if (XEN_METADATA_PROPERTY_EXISTS(pDataset, "InnerSize"))
			pElement->SetInnerSize(XEN_METADATA_PROPERTY->GetPoint());
	}
}

// =============================================================================
void CComponentFactory::GetCheckProperties(CCheckElement* pElement, CDataset* pDataset)
{
	if (XEN_METADATA_PROPERTY_EXISTS(pDataset, "Checked"))
		pElement->SetChecked(XEN_METADATA_PROPERTY->GetBool());
}

// =============================================================================
void CComponentFactory::AttachChildren(CInterfaceElement* pElement, CDataset* pDataset)
{
	for (xint iA = 0; iA < pDataset->GetDatasetCount(); ++iA)
	{
		CDataset* pWorkingDataset = pDataset->GetDataset(iA);
		const xchar* pType = pWorkingDataset->GetType();

		if (String::IsMatch(pType, "Label"))
			pElement->Attach(CreateLabel(pWorkingDataset));

		else if (String::IsMatch(pType, "Hyperlink"))
			pElement->Attach(CreateHyperlink(pWorkingDataset));

		else if (String::IsMatch(pType, "Image"))
			pElement->Attach(CreateImage(pWorkingDataset));

		else if (String::IsMatch(pType, "Button"))
			pElement->Attach(CreateButton(pWorkingDataset));

		else if (String::IsMatch(pType, "Input"))
			pElement->Attach(CreateInput(pWorkingDataset));

		else if (String::IsMatch(pType, "Progress"))
			pElement->Attach(CreateProgress(pWorkingDataset));

		else if (String::IsMatch(pType, "Window"))
			pElement->Attach(CreateWindow(pWorkingDataset));

		else if (String::IsMatch(pType, "Group"))
			pElement->Attach(CreateGroup(pWorkingDataset));

		else if (String::IsMatch(pType, "Check"))
			pElement->Attach(CreateCheck(pWorkingDataset));

		else if (String::IsMatch(pType, "Radio"))
			pElement->Attach(CreateRadio(pWorkingDataset));
	}
}

// =============================================================================
CLabelComponent* CComponentFactory::CreateLabel(CDataset* pDataset)
{
	XASSERT(pDataset->GetProperty("Font"));

	CFontMetadata* pFont = _FONT(pDataset->GetProperty("Font")->GetString());

	CLabelComponent* pLabel = new CLabelComponent(pFont);

	GetBasicProperties(pLabel, pDataset);
	GetLabelProperties(pLabel, pDataset);

	return pLabel;
}

// =============================================================================
CHyperlinkComponent* CComponentFactory::CreateHyperlink(CDataset* pDataset)
{
	XASSERT(pDataset->GetProperty("Font"));

	CFontMetadata* pFont = _FONT(pDataset->GetProperty("Font")->GetString());

	CHyperlinkComponent* pLabel = new CHyperlinkComponent(pFont);

	GetBasicProperties(pLabel, pDataset);
	GetLabelProperties(pLabel, pDataset);

	return pLabel;
}

// =============================================================================
CImageComponent* CComponentFactory::CreateImage(CDataset* pDataset)
{
	XASSERT(pDataset->GetProperty("Sprite"));

	CSpriteMetadata* pSprite = _SPRITE(pDataset->GetProperty("Sprite")->GetString());

	CImageComponent* pImage = new CImageComponent(pSprite);

	GetBasicProperties(pImage, pDataset);
	GetImageProperties(pImage, pDataset);

	return pImage;
}

// =============================================================================
CButtonComponent* CComponentFactory::CreateButton(CDataset* pDataset)
{
	XASSERT(pDataset->GetProperty("Sprite"));
	XASSERT(pDataset->GetProperty("Font"));

	CSpriteMetadata* pSprite = _SPRITE(pDataset->GetProperty("Sprite")->GetString());
	CFontMetadata* pFont = _FONT(pDataset->GetProperty("Font")->GetString());

	CButtonComponent* pButton = new CButtonComponent(pSprite, pFont);

	GetBasicProperties(pButton, pDataset);

	if (XEN_METADATA_PROPERTY_EXISTS(pDataset, "Text"))
		pButton->SetText(Global.GetLocaleFromVar(XEN_METADATA_PROPERTY->GetString()));

	GetStripProperties(pButton, pDataset);

	return pButton;
}

// =============================================================================
CInputComponent* CComponentFactory::CreateInput(CDataset* pDataset)
{
	XASSERT(pDataset->GetProperty("Sprite"));
	XASSERT(pDataset->GetProperty("Font"));

	CSpriteMetadata* pSprite = _SPRITE(pDataset->GetProperty("Sprite")->GetString());
	CFontMetadata* pFont = _FONT(pDataset->GetProperty("Font")->GetString());

	CInputComponent* pInput = new CInputComponent(pSprite, pFont);

	GetBasicProperties(pInput, pDataset);

	if (XEN_METADATA_PROPERTY_EXISTS(pDataset, "Text"))
		pInput->SetText(Global.GetLocaleFromVar(XEN_METADATA_PROPERTY->GetString()));

	if (XEN_METADATA_PROPERTY_EXISTS(pDataset, "Masked"))
		pInput->SetMasked(XEN_METADATA_PROPERTY->GetBool());

	if (XEN_METADATA_PROPERTY_EXISTS(pDataset, "Limit"))
		pInput->SetCharacterLimit(XEN_METADATA_PROPERTY->GetInt());

	GetStripProperties(pInput, pDataset);

	return pInput;
}

// =============================================================================
CProgressComponent* CComponentFactory::CreateProgress(CDataset* pDataset)
{
	XASSERT(pDataset->GetProperty("Sprite"));

	CSpriteMetadata* pSprite = _SPRITE(pDataset->GetProperty("Sprite")->GetString());

	CProgressComponent* pProgress = new CProgressComponent(pSprite);

	GetBasicProperties(pProgress, pDataset);

	if (XEN_METADATA_PROPERTY_EXISTS(pDataset, "Progress"))
		pProgress->SetProgress(XEN_METADATA_PROPERTY->GetFloat());

	GetStripProperties(pProgress, pDataset);

	return pProgress;
}

// =============================================================================
CWindowComponent* CComponentFactory::CreateWindow(CDataset* pDataset)
{
	XASSERT(pDataset->GetProperty("Sprite"));
	XASSERT(pDataset->GetProperty("Font"));

	CSpriteMetadata* pSprite = _SPRITE(pDataset->GetProperty("Sprite")->GetString());
	CFontMetadata* pFont = _FONT(pDataset->GetProperty("Font")->GetString());

	CWindowComponent* pWindow = new CWindowComponent(pSprite, pFont);

	GetBasicProperties(pWindow, pDataset);

	if (XEN_METADATA_PROPERTY_EXISTS(pDataset, "Title"))
		pWindow->SetTitle(Global.GetLocaleFromVar(XEN_METADATA_PROPERTY->GetString()));

	if (XEN_METADATA_PROPERTY_EXISTS(pDataset, "Moveable"))
		pWindow->SetMoveable(XEN_METADATA_PROPERTY->GetBool());

	AttachChildren(pWindow, pDataset);

	GetContainerProperties(pWindow, pDataset);

	return pWindow;
}

// =============================================================================
CGroupComponent* CComponentFactory::CreateGroup(CDataset* pDataset)
{
	XASSERT(pDataset->GetProperty("Sprite"));
	XASSERT(pDataset->GetProperty("Font"));

	CSpriteMetadata* pSprite = _SPRITE(pDataset->GetProperty("Sprite")->GetString());
	CFontMetadata* pFont = _FONT(pDataset->GetProperty("Font")->GetString());

	CGroupComponent* pGroup = new CGroupComponent(pSprite, pFont);

	GetBasicProperties(pGroup, pDataset);

	if (XEN_METADATA_PROPERTY_EXISTS(pDataset, "Title"))
		pGroup->SetTitle(Global.GetLocaleFromVar(XEN_METADATA_PROPERTY->GetString()));

	AttachChildren(pGroup, pDataset);

	GetContainerProperties(pGroup, pDataset);

	return pGroup;
}

// =============================================================================
CListComponent* CComponentFactory::CreateList(CDataset* pDataset)
{
	return NULL;
}

// =============================================================================
CCheckComponent* CComponentFactory::CreateCheck(CDataset* pDataset)
{
	XASSERT(pDataset->GetProperty("Sprite"));
	XASSERT(pDataset->GetProperty("Font"));

	CSpriteMetadata* pSprite = _SPRITE(pDataset->GetProperty("Sprite")->GetString());
	CFontMetadata* pFont = _FONT(pDataset->GetProperty("Font")->GetString());

	CCheckComponent* pCheck = new CCheckComponent(pSprite, pFont);

	GetBasicProperties(pCheck, pDataset);

	if (XEN_METADATA_PROPERTY_EXISTS(pDataset, "Text"))
		pCheck->SetText(Global.GetLocaleFromVar(XEN_METADATA_PROPERTY->GetString()));

	GetCheckProperties(pCheck, pDataset);

	return pCheck;
}

// =============================================================================
CRadioComponent* CComponentFactory::CreateRadio(CDataset* pDataset)
{
	XASSERT(pDataset->GetProperty("Group"));
	XASSERT(pDataset->GetProperty("Sprite"));
	XASSERT(pDataset->GetProperty("Font"));

	xint iGroup = pDataset->GetProperty("Group")->GetInt();
	CSpriteMetadata* pSprite = _SPRITE(pDataset->GetProperty("Sprite")->GetString());
	CFontMetadata* pFont = _FONT(pDataset->GetProperty("Font")->GetString());

	CRadioComponent* pRadio = new CRadioComponent(iGroup, pSprite, pFont);

	GetBasicProperties(pRadio, pDataset);
	
	if (XEN_METADATA_PROPERTY_EXISTS(pDataset, "Text"))
		pRadio->SetText(Global.GetLocaleFromVar(XEN_METADATA_PROPERTY->GetString()));

	GetCheckProperties(pRadio, pDataset);

	return pRadio;
}

//##############################################################################
