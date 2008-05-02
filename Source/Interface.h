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
  ElementType_Abstract,
  ElementType_Container,
  ElementType_Dialog,
  ElementType_Button,
  ElementType_InputBox,
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

  // The current mouse position.
  XPOINT m_xMousePos;

  // The last mouse position.
  XPOINT m_xLastMousePos;

  // The active element the mouse is over.
  CInterfaceElement* m_pActiveElement;

	// The last focused element.
	CInterfaceElement* m_pFocusedElement;
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
  /*XUINT GetAttachedCount()
  {
    return (XUINT)m_lpChildElements.size();
  }*/

  /**
  * 
  */
  /*CInterfaceElement* GetAttached(XUINT iIndex)
  {
    return m_lpChildElements[iIndex];
  }*/

	/**
	* 
	*/
	void ToFront();

  /**
  * Fired on click/tab or manually with SetFocus().
  */
  virtual void OnFocus() {}
  virtual void OnBlur() {}

  /**
  * Fired always.
  */
  virtual void OnMouseEnter(XBOOL bActive) {}
  virtual void OnMouseLeave() {}

	/**
	* Fired when active.
	*/
	virtual void OnMouseMove(XPOINT xDifference) {}
  virtual void OnMouseDown(XPOINT xPosition) {}
  virtual void OnMouseUp(XPOINT xPosition) {}

  /**
  * Fired when in focus.
  */
  virtual void OnKeyDown(XUINT iVirtualKey, XBOOL bShift) {}
  virtual void OnKeyUp(XUINT iVirtualKey, XBOOL bShift) {}
  virtual void OnKeyHold(XUINT iVirtualKey, XBOOL bShift) {}

protected:
  /**
  * 
  */
  CInterfaceElement(t_ElementType iType);

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
class CWindow : public CContainer
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
	* 
	*/
	XUINT GetInnerWidth()
	{
		return CContainer::GetWidth();
	}

	/**
	* 
	*/
	XUINT GetInnerHeight()
	{
		return CContainer::GetHeight();
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

  // The element sprite.
  CBasicSprite* m_pSprite;

  // The element area list.
  CSpriteMetadata::CArea* m_pAreas[AreaIndex_Max];

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
protected:
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
  CButton(CSpriteMetadata* pMetadata);

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
		return m_pAreas[m_iButtonState]->xRect.GetWidth();
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
  void SetClickCallback(void* pCallback)
  {
  }

  /**
  * 
  */
  void SetText(const XCHAR* pButtonText) 
  {
  }

  /**
  * 
  */
  const XCHAR* GetText() 
  {
    return NULL;
  }

  /**
  * 
  */
  virtual void OnMouseEnter(XBOOL bActive) 
  {
		if (bActive)
			m_iButtonState = InterfaceManager.IsMouseDown() ? AreaIndex_Down : AreaIndex_Over;
  }

  /**
  * 
  */
  virtual void OnMouseLeave()
  {
    m_iButtonState = AreaIndex_Normal;
  }

  /**
  * 
  */
  virtual void OnMouseDown(XPOINT xPosition)
  {
    m_iButtonState = AreaIndex_Down;
  }

  /**
  * 
  */
  virtual void OnMouseUp(XPOINT xPosition) 
  {
    m_iButtonState = AreaIndex_Over;

		if (m_fpOnClickCallback)
			m_fpOnClickCallback(InterfaceManager.GetMousePosition() - GetPosition());
  }

protected:
  // Internal types.
  enum t_AreaIndex
  {
    AreaIndex_Normal,
    AreaIndex_Over,
    AreaIndex_Down,
    /*MAX*/AreaIndex_Max,
  };

  // The element sprite.
  CBasicSprite* m_pSprite;

  // The element area list.
  CSpriteMetadata::CArea* m_pAreas[AreaIndex_Max];

  // The button state.
  t_AreaIndex m_iButtonState;

  // The button label.
  CLabel* m_pLabel;

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
	CInputBox(CSpriteMetadata* pMetadata);

	/**
	* 
	*/
	virtual ~CInputBox();

	/**
	* 
	*/
	virtual void Render();

protected:
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

	// The element area list.
	CSpriteMetadata::CArea* m_pAreas[AreaIndex_Max];
};

//##############################################################################