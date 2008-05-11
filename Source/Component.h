#pragma once

/**
* @file Component.h
* @author Nat Ryall
* @date 10/05/2008
* @brief Interface element set that can be instantiated.
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
#include <Element.h>
#include <Sprite.h>
#include <Font.h>

//##############################################################################
//
//                                   LABEL
//
//##############################################################################
class CLabel : public CLabelElement
{
};

//##############################################################################
//
//                                 IMAGE BOX
//
//##############################################################################
class CImageBox : public CImageElement
{
};

//##############################################################################
//
//                                   BUTTON
//
//##############################################################################
class CButton : public CRowElement
{
public:
  // Callbacks.
  typedef void (*t_OnClickCallback)(xpoint /*Offset*/);

  // Initialise the button with the button image and font (optional).
  CButton(CSpriteMetadata* pSprite, CFontMetadata* pFont = NULL);

  // Deinitialise the button and clean up any memory.
  virtual ~CButton();

  // Render the button.
  virtual void Render();

  // Get the height of the button.
  virtual xint GetHeight()
  {
    return m_pNM->xRect.GetHeight();
  }

  // Set the callback to execute if the button is clicked.
  void SetClickCallback(t_OnClickCallback fpCallback)
  {
    m_fpOnClickCallback = fpCallback;
  }

protected:
  // Triggered when the mouse enters this element.
  virtual void OnMouseEnter() 
  {
    m_iButtonState = InterfaceManager.IsMouseDown() ? ButtonState_Down : ButtonState_Over;
  }

  // Triggered when the mouse leaves this element.
  virtual void OnMouseLeave()
  {
    m_iButtonState = ButtonState_Normal;
  }

  // Called when the left mouse-button is pressed over the element.
  virtual void OnMouseDown(xpoint xPosition)
  {
    m_iButtonState = ButtonState_Down;
  }

  // Called when the left mouse-button is released over the element.
  virtual void OnMouseUp(xpoint xPosition) 
  {
    m_iButtonState = ButtonState_Over;

    if (m_fpOnClickCallback)
      m_fpOnClickCallback(InterfaceManager.GetMousePosition() - GetPosition());
  }

  // The button states.
  enum t_ButtonState
  {
    ButtonState_Normal,
    ButtonState_Over,
    ButtonState_Down,
  };

  // The internal button state.
  t_ButtonState m_iButtonState;

  // The button text font.
  CFont* m_pFont;

  // Element areas.
  CSpriteMetadata::CArea* m_pNL;
  CSpriteMetadata::CArea* m_pNM;
  CSpriteMetadata::CArea* m_pNR;
  CSpriteMetadata::CArea* m_pOL;
  CSpriteMetadata::CArea* m_pOM;
  CSpriteMetadata::CArea* m_pOR;
  CSpriteMetadata::CArea* m_pDL;
  CSpriteMetadata::CArea* m_pDM;
  CSpriteMetadata::CArea* m_pDR;

  // The callback to execute if the button is clicked.
  t_OnClickCallback m_fpOnClickCallback;
};

//##############################################################################
//
//                                  TEXT BOX
//
//##############################################################################
class CTextBox : public CRowElement
{
};

//##############################################################################
//
//                                PROGRESS BAR
//
//##############################################################################
class CProgressBar : public CRowElement
{
};

//##############################################################################
//
//                                   WINDOW
//
//##############################################################################
class CWindow : public CContainerElement
{
};

//##############################################################################
//
//                                  LIST BOX
//
//##############################################################################
class CListBox : public CContainerElement
{
};

//##############################################################################
//
//                                 CHECK BOX
//
//##############################################################################
class CCheckBox : public CCheckElement
{
};

//##############################################################################
//
//                                RADIO BUTTON
//
//##############################################################################
class CRadioButton : public CCheckElement
{
};

//##############################################################################
//
//                                 SCROLL BAR
//
//##############################################################################
class CScrollBar : public CInterfaceElement
{
};