#pragma once

/**
* @file Global.h
* @author Nat Ryall
* @date 23/03/2008
*
* Copyright © SAPIAN
*/

#pragma region Include
//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Main.
#include <Main.h>

// Xen.
#include <Xen/Xen.h>

// HGE.
#include <HGE/hge.h>
#include <HGE/hgeanim.h>
#include <HGE/hgecolor.h>
#include <HGE/hgedistort.h>
#include <HGE/hgefont.h>
#include <HGE/hgegui.h>
#include <HGE/hgeguictrls.h>
#include <HGE/hgeparticle.h>
#include <HGE/hgerect.h>
#include <HGE/hgeresource.h>
#include <HGE/hgesprite.h>
#include <HGE/hgestrings.h>
#include <HGE/hgevector.h>

// Standard Lib.
#include <list>
#include <vector>

//##############################################################################
#pragma endregion

#pragma region Macros
//##############################################################################
//
//                                   MACROS
//
//##############################################################################

// Screen options.
#define _SWIDTH         800
#define _SHEIGHT        600
#define _HSWIDTH        400
#define _HSHEIGHT       300

#define _SRECT          XRECT(0, 0, _SWIDTH, _SHEIGHT)

// Shortcuts.
#define _GLOBAL					CGlobal::Get()
#define _HGE						Application::GetInterface()
#define _TERMINATE			Application::Terminate()
#define _TIMEDELTA			Application::GetTimeDelta()
#define _TIMEDELTAF			_HGE->Timer_GetDelta()

// Colour manipulations.
#define COLOURF(COLF) \
				(xuchar)(COLF * 255.0f)

#define ARGBF(A, R, G, B) \
				ARGB(COLOURF(A), COLOURF(R), COLOURF(G), COLOURF(B))

//##############################################################################
#pragma endregion

#pragma region Types
//##############################################################################
//
//                                   TYPES
//
//##############################################################################

// Predeclare.
class CMap;
class CSurface;
class CSprite;
class CPlayer;

// The player identifier type.
enum t_PlayerType
{
	PlayerType_Ghost,
	PlayerType_PacMan,
	/*MAX*/PlayerType_Max,
};

// Screen index.
enum t_ScreenIndex
{
	ScreenIndex_Invalid,
  ScreenIndex_SplashScreen,
	ScreenIndex_MenuScreen,
	ScreenIndex_GameScreen,
};

// Layer index.
enum t_LayerIndex
{
	// Menu.
	LayerIndex_Background		= 0,
	LayerIndex_Elements			= 1,

	// Game.
	LayerIndex_Map					= 0,
	LayerIndex_Items				= 1,
	LayerIndex_Player				= 2,
	LayerIndex_Effects			= 3,
	LayerIndex_Radar				= 4,
};

// Renderable types.
enum t_RenderableType
{
	RenderableType_Background,
	RenderableType_Sprite,
	RenderableType_Map,
	RenderableType_Player,
};

// Common list types.
typedef XLIST<CSurface*> t_SurfaceList;
typedef XLIST<CSprite*> t_SpriteList;
typedef XLIST<CPlayer*> t_PlayerList;

//##############################################################################
#pragma endregion

#pragma region Namespace
//##############################################################################
//
//                                 NAMESPACE
//
//##############################################################################
using namespace Xen;

//##############################################################################
#pragma endregion

#pragma region Global
//##############################################################################
//
//                                   GLOBAL
//
//##############################################################################
class CGlobal : public Templates::CSingletonT<CGlobal>
{
public:
	// The currently active map.
	CMap* pActiveMap;

	// The player list.
	t_PlayerList lpPlayers;

	// The currently active player on the local machine.
	CPlayer* pActivePlayer;
};

//##############################################################################
#pragma endregion