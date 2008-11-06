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
	// The visor transition state.
	t_VisorTransitionState m_iState;
	
	// The visor display message.
	xstring m_xText;

	// The visor sub-message.
	xstring m_xSubText;
};

//##############################################################################