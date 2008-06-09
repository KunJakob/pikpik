#pragma once

/**
* @file Lobby.h
* @author Nat Ryall
* @date 22/05/2008
* @brief The online lobby screen implementation.
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
#include <Component.h>

//##############################################################################

//##############################################################################
//
//                                   TYPES
//
//##############################################################################

// The lobby state.
enum t_LobbyState
{
	LobbyState_None,
	LobbyState_JoinScreen,
};

//##############################################################################

//##############################################################################
//
//                                LOBBY SCREEN
//
//##############################################################################
class CLobbyScreen : public CScreen
{
public:
	// Initialise.
	CLobbyScreen();

	// Deinitialise
	virtual ~CLobbyScreen();
	
	// Called when the screen is first added to the screen stack.
	virtual void Load();

	// Called when the screen is removed from the screen stack.
	virtual void Unload();

	// Called each frame to update the screen when active.
	virtual void Update();

	// Called each frame to render the screen when active.
	virtual void Render();

	// Called when a game-specific event is executed when active.
	//virtual void Event(XIN xuint iEventType, XIN void* pEventInfo) {}

protected:
	// The join screen address box.
	CInputComponent* m_pAddressBox;

	// The join screen join button.
	CButtonComponent* m_pJoinButton;

	// The join screen back button.
	//CButtonComponent* m_pBackButton;
};

//##############################################################################