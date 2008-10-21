#pragma once

/**
* @file Menu.h
* @author Nat Ryall
* @date 11/04/2008
* @brief Manages menu transitions.
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
#include <Sprite.h>
#include <Background.h>
#include <Font.h>
#include <Interface.h>
#include <Component.h>


//##############################################################################

//##############################################################################
//
//                                   TYPES
//
//##############################################################################

// Predeclare.
class CMenuScreen;
class CMenuLink;

// The different group types.
enum t_MenuGroup
{
	MenuGroup_None = -1,
	MenuGroup_Main,
	MenuGroup_Online,
	MenuGroup_Join,
	MenuGroup_Create,
	MenuGroup_Levels,
	/*MAX*/MenuGroup_Max,
};

// The menu states.
enum t_MenuState
{
	MenuState_None = -1,
	MenuState_TransitionIn,
	MenuState_Idle,
	MenuState_TransitionOut,
};

// Lists.
typedef xlist<CMenuLink*> t_MenuLinkList;

//##############################################################################

//##############################################################################
//
//                                  ELEMENTS
//
//##############################################################################

// Text link element.
class CMenuLink : public CHyperlinkComponent
{
public:
	// Friends.
	friend CMenuScreen;

	// Constructor.
	CMenuLink(xuint iGroupIndex, CFontMetadata* pFont, const xchar* pText, t_fpLinkSelectedCallback fpCallback = NULL);

	// Destructor.
	virtual ~CMenuLink() {}

	// The element index specifying the on screen order of the link.
	xuint m_iElementIndex;

	// Determines if the element is transitioning in from the right.
	xbool m_bTransitionRight;

	// The element transition tracker.
	xuint m_iTransitionTime;

	// The offset timer for the transition to start.
	CTimer m_xStartTimer;

	// The final link position on screen.
	xpoint m_xLinkPosition;

protected:
	// Re-position the menu link in relation to other links.
	void RePosition(xuint iElementIndex, xuint iNumElements);

	// The group index of the element. This us used to enable/disable elements in groups.
	xuint m_iGroupIndex;
};

//##############################################################################

//##############################################################################
//
//                                 DECLARATION
//
//##############################################################################
class CMenuScreen : public CScreen
{
public:
	// Constructor.
	CMenuScreen() : CScreen(ScreenIndex_MenuScreen) {}

protected:
	// Called to load the screen resources.
	virtual void OnLoad();

	// Called to unload the screen resources.
	virtual void OnUnload();

	// Called when the screen is activated.
	virtual void OnActivate();

	// Called when the state is deactivated.
	virtual void OnDeactivate();

	// Called once when the screen is placed at the top of the stack.
	virtual void OnWake();

	// Called once when the screen goes out of scope either through destruction or another screen is placed above this on the stack.
	virtual void OnSleep();

	// Called each frame to update the screen when active.
	virtual void OnUpdate();

	// Called each frame to render the screen when active.
	virtual void OnRender();

	// Called when a game-specific event is executed when active.
	virtual void OnNotify(xuint iEventType, void* pEventInfo) {}

	// OnUpdate the transition in/out state.
	void UpdateTransition(xbool bTransitionIn);

	// Show any pending screens.
	void ShowNextScreen();

	// Set the menu group to use.
	void SetMenuGroup(t_MenuGroup iMenuGroup);

	// Set the menu state.
	void SetState(t_MenuState iState);

	// Set the next screen to show once the transition out has completed.
	void SetNextScreen(t_ScreenIndex iScreenIndex);

	// Detach all links and add specific elements corresponding to the specified menu group.
	void AttachMenuGroup(t_MenuGroup iMenuGroup);

	// Initialise the transition.
	void InitTransition(xbool bTransitionIn);

	// Menu link actions.
	void Callback_ShowMainMenu();
	void Callback_ShowOnlineMenu();
	void Callback_ShowJoinMenu();
	void Callback_ShowCreateMenu();
	void Callback_JoinPublic();
	void Callback_JoinPrivate();
	void Callback_CreatePublic();
	void Callback_CreatePrivate();
	void Callback_ShowLevelsMenu();
	void Callback_StartGame();
	void Callback_QuitGame();

	// The current menu state.
	t_MenuState m_iState;

	// The font metadata to render the links with.
	CFontMetadata* m_pMenuDefault;

	// The highlight font metadata to highlight key links with.
	CFontMetadata* m_pMenuHighlight;

	// A list of all the menu elements.
	t_MenuLinkList m_lpMenuLinks[MenuGroup_Max];

	// The background image to scroll.
	CBackgroundImage* m_pBackground;

	// The cursor image to use.
	CBasicSprite* m_pCursor;

	// The currently active menu group.
	t_MenuGroup m_iMenuGroup;

	// The pending menu group for transitions.
	t_MenuGroup m_iPendingMenuGroup;

	// The last active menu group.
	t_MenuGroup m_iLastMenuGroup;

	// The next screen to process.
	t_ScreenIndex m_iNextScreen;

	// The lobby start mode.
	t_LobbyStartMode m_iLobbyMode;
};

//##############################################################################