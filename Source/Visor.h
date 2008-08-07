#pragma once

/**
* @file Visor.h
* @author Nat Ryall
* @date 07/08/2008
* @brief The screen introduction, visor effect.
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

//##############################################################################

//##############################################################################
//
//                                   TYPES
//
//##############################################################################

// The visor screen transition states.
enum t_VisorTransitionState
{
	VisorTransitionState_None,
	VisorTransitionState_Enter,
	VisorTransitionState_Idle,
	VisorTransitionState_Exit,
};

//##############################################################################

//##############################################################################
//
//                                   VISOR
//
//##############################################################################
class CVisorScreen : public CScreen
{
public:
	// Constructor.
	CVisorScreen();

	// Set the visor text and subtext.
	void SetText(const xchar* pText, const xchar* pSubText = NULL);

protected:
	// Called to load the screen resources.
	virtual void OnLoad();

	// Called to unload the screen resources.
	virtual void OnUnload();

	// Called when the screen is added to the screen stack.
	virtual void OnActivate();

	// Called when the screen is removed from the screen stack.
	virtual void OnDeactivate();

	// Called to update the screen (updates the parent screen by default).
	virtual void OnUpdate();

	// Called to render the next screen frame (renders the parent screen by default).
	virtual void OnRender();

	// The visor transition state.
	t_VisorTransitionState m_iState;
	
	// The visor display message.
	xstring m_xText;

	// The visor sub-message.
	xstring m_xSubText;
};

//##############################################################################