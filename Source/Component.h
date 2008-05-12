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
class CLabelComponent : public CLabelElement
{
};

//##############################################################################
//
//                                   IMAGE
//
//##############################################################################
class CImageComponent : public CImageElement
{
};

//##############################################################################
//
//                                   BUTTON
//
//##############################################################################
class CButtonComponent : public CRowElement
{
public:
  // Callbacks.
  typedef void (*t_OnClickCallback)(xpoint /*Offset*/);

  // Initialise the button with the button image and label.
  CButtonComponent(CSpriteMetadata* pSprite, CLabelElement* pLabel = NULL);

  // Deinitialise the button and clean up any memory.
  virtual ~CButtonComponent();

  // Render the button.
  virtual void Render();

  // Get the height of the button.
  virtual xint GetHeight()
  {
    return m_pCentre[m_iButtonState]->xRect.GetHeight();
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

  // Triggered when the left mouse-button is pressed within the element area when the element is active.
  virtual void OnMouseDown(xpoint xPosition)
  {
    m_iButtonState = ButtonState_Down;
  }

  // Triggered when the left mouse-button is released within the element area when the element is active.
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
		/*MAX*/ButtonState_Max,
  };

	// Element areas.
	CSpriteMetadata::CArea* m_pLeft[ButtonState_Max];
	CSpriteMetadata::CArea* m_pCentre[ButtonState_Max];
	CSpriteMetadata::CArea* m_pRight[ButtonState_Max];

  // The internal button state.
  t_ButtonState m_iButtonState;

  // The button text font.
  CLabelElement* m_pLabel;

  // The callback to execute if the button is clicked.
  t_OnClickCallback m_fpOnClickCallback;
};

//##############################################################################
//
//                                  TEXT BOX
//
//##############################################################################
class CInputComponent : public CRowElement
{
public:
	// Initialise the component.
	CInputComponent(CSpriteMetadata* pSprite, CFontMetadata* pFont);

	// Deinitialise the component.
	virtual ~CInputComponent();

	// Update the component.
	virtual void Update();

	// Render the component.
	virtual void Render();

	// Get the height of the button.
	virtual xint GetHeight()
	{
		return m_pCentre->xRect.GetHeight();
	}

	// Specify if the input should be masked.
	inline void SetMasked(xbool bMasked)
	{
		m_bMasked = bMasked;
	}

	// Determine if the input is masked.
	inline xbool IsMasked()
	{
		return m_bMasked;
	}

	// Set the maximum number of characters allowed in the input box.
	void SetCharacterLimit(xint iLimit)
	{
		m_iCharLimit = iLimit;
	}

	// Get the maximum number of characters allowed in the input box.
	XUINT GetCharacterLimit()
	{
		return m_iCharLimit;
	}

	// Set the text in the input box.
	void SetText(const xchar* pText)
	{
		m_xText = pText;
	}

	// Get the text in the input box.
	const xchar* GetText()
	{
		return m_xText.c_str();
	}

protected:
	// Triggered when focus is applied to the element.
	virtual void OnFocus()
	{
		m_iFlashTimer = 0;
	}
	
	// Triggered when the left mouse-button is pressed within the element area.
	virtual void OnMouseDown(xpoint xPosition);

	// Triggered when a keyboard key is pressed whilst the element is in focus.
	virtual void OnKeyDown(xint iVirtualKey);

	// Triggered when an ASCII key is input on the keyboard whilst the element is in focus.
	virtual void OnKeyChar(xchar cChar);

	// Element areas.
	CSpriteMetadata::CArea* m_pLeft;
	CSpriteMetadata::CArea* m_pCentre;
	CSpriteMetadata::CArea* m_pRight;

	// The text area font.
	CFont* m_pFont;

	// The input text string.
	xstring m_xText;

	// Determines if the input field has masked input.
	xbool m_bMasked;

	// The total number of input characters allowed with zero being unlimited.
	xint m_iCharLimit;

	// The input cursor text character offset.
	xint m_iCharOffset;

	// The cursor flash timer.
	xint m_iFlashTimer;
};

//##############################################################################
//
//                                PROGRESS BAR
//
//##############################################################################
class CProgressComponent : public CRowElement
{
};

//##############################################################################
//
//                                   WINDOW
//
//##############################################################################
class CWindowComponent : public CContainerElement
{
public:
  // Initialise the window using a specific graphic.
	CWindowComponent(CSpriteMetadata* pSprite, CLabelElement* pLabel = NULL);

  // Deinitialise the button and clean up any memory.
  virtual ~CWindowComponent();

  // Render the button.
  virtual void Render();

	// Allow or prevent the window from being draggable.
	void SetMoveable(xbool bMoveable)
	{
		m_bMoveable = bMoveable;
	}

	// Determine if the window is draggable.
	xbool IsMoveable()
	{
		return m_bMoveable;
	}

protected:
	// Triggered when the left mouse-button is pressed within the element area.
	virtual void OnMouseDown(xpoint xPosition) 
	{
		if (m_bMoveable && Math::Intersect(xPosition, xrect(m_xFrameSize.iLeft, 0, GetInnerWidth(), m_xFrameSize.iTop) + GetPosition()))
			m_bDragging = true;
	}

	// Triggered when the left mouse-button is released within the element area.
	virtual void OnMouseUp(xpoint xPosition) 
	{
		m_bDragging = false;
	}

	// Triggered when the mouse is moved within the element area.
	virtual void OnMouseMove(xpoint xDifference, xbool bMouseDown)
	{
		if (m_bDragging)
			Move(xDifference);
	}

	// Element areas.
	CSpriteMetadata::CArea* m_pTL;
	CSpriteMetadata::CArea* m_pTC;
	CSpriteMetadata::CArea* m_pTR;
	CSpriteMetadata::CArea* m_pML;
	CSpriteMetadata::CArea* m_pMC;
	CSpriteMetadata::CArea* m_pMR;
	CSpriteMetadata::CArea* m_pBL;
	CSpriteMetadata::CArea* m_pBC;
	CSpriteMetadata::CArea* m_pBR;

	// The label element for the window title.
	CLabelElement* m_pLabel;

	// Specifies if the container can be dragged around the screen.
	xbool m_bMoveable;

	// Specifies if the container is being dragged.
	xbool m_bDragging;
};

//##############################################################################
//
//                                  LIST BOX
//
//##############################################################################
class CListComponent : public CContainerElement
{
};

//##############################################################################
//
//                                 CHECK BOX
//
//##############################################################################
class CCheckComponent : public CCheckElement
{
};

//##############################################################################
//
//                                RADIO BUTTON
//
//##############################################################################
class CRadioComponent : public CCheckElement
{
};

//##############################################################################
//
//                                 SCROLL BAR
//
//##############################################################################
class CScrollComponent : public CInterfaceElement
{
};