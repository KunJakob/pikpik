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
public:
	// Initialise the element.
	CLabelComponent(CFontMetadata* pMetaFont);

	// Deinitialise the element.
	virtual ~CLabelComponent();

	// Render the element.
	virtual void Render();
};

//##############################################################################
//
//                                   IMAGE
//
//##############################################################################
class CImageComponent : public CImageElement
{
public:
	// Initialise the element.
	CImageComponent(CSpriteMetadata* pMetaSprite);

	// Deinitialise the element.
	virtual ~CImageComponent();

	// Render the element.
	virtual void Render();
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
	typedef xfunction(2)<CButtonComponent* /*Button*/, xpoint /*Offset*/> t_OnClickCallback;

	// Initialise the button with the button image and label.
	CButtonComponent(CSpriteMetadata* pMetaSprite, CFontMetadata* pMetaFont = NULL);

	// Deinitialise the button and clean up any memory.
	virtual ~CButtonComponent();

	// Render the button.
	virtual void Render();

	// Get the height of the button.
	virtual xint GetHeight()
	{
		return m_pC[m_iButtonState]->xRect.Height();
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
			m_fpOnClickCallback(this, InterfaceManager.GetMousePosition() - GetPosition());
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
	CSpriteMetadata::CArea* m_pL[ButtonState_Max];
	CSpriteMetadata::CArea* m_pC[ButtonState_Max];
	CSpriteMetadata::CArea* m_pR[ButtonState_Max];

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
	CInputComponent(CSpriteMetadata* pMetaSprite, CFontMetadata* pMetaFont);

	// Deinitialise the component.
	virtual ~CInputComponent();

	// Update the component.
	virtual void Update();

	// Render the component.
	virtual void Render();

	// Get the height of the button.
	virtual xint GetHeight()
	{
		return m_pC->xRect.Height();
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
	CSpriteMetadata::CArea* m_pL;
	CSpriteMetadata::CArea* m_pC;
	CSpriteMetadata::CArea* m_pR;

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
public:
	// Initialise the component.
	CProgressComponent(CSpriteMetadata* pMetaSprite);

	// Deinitialise the component.
	virtual ~CProgressComponent();

	// Render the component.
	virtual void Render();

	// Get the height of the button.
	virtual xint GetHeight()
	{
		return m_pC->xRect.Height();
	}

	// Set the progress percentage in the range range 0 to 1.
	inline void SetProgress(xfloat fPercent)
	{
		m_fProgress = Math::Clamp(fPercent, 0.f, 1.f);
	}

	// Get the progress percentage in the range range 0 to 1.
	inline xfloat GetProgress()
	{
		return m_fProgress;
	}

protected:
	// Element areas.
	CSpriteMetadata::CArea* m_pL;
	CSpriteMetadata::CArea* m_pC;
	CSpriteMetadata::CArea* m_pR;
	CSpriteMetadata::CArea* m_pProgress;

	// The progress bar progress in the range 0 to 1.
	xfloat m_fProgress;
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
	CWindowComponent(CSpriteMetadata* pMetaSprite, CFontMetadata* pMetaFont = NULL);

	// Deinitialise the window and clean up any memory.
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
//                                 GROUP BOX
//
//##############################################################################
class CGroupComponent : public CContainerElement
{
public:
	// Initialise the element.
	CGroupComponent(CSpriteMetadata* pMetaSprite, CFontMetadata* pMetaFont = NULL);

	// Deinitialise the element.
	virtual ~CGroupComponent();

	// Render the button.
	virtual void Render();

	// Set the group title text.
	inline void SetTitle(const xchar* pTitle)
	{
		m_xTitle = pTitle;
	}

	// Get the current group title text.
	inline const xchar* GetTitle()
	{
		return m_xTitle.c_str();
	}

protected:
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

	// The font for the group title.
	CFont* m_pFont;

	// The group title.
	xstring m_xTitle;
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
	// Callbacks
	typedef xfunction(2)<CCheckComponent* /*Component*/, xbool /*Checked*/> t_OnCheckCallback;

	// Initialise the element.
	CCheckComponent(CSpriteMetadata* pMetaSprite, CFontMetadata* pMetaFont = NULL);

	// Deinitialise the element.
	virtual ~CCheckComponent();

	// Render the element.
	virtual void Render();

	// Get the width of the element.
	virtual xint GetWidth()
	{
		if (m_pFont && m_xText.length())
			return m_pBox[m_iCheckState]->xRect.Width() + (m_pBox[m_iCheckState]->xRect.Width() / 2) + m_pFont->GetStringWidth(GetText());
		else
			return m_pBox[m_iCheckState]->xRect.Width();
	}

	// Get the height of the element.
	virtual xint GetHeight()
	{
		return m_pBox[m_iCheckState]->xRect.Height();
	}

	// Set the text string to render.
	inline void SetText(const xchar* pString)
	{
		m_xText = pString;
	}

	// Get the text string to be rendered.
	inline const xchar* GetText()
	{
		return m_xText.c_str();
	}

	// Set the on check, on uncheck callback.
	inline void SetCheckCallback(t_OnCheckCallback fpCallback)
	{
		m_fpOnCheckCallback = fpCallback;
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
	virtual void OnMouseUp(xpoint xPosition);

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

	// The element font.
	CFont* m_pFont;

	// The element text string.
	xstring m_xText;

	// The callback to execute when the check component is checked or unchecked.
	t_OnCheckCallback m_fpOnCheckCallback; 
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
	CRadioComponent(xint iRadioGroup, CSpriteMetadata* pMetaSprite, CFontMetadata* pFont = NULL);

	// Deinitialise the element.
	virtual ~CRadioComponent();

	// Get the radio group this component belongs to.
	inline xint GetRadioGroup()
	{
		return m_iRadioGroup;
	}

	// Get the currently selected component for the specified radio group or NULL if no component is selected.
	static CRadioComponent* GetChecked(xint iRadioGroup);

protected:
	// Triggered when the left mouse-button is released within the element area.
	virtual void OnMouseUp(xpoint xPosition);

	// The radio component group.
	xint m_iRadioGroup;
};