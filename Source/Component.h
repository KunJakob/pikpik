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

// Global.
#include <Global.h>

// Other.
#include <Element.h>
#include <Sprite.h>
#include <Font.h>

//##############################################################################

// Shortcuts.
#define ComponentFactory CComponentFactory::Get()

// Allow the use of CreateWindow.
#if defined(CreateWindow)
	#undef CreateWindow
#endif

//##############################################################################
class CLabelComponent : public CLabelElement
{
public:
	// Initialise the element.
	CLabelComponent(CFontMetadata* pMetaFont);

	// Deinitialise the element.
	virtual ~CLabelComponent();

	// Render the element.
	virtual void OnRender();
};

//##############################################################################
class CHyperlinkComponent : public CLabelElement
{
public:
	// Callbacks.
	typedef xfunction(0)<> t_OnClickCallback;

	// Constructor.
	CHyperlinkComponent(CFontMetadata* pFont);

	// Destructor.
	virtual ~CHyperlinkComponent() {}

	// Render the element.
	virtual void OnRender();

	// Set the callback to execute if the hyperlink is clicked.
	void SetClickCallback(t_OnClickCallback fpCallback)
	{
		m_fpOnClickCallback = fpCallback;
	}

protected:
	// Called when the mouse is released when the element is active.
	virtual xbool OnMouseUp(xpoint xPosition);

	// The link callback.
	t_OnClickCallback m_fpOnClickCallback;
};

//##############################################################################
class CImageComponent : public CImageElement
{
public:
	// Initialise the element.
	CImageComponent(CSpriteMetadata* pMetaSprite);

	// Deinitialise the element.
	virtual ~CImageComponent();

	// Render the element.
	virtual void OnRender();
};

//##############################################################################
class CButtonComponent : public CStripElement
{
public:
	// Callbacks.
	typedef xfunction(2)<CButtonComponent* /*Button*/, xpoint /*Offset*/> t_OnClickCallback;

	// Initialise the button with the button image and label.
	CButtonComponent(CSpriteMetadata* pMetaSprite, CFontMetadata* pMetaFont = NULL);

	// Deinitialise the button and clean up any memory.
	virtual ~CButtonComponent();

	// Render the button.
	virtual void OnRender();

	// Get the height of the button.
	virtual xint GetHeight()
	{
		return m_pC[m_iButtonState]->m_xRect.GetHeight();
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
	virtual xbool OnMouseDown(xpoint xPosition);

	// Triggered when the left mouse-button is released within the element area when the element is active.
	virtual xbool OnMouseUp(xpoint xPosition);

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
class CInputComponent : public CStripElement
{
public:
	// Initialise the component.
	CInputComponent(CSpriteMetadata* pMetaSprite, CFontMetadata* pMetaFont);

	// Deinitialise the component.
	virtual ~CInputComponent();

	// Update the component.
	virtual void OnUpdate();

	// Render the component.
	virtual void OnRender();

	// Get the height of the button.
	virtual xint GetHeight()
	{
		return m_pC->m_xRect.GetHeight();
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
	virtual xbool OnMouseDown(xpoint xPosition);

	// Triggered when a keyboard key is pressed whilst the element is in focus.
	virtual xbool OnKeyDown(xint iVirtualKey);

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
class CProgressComponent : public CStripElement
{
public:
	// Initialise the component.
	CProgressComponent(CSpriteMetadata* pMetaSprite);

	// Deinitialise the component.
	virtual ~CProgressComponent();

	// Render the component.
	virtual void OnRender();

	// Get the height of the button.
	virtual xint GetHeight()
	{
		return m_pC->m_xRect.GetHeight();
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
class CWindowComponent : public CContainerElement
{
public:
	// Initialise the window using a specific graphic.
	CWindowComponent(CSpriteMetadata* pMetaSprite, CFontMetadata* pMetaFont = NULL);

	// Deinitialise the window and clean up any memory.
	virtual ~CWindowComponent();

	// Render the button.
	virtual void OnRender();

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
	virtual xbool OnMouseDown(xpoint xPosition);

	// Triggered when the left mouse-button is released within the element area.
	virtual xbool OnMouseUp(xpoint xPosition);

	// Triggered when the mouse is moved within the element area.
	virtual void OnMouseMove(xpoint xDifference, xbool bMouseDown);

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
class CGroupComponent : public CContainerElement
{
public:
	// Initialise the element.
	CGroupComponent(CSpriteMetadata* pMetaSprite, CFontMetadata* pMetaFont = NULL);

	// Deinitialise the element.
	virtual ~CGroupComponent();

	// Render the button.
	virtual void OnRender();

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
class CListComponent : public CContainerElement
{
};

//##############################################################################
class CCheckComponent : public CCheckElement
{
public:
	// Callbacks
	typedef xfunction(2)<CCheckComponent* /*Component*/, xbool /*Checked*/> t_OnCheckCallback;
	typedef xfunction(1)<xbool /*Checked*/> t_CheckBinding;

	// Initialise the element.
	CCheckComponent(CSpriteMetadata* pMetaSprite, CFontMetadata* pMetaFont = NULL);

	// Deinitialise the element.
	virtual ~CCheckComponent();

	// Render the element.
	virtual void OnRender();

	// Get the width of the element.
	virtual xint GetWidth()
	{
		if (m_pFont && m_xText.length())
			return m_pBox[m_iCheckState]->m_xRect.GetWidth() + (m_pBox[m_iCheckState]->m_xRect.GetWidth() / 2) + m_pFont->GetStringWidth(GetText());
		else
			return m_pBox[m_iCheckState]->m_xRect.GetWidth();
	}

	// Get the height of the element.
	virtual xint GetHeight()
	{
		return m_pBox[m_iCheckState]->m_xRect.GetHeight();
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

	// Set the check-state binding function.
	inline void SetCheckBinding(t_CheckBinding fpBinding)
	{
		m_fpCheckBinding = fpBinding;
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
	virtual xbool OnMouseDown(xpoint xPosition);

	// Triggered when the left mouse-button is released within the element area.
	virtual xbool OnMouseUp(xpoint xPosition);

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

	// The binding to allow direct binding to another function passing in the checked state.
	t_CheckBinding m_fpCheckBinding;
};

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
	virtual xbool OnMouseUp(xpoint xPosition);

	// The radio component group.
	xint m_iRadioGroup;
};

//##############################################################################
class CComponentFactory 
{
public:
	// Singleton instance.
	static inline CComponentFactory& Get() 
	{
		static CComponentFactory s_Instance;
		return s_Instance;
	}

	// Register a metadata file with the system for later use.
	//void RegisterMetadata(CMetadata* pMetadata);

	// Remove a metadata file from the system.
	//void DeregisterMetadata(CMetadata* pMetadata);

	// Creates a new window from metadata and also instantiates all child elements.
	CWindowComponent* CreateWindow(CMetadata* pMetadata, const char* pName);

protected:
	// Get the basic properties that are valid for all components.
	void GetBasicProperties(CInterfaceElement* pElement, CDataset* pDataset);

	// Get the generic properties for label elements.
	void GetLabelProperties(CLabelElement* pElement, CDataset* pDataset);

	// Get the generic properties for image elements.
	void GetImageProperties(CImageElement* pElement, CDataset* pDataset);

	// Get the generic properties for strip elements.
	void GetStripProperties(CStripElement* pElement, CDataset* pDataset);

	// Get the generic properties for container elements.
	void GetContainerProperties(CContainerElement* pElement, CDataset* pDataset);

	// Get the generic properties for check elements.
	void GetCheckProperties(CCheckElement* pElement, CDataset* pDataset);

	// Create and attach any child elements to the specified element.
	void AttachChildren(CInterfaceElement* pElement, CDataset* pDataset);

	// Create a label component from metadata.
	CLabelComponent* CreateLabel(CDataset* pDataset);

	// Create a hyperlink component from metadata.
	CHyperlinkComponent* CreateHyperlink(CDataset* pDataset);

	// Create a image component from metadata.
	CImageComponent* CreateImage(CDataset* pDataset);

	// Create a button component from metadata.
	CButtonComponent* CreateButton(CDataset* pDataset);

	// Create a input component from metadata.
	CInputComponent* CreateInput(CDataset* pDataset);

	// Create a progress component from metadata.
	CProgressComponent* CreateProgress(CDataset* pDataset);

	// Create a window component from metadata.
	CWindowComponent* CreateWindow(CDataset* pDataset);

	// Create a group component from metadata.
	CGroupComponent* CreateGroup(CDataset* pDataset);

	// Create a list component from metadata.
	CListComponent* CreateList(CDataset* pDataset);

	// Create a check component from metadata.
	CCheckComponent* CreateCheck(CDataset* pDataset);

	// Create a radio component from metadata.
	CRadioComponent* CreateRadio(CDataset* pDataset);
};

//##############################################################################

//