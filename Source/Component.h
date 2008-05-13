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
  CButtonComponent(CSpriteMetadata* pSprite, CFontMetadata* pFont = NULL);

  // Deinitialise the button and clean up any memory.
  virtual ~CButtonComponent();

  // Render the button.
  virtual void Render();

  // Get the height of the button.
  virtual xint GetHeight()
  {
    return m_pCentre[m_iButtonState]->xRect.Height();
  }

	// Set the button text label.
	inline void SetText(const xchar* pText)
	{
		m_xText = pText;
	}

	// Get the button text label.
	inline const xchar* GetText()
	{
		return m_xText.c_str();
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

	// The button font.
	CFont* m_pFont;

	// The button string.
	xstring m_xText;

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
		return m_pCentre->xRect.Height();
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
	inline void SetCharacterLimit(xint iLimit)
	{
		m_iCharLimit = iLimit;
	}

	// Get the maximum number of characters allowed in the input box.
	inline xint GetCharacterLimit()
	{
		return m_iCharLimit;
	}

	// Set the text in the input box.
	inline void SetText(const xchar* pText)
	{
		m_xText = pText;
	}

	// Get the text in the input box.
	inline const xchar* GetText()
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
	CWindowComponent(CSpriteMetadata* pSprite, CFontMetadata* pFont = NULL);

  // Deinitialise the button and clean up any memory.
  virtual ~CWindowComponent();

  // Render the button.
  virtual void Render();

	// Set the window title text.
	inline void SetTitle(const xchar* pTitle)
	{
		m_xTitle = pTitle;
	}

	// Get the current window title text.
	inline const xchar* GetTitle()
	{
		return m_xTitle.c_str();
	}

	// Allow or prevent the window from being draggable.
	inline void SetMoveable(xbool bMoveable)
	{
		m_bMoveable = bMoveable;
	}

	// Determine if the window is draggable.
	inline xbool IsMoveable()
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

	// The font for the window title.
	CFont* m_pFont;

	// The window title.
	xstring m_xTitle;

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
public:
	// Initialise the element.
	CCheckComponent(CSpriteMetadata* pSprite, CLabelComponent* pLabel = NULL);

	// Deinitialise the element.
	virtual ~CCheckComponent();

	// Render the element.
	virtual void Render();

	// Get the width of the element.
	virtual xint GetWidth()
	{
		return m_pBox[m_iCheckState]->xRect.Width();
	}

	// Get the height of the element.
	virtual xint GetHeight()
	{
		return m_pBox[m_iCheckState]->xRect.Height();
	}

protected:
	// Triggered when the mouse enters the element.
	virtual void OnMouseEnter() 
	{
		m_iCheckState = InterfaceManager.IsMouseDown() ? CheckState_Down : CheckState_Over;
	}

	// Triggered when the mouse leaves the element.
	virtual void OnMouseLeave()
	{
		m_iCheckState = CheckState_Normal;
	}

	// Triggered when the left mouse-button is pressed within the element area.
	virtual void OnMouseDown(xpoint xPosition)
	{
		m_iCheckState = CheckState_Down;
	}

	// Triggered when the left mouse-button is released within the element area.
	virtual void OnMouseUp(xpoint xPosition) 
	{
		m_bChecked = !m_bChecked;
		m_iCheckState = CheckState_Over;
	}

	// The check states.
	enum t_CheckState
	{
		CheckState_Normal,
		CheckState_Over,
		CheckState_Down,
		/*MAX*/CheckState_Max,
	};

	// The element areas.
	CSpriteMetadata::CArea* m_pBox[CheckState_Max];
	CSpriteMetadata::CArea* m_pCheck;

	// The internal check state.
	t_CheckState m_iCheckState;
};

//##############################################################################
//
//                                RADIO BUTTON
//
//##############################################################################
class CRadioComponent : public CCheckComponent
{
public:
	// Initialise the element.
	CRadioComponent(xint iRadioGroup, CSpriteMetadata* pSprite, CLabelComponent* pLabel = NULL);

	// Deinitialise the element.
	virtual ~CRadioComponent();

	// Get the radio group this component belongs to.
	inline xint GetRadioGroup()
	{
		return m_iRadioGroup;
	}

protected:
	// Triggered when the left mouse-button is released within the element area.
	virtual void OnMouseUp(xpoint xPosition);

	// The radio component group.
	xint m_iRadioGroup;
};

//##############################################################################
//
//                                 SCROLL BAR
//
//##############################################################################
class CScrollComponent : public CInterfaceElement
{
};