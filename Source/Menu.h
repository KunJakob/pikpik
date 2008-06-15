#pragma once

/**
* @file Menu.h
* @author Nat Ryall
* @date 11/04/2008
* @brief Manages menu transitions.
*
* Copyright © SAPIAN
*/

#pragma region Include
//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Global.
#include <Global.h>

// Other.
#include <Sprite.h>
#include <Background.h>
#include <Font.h>
#include <Interface.h>
#include <Component.h>
#include <Lobby.h>

//##############################################################################
#pragma endregion

#pragma region Types
//##############################################################################
//
//                                   TYPES
//
//##############################################################################

// Predeclare.
class CMenuScreen;
class CMenuLink;

// The different group types.
enum t_MenuGroupIndex
{
	MenuGroupIndex_None = -1,
	MenuGroupIndex_Main,
	MenuGroupIndex_Play,
	MenuGroupIndex_Online,
	/*MAX*/MenuGroupIndex_Max,
};

// The different transition states.
enum t_MenuElementState
{
	MenuElementState_None = -1,
	MenuElementState_In,
	MenuElementState_Normal,
	MenuElementState_Out,
};

// Lists.
typedef XLIST<CMenuLink*> t_MenuLinkList;

//##############################################################################
#pragma endregion

#pragma region Elements
//##############################################################################
//
//                                  ELEMENTS
//
//##############################################################################

// Text link element.
class CMenuLink : public CInterfaceElement
{
public:
	// Friends.
	friend CMenuScreen;

	// Callbacks.
	typedef xfunction(0)<> t_fpLinkSelectedCallback;

	/**
	* Constructor.
	*/
	CMenuLink(XUINT iGroupIndex, CFontMetadata* pFont, const XCHAR* pText, t_fpLinkSelectedCallback fpCallback = NULL);

	/**
	* Destructor.
	*/
	virtual ~CMenuLink();

	/**
	* Update the element.
	*/
	virtual void Update();

	/**
	* Render the element.
	*/
	virtual void Render();

	/**
	* Get the element width in pixels.
	*/
	virtual xint GetWidth()
	{
		return m_pText->GetStringWidth();
	}

	/**
	* Get the element height in pixels.
	*/
	virtual xint GetHeight()
	{
		return m_pText->GetFontHeight();
	}

protected:
	/**
	* Re-position the menu link in relation to other links.
	*/
	void RePosition(xuint iElementIndex, xuint iNumElements);

	/**
	* Called when the mouse is released when the element is active.
	*/
	virtual void OnMouseUp(xpoint xPosition)
	{
		if (m_fpLinkSelectedCallback)
			m_fpLinkSelectedCallback();
	}

	// The group index of the element. This us used to enable/disable elements in groups.
	xuint m_iGroupIndex;

	// The text to render for the link.
	CText* m_pText;

	// The link callback.
	t_fpLinkSelectedCallback m_fpLinkSelectedCallback;
};

//##############################################################################
#pragma endregion

#pragma region Declaration
//##############################################################################
//
//                                 DECLARATION
//
//##############################################################################
class CMenuScreen : public CScreen
{
public:
	CMenuScreen() : CScreen(ScreenIndex_MenuScreen) {}

	/**
	* Called when the screen is registered in the constructor.
	*/
	virtual void Load();

	/**
	* Called when the state is dismissed in the destructor.
	*/
	virtual void Unload();

	/**
	* Called once when the screen is placed at the top of the stack.
	*/
	virtual void Wake();

	/**
	* Called once when the screen goes out of scope either through destruction or another screen is placed above this on the stack.
	*/
	virtual void Sleep();

	/**
	* Called each frame to update the screen when active.
	*/
	virtual void Update();

	/**
	* Called each frame to render the screen when active.
	*/
	virtual void Render();

	/**
	* Called when a game-specific event is executed when active.
	*/
	virtual void Notify(XIN xuint iEventType, XIN void* pEventInfo) {}

	/**
	* Set the menu group to render.
	*/
	void SetMenuGroup(t_MenuGroupIndex iMenuGroup);

protected:
	// Menu link actions.
	void Callback_ShowMainMenu();
	void Callback_ShowPlayMenu();
	void Callback_ShowOnlineMenu();
	void Callback_JoinLobby();
	void Callback_CreateLobby();
	void Callback_StartLobby();
	void Callback_StartGame();
	void Callback_QuitGame();

	// Callback implementations.
	void Debug_HideWindow(CButtonComponent* pButton, xpoint xOffset);
	
	// The font metadata to render the links with.
	CFontMetadata* m_pMenuDefault;
	CFontMetadata* m_pMenuHighlight;

	// A list of all the menu elements.
	t_MenuLinkList m_lpMenuLinks[MenuGroupIndex_Max];

	// The background image to scroll.
	CBackgroundImage* m_pBackground;

	// The cursor image to use.
	CBasicSprite* m_pCursor;

	// The currently active menu group.
	t_MenuGroupIndex m_iMenuGroup;

	// The last active menu group.
	t_MenuGroupIndex m_iLastMenuGroup;

	// The next screen to process.
	t_ScreenIndex m_iNextScreen;

	// The lobby start mode.
	t_LobbyStartMode m_iLobbyMode;
};

//##############################################################################
#pragma endregion