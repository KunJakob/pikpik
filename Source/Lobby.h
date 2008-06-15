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

// Predeclare.
class CStatusBox;
class CJoinInterface;

// The lobby start mode.
enum t_LobbyStartMode
{
	LobbyStartMode_Join,
	LobbyStartMode_Create,
};

// The lobby state.
enum t_LobbyState
{
	LobbyState_None,
	LobbyState_Join,
	LobbyState_Creating,
	LobbyState_Connecting,
	LobbyState_Joining,
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

	// Start the lobby.
	void Start(t_LobbyStartMode iStartMode);

protected:
	//
	void OnJoinClicked(CButtonComponent* pButton, xpoint xOffset);

	// Set the internal state.
	void SetState(t_LobbyState iState);

	// The current lobby state.
	t_LobbyState m_iState;

	// The join interface.
	CJoinInterface* m_pJoinInterface;

	// The status box.
	CStatusBox* m_pStatusBox;
};

//##############################################################################

//##############################################################################
//
//                               STATUS SCREEN
//
//##############################################################################
class CStatusBox
{
public:
	// Friends.
	friend class CLobbyScreen;

	// Constructor.
	CStatusBox();

	// Destructor.
	virtual ~CStatusBox();

	// Update the status box with the specified text.
	void SetText(const xchar* pStatus);

	// Attach all elements to the interface.
	inline void AttachElements()
	{
		InterfaceManager.GetScreen()->Attach(m_pStatusBox);
		InterfaceManager.GetScreen()->Attach(m_pLabel);
	}

protected:
	// The status screen display box.
	CProgressComponent* m_pStatusBox;

	// The status text.
	CLabelComponent* m_pLabel;
};

//##############################################################################

//##############################################################################
//
//                               JOIN INTERFACE
//
//##############################################################################
class CJoinInterface
{
public:
	// Friends.
	friend class CLobbyScreen;

	// Constructor.
	CJoinInterface();

	// Destructor.
	virtual ~CJoinInterface();

	// Attach all elements to the interface.
	inline void AttachElements()
	{
		InterfaceManager.GetScreen()->Attach(m_pAddressBox);
		InterfaceManager.GetScreen()->Attach(m_pJoinButton);
	}

protected:
	// The join screen address box.
	CInputComponent* m_pAddressBox;

	// The join screen join button.
	CButtonComponent* m_pJoinButton;
};

//##############################################################################