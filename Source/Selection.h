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

//##############################################################################

// Global.
#include <Global.h>

// Other.
#include <Player.h>
#include <Font.h>

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
class CSelectionScreen : public CScreen
{
public:
	// Constructor.
	CSelectionScreen();

	// Destructor.
	virtual ~CSelectionScreen();

	// Called to update the screen.
	virtual void OnUpdate();

	// Add a player to the selection screen.
	void PushPlayer(CPlayer* pPlayer);

protected:
	// The text renderer.
	//CFont* m_pFont[FontType_Max];

	// The pacman & ghost sprites.
	CPacman* m_pPacMan;
	CGhost* m_pGhost;

	// The selection screen players.
	t_PlayerList m_lpPlayers;
};

//##############################################################################

#endif // __SELECTION_H__
