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

#pragma region Types
//##############################################################################
//
//                                   TYPES
//
//##############################################################################

// The font list types.
enum t_FontType
{
	FontType_Main,
	FontType_Name,
	FontType_Info,
	FontType_Char,
	/*MAX*/FontType_Max,
};

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
	virtual void Sleep() 
	{  
		m_lpPlayers.clear();
	}

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
	virtual void Event(XIN xuint iEventType, XIN void* pEventInfo) {}

	/**
	* Add a player to the selection screen.
	*/
	void PushPlayer(CPlayer* pPlayer);

protected:
	// The text renderer.
	//CFont* m_pFont[FontType_Max];

	// The pacman & ghost sprites.
	CPacMan* m_pPacMan;
	CGhost* m_pGhost;

	// The selection screen players.
	t_PlayerList m_lpPlayers;
};

//##############################################################################
#pragma endregion

#endif // __SELECTION_H__
