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
//                                   TYPES
//
//##############################################################################

// Predeclare.
class CInterfaceElement;

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
typedef XVLIST<CInterfaceElement*> t_ElementList;

//##############################################################################

//##############################################################################
//
//                             INTERFACE MANAGER
//
//##############################################################################

//##############################################################################

//##############################################################################
//
//                             INTERFACE ELEMENT
//
//##############################################################################
class CInterfaceElement
{
public:
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
  XRECT GetArea()
  {
    return m_xArea;
  }

  /**
  * 
  */
  void SetPosition(XPOINT xPosition)
  {
    m_xPosition = xPosition;
  }

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
  void Attach(CInterfaceElement* pElement);
  
  /**
  * 
  */
  void Detach(CInterfaceElement* pElement);

  /**
  * 
  */
  XUINT GetAttachedCount()
  {
    return (XUINT)m_lpChildElements.size();
  }

  /**
  * 
  */
  CInterfaceElement* GetAttached(XUINT iIndex)
  {
    return m_lpChildElements[iIndex];
  }

  /**
  * 
  */
  virtual void OnFocus() {}
  virtual void OnBlur() {}

  /**
  * 
  */
  virtual void OnMouseEnter() {}
  virtual void OnMouseLeave() {}
  virtual void OnMouseOver(XPOINT xLocalPos) {}
  virtual void OnMouseDown(XPOINT xLocalPos) {}
  virtual void OnMouseUp(XPOINT xLocalPos) {}

  /**
  * 
  */
  virtual void OnKeyDown(XUINT iVirtualKey, XBOOL bShift) {}
  virtual void OnKeyUp(XUINT iVirtualKey, XBOOL bShift) {}
  virtual void OnKeyHold(XUINT iVirtualKey, XBOOL bShift) {}

protected:
  /**
  * 
  */
  CInterfaceElement(t_ElementType iType);

  /**
  * 
  */
  static void SetFocus(CInterfaceElement* pElement);
  
  // The currently focused element.
  static CInterfaceElement* s_pFocusedElement;

  // The parent element or NULL if top level.
  CInterfaceElement* m_pParent;

  // The type of the element.
  t_ElementType m_iType;

  // Specifies if the element is visible.
  XBOOL m_bVisible;

  // Specifies if the element is enabled and will accept events.
  XBOOL m_bEnabled;

  // The relative position of the element to the parent.
  XPOINT m_xPosition;

  // The element area for collision and event purposes.
  XRECT m_xArea;

  // A list of child elements.
  t_ElementList m_lpChildElements;
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
  XUINT GetWidth()
  {
    return m_iWidth;
  }

  /**
  * 
  */
  XUINT GetHeight()
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
  XUINT GetWindowWidth()
  {
    return GetWidth() + m_pAreas[AreaIndex_MiddleLeft]->xRect.GetWidth() + m_pAreas[AreaIndex_MiddleRight]->xRect.GetWidth();
  }

  /**
  * 
  */
  XUINT GetWindowHeight()
  {
    return GetHeight()  + m_pAreas[AreaIndex_TopMiddle]->xRect.GetHeight() + m_pAreas[AreaIndex_BottomMiddle]->xRect.GetHeight();
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
  t_AreaIndex iButtonState;

  // The button label.
  CLabel* m_pLabel;

  // The callback to execute if the button is clicked.
  //m_fpClickCallback
};

//##############################################################################