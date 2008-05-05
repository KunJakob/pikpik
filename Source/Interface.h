#pragma once

/**
* @file Interface.h
* @author Nat Ryall
* @date 30/04/2008
* @brief Interface manager and elements.
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
#include <Renderer.h>
#include <Resource.h>
#include <Sprite.h>
#include <Font.h>

//##############################################################################

//##############################################################################
//
//                                   MACROS
//
//##############################################################################

// Shortcuts.
#define InterfaceManager CInterfaceManager::Get()

//##############################################################################

//##############################################################################
//
//                                   TYPES
//
//##############################################################################

// Predeclare.
class CInterfaceElement;
class CContainer;

// The types of interface elements.
enum t_ElementType
{
  ElementType_Unknown,
  ElementType_Container,
  ElementType_Window,
  ElementType_Label,
  ElementType_Button,
  ElementType_InputBox,

  ElementType_MenuLink,
  
  ElementType_Max,
};

// Lists.
typedef XLIST<CInterfaceElement*> t_ElementList;

//##############################################################################

//##############################################################################
//
//                             INTERFACE MANAGER
//
//##############################################################################
class CInterfaceManager : public Templates::CSingletonT<CInterfaceManager>
{
public:
  /**
  * 
  */
  CInterfaceManager();

  /**
  * 
  */
  ~CInterfaceManager();

	/**
	* 
	*/
	void Reset();

  /**
  * 
  */
  virtual void Update();

  /**
  * 
  */
  virtual void Render();

  /**
  * 
  */
  CContainer* GetContainer()
  {
    return m_pContainer;
  }

  /**
  * 
  */
  void SetCursor(t_ElementType iType, CSpriteMetadata* pMetadata);

  /**
  * 
  */
  void SetCursor(CSpriteMetadata* pMetadata)
  {
    SetCursor(ElementType_Unknown, pMetadata);
  }

	/**
	* 
	*/
	CInterfaceElement* GetActiveElement()
	{
		return m_pActiveElement;
	}

	/**
	* 
	*/
	XBOOL IsActiveElement(CInterfaceElement* pElement)
	{
		return m_pActiveElement == pElement;
	}

	/**
	* 
	*/
	void SetFocus(CInterfaceElement* pElement);

	/**
	* 
	*/
	CInterfaceElement* GetFocusedElement()
	{
		return m_pFocusedElement;
	}

  /**
  * 
  */
  XBOOL IsFocusedElement(CInterfaceElement* pElement)
  {
    return m_pFocusedElement == pElement;
  }

	/**
	* 
	*/
	XPOINT GetMousePosition()
	{
		return m_xMousePos;
	}

	/**
	* 
	*/
	XBOOL IsMouseOver(CInterfaceElement* pElement);

	/**
	* 
	*/
	XBOOL IsMouseDown()
	{
		return _HGE->Input_GetKeyState(HGEK_LBUTTON);
	}

  /**
  * 
  */
  void SetDebugRender(XBOOL bDebug)
  {
    m_bDebugRender = bDebug;
  }

protected:
  /**
  * 
  */
  void UpdateElement(CInterfaceElement* pElement);

  /**
  * 
  */
  void RenderElement(CInterfaceElement* pElement);

  /**
  * 
  */
  void CheckIntersection(CInterfaceElement* pElement);

  // The base container object.
  CContainer* m_pContainer;

  // The cursor sprite.
  CBasicSprite* m_pCursor[ElementType_Max];

  // The current mouse position.
  XPOINT m_xMousePos;

  // The last mouse position.
  XPOINT m_xLastMousePos;

  // The active element the mouse is over.
  CInterfaceElement* m_pActiveElement;

	// The last focused element.
	CInterfaceElement* m_pFocusedElement;

private:
  // Tracking variable for finding the current active element.
  XBOOL m_bFoundActive;

  // Specifies if debug rendering is used.
  XBOOL m_bDebugRender;
};

//##############################################################################

//##############################################################################
//
//                             INTERFACE ELEMENT
//
//##############################################################################
class CInterfaceElement
{
public:
  // Friend.
  friend CInterfaceManager;

  /**
  * 
  */
  virtual ~CInterfaceElement() {}

  /**
  * 
  */
  virtual void Update() {}
  
  /**
  * 
  */
  virtual void Render() = 0;

  /**
  * 
  */
  CInterfaceElement* GetParent() 
  { 
    return m_pParent; 
  }

  /**
  * 
  */
  XUINT GetType()
  {
    return m_iType;
  }

  /**
  * 
  */
  void SetVisible(XBOOL bVisible)
  {
    m_bVisible = bVisible;
  }

  /**
  * 
  */
  XBOOL IsVisible()
  {
    return m_bVisible;
  }

  /**
  * 
  */
  void SetEnabled(XBOOL bEnabled)
  {
    m_bEnabled = bEnabled;
  }

  /**
  * 
  */
  XBOOL IsEnabled()
  {
    return m_bEnabled;
  }

  /**
  * 
  */
  virtual XUINT GetWidth() = 0;

	/**
	* 
	*/
	virtual XUINT GetHeight() = 0;

	/**
	* 
	*/
	XPOINT GetSize()
	{
		return XPOINT(GetWidth(), GetHeight());
	}

	/**
	* Get the element area for collision and event purposes.
	*/
	XRECT GetArea()
	{
		return XRECT(m_xPosition, m_xPosition + GetSize());
	}

	/**
	* 
	*/
	virtual XRECT GetFocusArea()
	{
		return GetArea();
	}

  /**
  * 
  */
  void SetPosition(XPOINT xPosition)
	{
		Move(xPosition - m_xPosition);
	}

	/**
	* 
	*/
	void Move(XPOINT xOffset);

  /**
  * 
  */
  XPOINT GetPosition()
  {
    return m_xPosition;
  }

	/**
	* 
	*/
	XPOINT GetLocalPosition()
	{
		if (m_pParent)
			return GetPosition() - m_pParent->GetPosition();
		else
			return GetPosition();
	}

  /**
  * 
  */
  void Attach(CInterfaceElement* pElement);
  
  /**
  * 
  */
  void Detach(CInterfaceElement* pElement);

	/**
	* 
	*/
	void DetachAll()
	{
		m_lpChildElements.clear();
	}

	/**
	* 
	*/
	void ToFront();

protected:
  /**
  * 
  */
  CInterfaceElement(t_ElementType iType);

  /**
  * Fired on click/tab or manually with SetFocus().
  */
  virtual void OnFocus() {}
  virtual void OnBlur() {}

  /**
  * Fired when active.
  */
  virtual void OnMouseEnter() {}
  virtual void OnMouseLeave() {}
  virtual void OnMouseMove(XPOINT xDifference) {}
  virtual void OnMouseDown(XPOINT xPosition) {}
  virtual void OnMouseUp(XPOINT xPosition) {}

  /**
  * Fired when in focus.
  */
  virtual void OnKeyChar(XINT iChar) {}
  virtual void OnKeyDown(XUINT iVirtualKey) {}
  virtual void OnKeyUp(XUINT iVirtualKey) {}

  // The parent element or NULL if top level.
  CInterfaceElement* m_pParent;

  // The type of the element.
  t_ElementType m_iType;

  // Specifies if the element is visible.
  XBOOL m_bVisible;

  // Specifies if the element is enabled and will accept events.
  XBOOL m_bEnabled;

  // A list of child elements.
  t_ElementList m_lpChildElements;

private:
	// The screen position of the element.
	XPOINT m_xPosition;
};

//##############################################################################

//##############################################################################
//
//                                 CONTAINER
//
//##############################################################################
class CContainer : public CInterfaceElement
{
public:
  /**
  * 
  */
  CContainer();

  /**
  * 
  */
  virtual void Render() {}

  /**
  * 
  */
  void SetSize(XUINT iWidth, XUINT iHeight)
  {
    m_iWidth = iWidth;
    m_iHeight = iHeight;
  }

  /**
  * 
  */
  virtual XUINT GetWidth()
  {
    return m_iWidth;
  }

  /**
  * 
  */
  virtual XUINT GetHeight()
  {
    return m_iHeight;
  }

protected:
  // The width, in pixels, of the container.
  XUINT m_iWidth;

  // The height, in pixels, of the container.
  XUINT m_iHeight;
};

//##############################################################################

//##############################################################################
//
//                                   WINDOW
//
//##############################################################################
class CWindow : public CInterfaceElement
{
public:
  /**
  * 
  */
  CWindow(CSpriteMetadata* pMetadata);

  /**
  * 
  */
  virtual ~CWindow();

  /**
  * 
  */
  virtual void Render();

	/**
	* 
	*/
	virtual XUINT GetWidth()
	{
		return GetInnerWidth() + m_xFrameSize.iLeft + m_xFrameSize.iRight;
	}

	/**
	* 
	*/
	virtual XUINT GetHeight()
	{
		return GetInnerHeight() + m_xFrameSize.iTop + m_xFrameSize.iBottom;
	}

  /**
  * Set the size of the window including the border.
  */
  void SetSize(XUINT iWidth, XUINT iHeight);

  /**
  * Set the size of the inner-area of the window excluding the border.
  */
  void SetInnerSize(XUINT iWidth, XUINT iHeight)
  {
    m_iWidth = iWidth; 
    m_iHeight = iHeight;
  }

	/**
	* 
	*/
	XUINT GetInnerWidth()
	{
		return m_iWidth;
	}

	/**
	* 
	*/
	XUINT GetInnerHeight()
	{
		return m_iHeight;
	}

	/**
	* 
	*/
	XPOINT GetInnerPosition()
	{
		return GetPosition() + XPOINT(m_xFrameSize.iLeft, m_xFrameSize.iTop);
	}

	/**
	* 
	*/
	XRECT GetFrameSize()
	{
		return m_xFrameSize;
	}

	/**
	* 
	*/
	virtual XRECT GetFocusArea()
	{
		return XRECT(0, 0, GetInnerWidth(), GetInnerHeight()) + GetInnerPosition();
	}

  /**
  * 
  */
  void SetMoveable(XBOOL bMoveable)
  {
    m_bMoveable = bMoveable;
  }

  /**
  * 
  */
  XBOOL IsMoveable()
  {
    return m_bMoveable;
  }

protected:
  // Internal types.
  enum t_AreaIndex
  {
    AreaIndex_TopLeft,
    AreaIndex_TopMiddle,
    AreaIndex_TopRight,
    AreaIndex_MiddleLeft,
    AreaIndex_Middle,
    AreaIndex_MiddleRight,
    AreaIndex_BottomLeft,
    AreaIndex_BottomMiddle,
    AreaIndex_BottomRight,
    /*MAX*/AreaIndex_Max,
  };

  /**
  * 
  */
  void InternalRender(XRECT& xRect, XPOINT xOffset);

  /**
  * 
  */
  virtual void OnMouseMove(XPOINT xDifference) 
  {
    if (m_bDragging)
      Move(xDifference);
  }

  /**
  * 
  */
  virtual void OnMouseDown(XPOINT xPosition) 
  {
    if (m_bMoveable && Math::Intersect(xPosition, XRECT(m_xFrameSize.iLeft, 0, GetInnerWidth(), m_xFrameSize.iTop) + GetPosition()))
      m_bDragging = true;
  }

  /**
  * 
  */
  virtual void OnMouseUp(XPOINT xPosition) 
  {
    m_bDragging = false;
  }

  // The element sprite.
  CBasicSprite* m_pSprite;

  // The element area list.
  CSpriteMetadata::CArea* m_pAreas[AreaIndex_Max];

  // The width, in pixels, of the inner container.
  XUINT m_iWidth;

  // The height, in pixels, of the inner container.
  XUINT m_iHeight;

	// The frame size from all directions.
	XRECT m_xFrameSize;

	// Specifies if the window is being dragged.
	XBOOL m_bDragging;

  // Specifies if the window can be dragged around the screen or if it is static.
  XBOOL m_bMoveable;
};

//##############################################################################

//##############################################################################
//
//                                   LABEL
//
//##############################################################################
class CLabel : public CInterfaceElement
{
public:
  /**
  * 
  */
  CLabel(CFontMetadata* pFont);

  /**
  * 
  */
  virtual ~CLabel();

  /**
  * 
  */
  virtual void Render();

  /**
  * 
  */
  virtual XUINT GetWidth()
  {
    return m_pFont->GetStringWidth(m_xText.c_str());
  }

  /**
  * 
  */
  virtual XUINT GetHeight()
  {
    return m_pFont->GetFontHeight();
  }

  /**
  * 
  */
  void SetText(const XCHAR* pText) 
  {
    m_xText = pText;
  }

  /**
  * 
  */
  const XCHAR* GetText() 
  {
    return m_xText.c_str();
  }

protected:
  // The element font.
  CFont* m_pFont;

  // The text string.
  XSTRING m_xText;
};

//##############################################################################

//##############################################################################
//
//                                   BUTTON
//
//##############################################################################
class CButton : public CInterfaceElement
{
public:
	// Callbacks.
	typedef void (*t_OnClickCallback)(XPOINT /*Offset*/);

  /**
  * 
  */
  CButton(CSpriteMetadata* pSprite, CFontMetadata* pFont = NULL);

  /**
  * 
  */
  virtual ~CButton();

  /**
  * 
  */
  virtual void Render();

	/**
	* 
	*/
	virtual XUINT GetWidth()
	{
		return m_iWidth + m_pAreas[m_iButtonState - 1]->xRect.GetWidth() + m_pAreas[m_iButtonState + 1]->xRect.GetWidth();
	}

	/**
	* 
	*/
	virtual XUINT GetHeight()
	{
		return m_pAreas[m_iButtonState]->xRect.GetHeight();
	}

  /**
  * 
  */
  void SetWidth(XUINT iWidth);

  /**
  * 
  */
  void SetInnerWidth(XUINT iWidth)
  {
    m_iWidth = iWidth;
  }

  /**
  * 
  */
  XUINT GetInnerWidth()
  {
    return m_iWidth;
  }

  /**
  * 
  */
  void SetClickCallback(t_OnClickCallback fpCallback)
  {
    m_fpOnClickCallback = fpCallback;
  }

  /**
  * 
  */
  void SetText(const XCHAR* pLabel) 
  {
    m_xLabel = pLabel;
  }

  /**
  * 
  */
  const XCHAR* GetLabel() 
  {
    return m_xLabel.c_str();
  }

protected:
  // Internal types.
  enum t_AreaIndex
  {
    AreaIndex_NormalLeft,
    AreaIndex_NormalMiddle,
    AreaIndex_NormalRight,
    AreaIndex_OverLeft,
    AreaIndex_OverMiddle,
    AreaIndex_OverRight,
    AreaIndex_DownLeft,
    AreaIndex_DownMiddle,
    AreaIndex_DownRight,
    /*MAX*/AreaIndex_Max,
  };

  /**
  * 
  */
  void InternalRender(XRECT& xRect, XPOINT xOffset);

  /**
  * 
  */
  virtual void OnMouseEnter() 
  {
    m_iButtonState = InterfaceManager.IsMouseDown() ? AreaIndex_DownMiddle : AreaIndex_OverMiddle;
  }

  /**
  * 
  */
  virtual void OnMouseLeave()
  {
    m_iButtonState = AreaIndex_NormalMiddle;
  }

  /**
  * 
  */
  virtual void OnMouseDown(XPOINT xPosition)
  {
    m_iButtonState = AreaIndex_DownMiddle;
  }

  /**
  * 
  */
  virtual void OnMouseUp(XPOINT xPosition) 
  {
    m_iButtonState = AreaIndex_OverMiddle;

    if (m_fpOnClickCallback)
      m_fpOnClickCallback(InterfaceManager.GetMousePosition() - GetPosition());
  }

  // The element sprite.
  CBasicSprite* m_pSprite;

  // The element font.
  CFont* m_pFont;

  // The element area list.
  CSpriteMetadata::CArea* m_pAreas[AreaIndex_Max];

  // The width, in pixels, of the button area.
  XUINT m_iWidth;

  // The button state.
  t_AreaIndex m_iButtonState;

  // The button label string.
  XSTRING m_xLabel;

  // The callback to execute if the button is clicked.
  t_OnClickCallback m_fpOnClickCallback;
};

//##############################################################################

//##############################################################################
//
//                                 INPUT BOX
//
//##############################################################################
class CInputBox : public CInterfaceElement
{
public:
	/**
	* 
	*/
	CInputBox(CSpriteMetadata* pMetadata, CFontMetadata* pFont);

	/**
	* 
	*/
	virtual ~CInputBox();

  /**
  * 
  */
  virtual void Update();

	/**
	* 
	*/
	virtual void Render();

  /**
  * 
  */
  virtual XUINT GetWidth()
  {
    return GetInnerWidth() + m_pAreas[AreaIndex_Left]->xRect.GetWidth() + m_pAreas[AreaIndex_Right]->xRect.GetWidth();
  }

  /**
  * 
  */
  virtual XUINT GetHeight()
  {
    return m_pAreas[AreaIndex_Middle]->xRect.GetHeight();
  }

  /**
  * 
  */
  void SetWidth(XUINT iWidth);

  /**
  * 
  */
  void SetInnerWidth(XUINT iWidth)
  {
    m_iWidth = iWidth;
  }

  /**
  * 
  */
  XUINT GetInnerWidth()
  {
    return m_iWidth;
  }

  /**
  * The total number of characters allowed in the input box.
  * Set this to 0 for unlimited length.
  */
  void SetCharLimit(XUINT iLimit)
  {
    m_iCharLimit = iLimit;
  }

  /**
  * 
  */
  XUINT GetCharLimit()
  {
    return m_iCharLimit;
  }

  /**
  * 
  */
  void SetText(const XCHAR* pText)
  {
    m_xText = pText;
  }

  /**
  * 
  */
  const XCHAR* GetText()
  {
    return m_xText.c_str();
  }

protected:
  /**
  * 
  */
  void InternalRender(XRECT& xRect, XPOINT xOffset);

  /**
  * 
  */
  virtual void OnFocus()
  {
    m_iFlashTimer = 0;
  }

  /**
  * 
  */
  virtual void OnMouseDown(XPOINT xPosition);

  /**
  * 
  */
  virtual void OnKeyChar(XINT iChar);

  /**
  * 
  */
  virtual void OnKeyDown(XUINT iVirtualKey);

	// Internal types.
	enum t_AreaIndex
	{
		AreaIndex_Left,
		AreaIndex_Middle,
		AreaIndex_Right,
		/*MAX*/AreaIndex_Max,
	};

	// The element sprite.
	CBasicSprite* m_pSprite;

  // The element font.
  CFont* m_pFont;

	// The element area list.
	CSpriteMetadata::CArea* m_pAreas[AreaIndex_Max];

  // The width, in pixels, of the input area.
  XUINT m_iWidth;

  // The total number of input characters allowed with zero being unlimited.
  XUINT m_iCharLimit;

  // The current input-box text.
  XSTRING m_xText;

  // The input cursor text character offset.
  XUINT m_iCharOffset;

  // The cursor flash timer.
  XUINT m_iFlashTimer;
};

//##############################################################################