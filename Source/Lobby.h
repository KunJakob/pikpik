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
//                                LOBBY SCREEN
//
//##############################################################################
class CLobbyScreen : public CScreen
{
public:
	/**
	* Initialise the screen.
	*/
	CLobbyScreen();

	/**
	* Deinitialise the screen.
	*/
	virtual ~CLobbyScreen();
	
	/**
	* Called when the screen is first added to the screen stack.
	*/
	virtual void Load() {}

	/**
	* Called when the screen is removed from the screen stack.
	*/
	virtual void Unload() {}

	/**
	* Called once when the screen is placed at the top of the stack.
	*/
	virtual void Wake() {}

	/**
	* Called once when the screen goes out of scope either through destruction or another screen is placed above this on the stack.
	*/
	virtual void Sleep() {}

	/**
	* Called each frame to update the screen when active.
	*/
	virtual void Update() {}

	/**
	* Called each frame to render the screen when active.
	*/
	virtual void Render() {}

	/**
	* Called when a game-specific event is executed when active.
	*/
	virtual void Event(XIN xuint iEventType, XIN void* pEventInfo) {}

protected:
	// Create and initialise the lobby interface.
	void CreateInterface();

	// Create a specific window for the lobby.
	void CreatePlayerList();
	void CreateOptionsWindow();
	void CreateChatWindow();

	// The player list interface object.
	class CPlayerList
	{
	public:
	}
	m_oPlayerList;

	// The options window interface object.
	class COptionsWindow
	{
	public:
		CWindowComponent* m_pMainWindow;
	}
	m_oOptionsWindow;
};

//##############################################################################