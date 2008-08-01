#pragma once

/**
* @file Character.h
* @author Nat Ryall
* @date 25/06/2008
* @brief The character selection screen.
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
#include <Font.h>
#include <Sprite.h>

//##############################################################################

//##############################################################################
//
//                                   TYPES
//
//##############################################################################

// The character screen selection mode.
enum t_CharacterScreenMode
{
	CharacterScreenMode_Offline,
	CharacterScreenMode_Online,
};

//##############################################################################

//##############################################################################
//
//                               CHARACTER SCREEN
//
//##############################################################################
class CCharacterScreen : public CScreen
{
public:
	// Constructor.
	CCharacterScreen();

	// Destructor.
	virtual ~CCharacterScreen() {}

	// Set the character selection mode.
	void SetMode(t_CharacterScreenMode iMode) {}

	// Called when the screen is first added to the screen stack.		
	virtual void OnActivate();

	// Called when the screen is removed from the screen stack.		
	virtual void OnDeactivate();

	// Called each frame to update the screen when active.		
	virtual void OnUpdate();

	// Called each frame to render the screen when active.	
	virtual void OnRender();

protected:
	// The screen message font.
	CFont* m_pMessageFont;

	// The screen countdown font.
	CFont* m_pCounterFont;

	// The number of players to select.
	xint m_iNumPlayers;
};

//##############################################################################
