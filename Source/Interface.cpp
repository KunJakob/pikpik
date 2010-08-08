//##############################################################################

// Global.
#include <Global.h>

// Local.
#include <Interface.h>

// Other.
#include <Renderer.h>

//##############################################################################

// =============================================================================
CInterfaceManager::CInterfaceManager() :
	m_pScreen(NULL),
	m_bCursorVisible(true),
	m_pActiveElement(NULL),
	m_pFocusedElement(NULL),
	m_bFoundActive(false),
	m_bDebugRender(false)
{
	m_pScreen = new CScreenElement();

	for (xint iA = 0; iA < ElementType_Max; ++iA)
	{
		m_pCursor[iA] = NULL;
	}

	Reset();
}

// =============================================================================
CInterfaceManager::~CInterfaceManager()
{
	delete m_pScreen;

	for (xint iA = 0; iA < ElementType_Max; ++iA)
	{
		if (m_pCursor[iA])
			delete m_pCursor[iA];
	}
}

// =============================================================================
void CInterfaceManager::Reset()
{
	m_pActiveElement = NULL;
	m_pFocusedElement = NULL;

	m_pScreen->Clear();

	for (xint iA = 0; iA < ElementType_Max; ++iA)
	{
		if (m_pCursor[iA])
		{
			delete m_pCursor[iA];
			m_pCursor[iA] = NULL;
		}
	}
}

// =============================================================================
xbool CInterfaceManager::OnEvent(xint iEventType, void* pEventInfo)
{
	hgeInputEvent* pEvent = (hgeInputEvent*)pEventInfo;

	switch (iEventType)
	{
	case INPUT_KEYDOWN:
		{
			if (m_pFocusedElement)
			{
				// Process the key-down.
				return m_pFocusedElement->OnKeyDown(pEvent->key);
			}
		}
		break;

	case INPUT_KEYUP:
		{
			if (m_pFocusedElement)
			{
				// Check for a valid char input.
				if (pEvent->chr && pEvent->chr >= ' ' && pEvent->chr <= '~')
					m_pFocusedElement->OnKeyChar((xchar)pEvent->chr);

				// Process the key-up.
				return m_pFocusedElement->OnKeyUp(pEvent->key);
			}
		}
		break;

	case INPUT_MBUTTONDOWN:
		{
			// Perform a mouse-down on the active element.
			if (m_pActiveElement)
			{
				SetFocus(m_pActiveElement);
				return m_pActiveElement->OnMouseDown(m_xMousePos);
			}
		}
		break;

	case INPUT_MBUTTONUP:
		{
			// Perform a mouse-up on the active element.
			if (m_pActiveElement)
				return m_pActiveElement->OnMouseUp(m_xMousePos);
		}
	}

	return false;
}

// =============================================================================
void CInterfaceManager::OnUpdate()
{
	m_xLastMousePos = m_xMousePos;

	xfloat fX;
	xfloat fY;

	_HGE->Input_GetMousePos(&fX, &fY);
	m_xMousePos = xpoint((xint)fX, (xint)fY);

	if (m_pActiveElement)
		m_pActiveElement->OnMouseMove(m_xMousePos - m_xLastMousePos, IsMouseDown());

	m_bFoundActive = false;

	UpdateElement(m_pScreen);

	if (m_pFocusedElement)
	{
		CInterfaceElement* pElement = m_pFocusedElement;

		while (pElement) 
		{
			pElement->ToFront();
			pElement = pElement->m_pParent;
		}
	}
}

// =============================================================================
void CInterfaceManager::Render(CRenderLayer* pLayer)
{
	RenderElement(m_pScreen);

#if !XRETAIL
	// Render debug boxes over the interface to show the active and focused elements.
	if (m_bDebugRender && m_pScreen->IsVisible())
	{
		if (m_pActiveElement && m_pActiveElement != m_pScreen)
		{
			xrect xRect = m_pActiveElement->GetArea();
			xuint iColour = ARGB(128, 32, 32, 32);

			RenderManager.RenderBox(true, xRect, iColour);
		}

		if (m_pFocusedElement)
		{
			xrect xRect = m_pFocusedElement->GetFocusArea() + xrect(1, 1, 0, 0);
			xuint iColour = ARGB(255, 255, 0, 0);

			RenderManager.RenderBox(false, xRect, iColour);
		}
	}
#endif

	// Render the cursor.
	if (m_bCursorVisible && _HGE->Input_IsMouseOver() && m_pCursor[ElementType_Unknown])
	{
		CBasicSprite* pCursor = m_pCursor[ElementType_Unknown];

		// If we have an active element and an custom cursor exists.
		if (m_pActiveElement && m_pCursor[m_pActiveElement->GetType()])
		{
			CInterfaceElement* pElement = m_pActiveElement;

			while (pElement->IsEnabled() && pElement != m_pScreen)
				pElement = pElement->GetParent();

			// Check that the there is nothing disabled and so blocking the custom cursor.
			if (pElement->IsEnabled())
				pCursor = m_pCursor[m_pActiveElement->GetType()];
		}

		pCursor->Render(m_xMousePos);
	}
}

// =============================================================================
CInterfaceElement* CInterfaceManager::FindElement(const xchar* pName)
{
	XLISTFOREACH(t_InterfaceElementList, ppElement, m_lpElements)
	{
		if ((*ppElement)->GetName() && String::IsMatch((*ppElement)->GetName(), pName))
			return *ppElement;
	}

	return NULL;
}

// =============================================================================
void CInterfaceManager::SetCursor(t_ElementType iType, CSpriteMetadata* pMetadata)
{
	if (m_pCursor[iType])
		delete m_pCursor[iType];

	m_pCursor[iType] = new CBasicSprite(pMetadata);
}

// =============================================================================
void CInterfaceManager::SetFocus(CInterfaceElement* pElement)
{
	if (m_pFocusedElement != pElement)
	{
		if (m_pFocusedElement)
			m_pFocusedElement->OnBlur();

		m_pFocusedElement = pElement;
		m_pFocusedElement->OnFocus();
	}
}

// =============================================================================
xbool CInterfaceManager::IsMouseOver(CInterfaceElement* pElement)
{
	return Math::IsIntersecting(m_xMousePos, pElement->GetArea());
}

// =============================================================================
void CInterfaceManager::UpdateElement(CInterfaceElement* pElement)
{
	if (pElement->IsEnabled() && pElement->IsVisible())
	{
		// Iterate through all children in reverse-render order.
		XLISTFOREACH_R(t_InterfaceElementList, ppElement, pElement->m_lpChildElements)
			UpdateElement(*ppElement);

		if (Math::IsIntersecting(m_xMousePos, pElement->GetArea()))
		{
			// If we are the first element intersecting, we become the active element.
			if (!m_bFoundActive)
			{
				m_bFoundActive = true;

				if (m_pActiveElement && m_pActiveElement != pElement)
					m_pActiveElement->OnMouseLeave();

				m_pActiveElement = pElement;
				m_pActiveElement->OnMouseEnter();
			}
		}

		pElement->OnUpdate();
	}
}

// =============================================================================
void CInterfaceManager::RenderElement(CInterfaceElement* pElement)
{
	if (pElement->IsVisible())
	{
		pElement->OnRender();

		XLISTFOREACH(t_InterfaceElementList, ppElement, pElement->m_lpChildElements)
			RenderElement(*ppElement);
	}
}

// =============================================================================
void CInterfaceManager::DeregisterElement(CInterfaceElement* pElement)
{
	if (CInterfaceElement* pParent = pElement->m_pParent)
		pParent->Detach(pElement);

	XLISTREMOVE(t_InterfaceElementList, m_lpElements, pElement);
}

//##############################################################################

// =============================================================================
CInterfaceElement::CInterfaceElement(t_ElementType iType) :
	m_pExtendedData(NULL),
	m_pParent(NULL),
	m_iType(iType),
	m_bVisible(true),
	m_bEnabled(true),
	m_pName(NULL)
{
}

// =============================================================================
CInterfaceElement::~CInterfaceElement()
{
}

// =============================================================================
CInterfaceElement* CInterfaceElement::FindChild(const xchar* pName)
{
	XLISTFOREACH(t_InterfaceElementList, ppElement, m_lpChildElements)
	{
		if ((*ppElement)->GetName() && String::IsMatch((*ppElement)->GetName(), pName))
			return *ppElement;

		if ((*ppElement)->m_lpChildElements.size())
		{
			if (CInterfaceElement* pElement = (*ppElement)->FindChild(pName))
				return pElement;
		}
	}

	return NULL;
}

// =============================================================================
void CInterfaceElement::Move(xpoint xOffset)
{
	m_xPosition += xOffset;

	XLISTFOREACH(t_InterfaceElementList, ppElement, m_lpChildElements)
		(*ppElement)->Move(xOffset);
}

// =============================================================================
void CInterfaceElement::Attach(CInterfaceElement* pElement)
{
	pElement->m_pParent = this;

	m_lpChildElements.push_back(pElement);

	InterfaceManager.RegisterElement(pElement);
}

// =============================================================================
void CInterfaceElement::Detach(CInterfaceElement* pElement)
{
	pElement->m_pParent = NULL;

	XLISTREMOVE(t_InterfaceElementList, m_lpChildElements, pElement);

	if (pElement == InterfaceManager.m_pActiveElement)
		InterfaceManager.m_pActiveElement = NULL;

	if (pElement == InterfaceManager.m_pFocusedElement)
		InterfaceManager.m_pFocusedElement = NULL;

	InterfaceManager.DeregisterElement(pElement);
}

// =============================================================================
void CInterfaceElement::Clear()
{
	while (m_lpChildElements.size())
		Detach(m_lpChildElements.front());
}

// =============================================================================
xbool CInterfaceElement::IsAttached(CInterfaceElement* pElement)
{
	XLISTFOREACH(t_InterfaceElementList, ppElement, m_lpChildElements)
	{
		if (pElement == *ppElement)
			return true;
	}

	return false;
}

// =============================================================================
void CInterfaceElement::ToFront()
{
	if (m_pParent)
	{
		XLISTREMOVE(t_InterfaceElementList, m_pParent->m_lpChildElements, this);
		m_pParent->m_lpChildElements.push_back(this);
	}
}

//##############################################################################