/**
* @file Selection.h
* @author Nat Ryall
* @date 18/04/2008
* @brief Selection screen implementation.
*
* Copyright © SAPIAN
*/

#ifndef __SELECTION_H__
#define __SELECTION_H__

#pragma region Include
//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Global.
#include <Global.h>

// Other.
#include <Player.h>
#include <Font.h>

//##############################################################################
#pragma endregion

#pragma region Declaration
//##############################################################################
//
//                                 DECLARATION
//
//##############################################################################
class CSelectionScreen : public CScreen
{
public:
	/**
	* Constructor.
	*/
	CSelectionScreen();

	/**
	* Destructor.
	*/
	virtual ~CSelectionScreen();

	/**
	* Add a player to the selection screen.
	*/
	void AddPlayer(CPlayer* pPlayer) {}

protected:
	// The text renderer.
	CShadowFont* m_pFont;

	// The selection screen players.
};

//##############################################################################
#pragma endregion

#endif // __SELECTION_H__
