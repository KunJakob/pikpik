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

// Tools.
#include <Tools.h>

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

// FMOD.
#include <FMOD/fmod.hpp>

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
#define _SWIDTH				800
#define _SHEIGHT			600
#define _HSWIDTH			400
#define _HSHEIGHT			300
#define _SRECT				xrect(0, 0, _SWIDTH, _SHEIGHT)

// Game options.
#define _MAXPLAYERS			5
#define _MAXNAMELEN			15

// Shortcuts.
#define _GLOBAL				CGlobal::Get()
#define _HGE				Application::GetInterface()
#define _FMOD				Application::GetSoundSystem()
#define _TERMINATE			Application::Terminate()
#define _TIMEDELTA			Application::GetTimeDelta()
#define _TIMEDELTAF			_HGE->Timer_GetDelta()
#define _LOCALE(NAME)		_GLOBAL.pLocale->GetProperty(NAME)->GetString()

// Colour manipulations.
#define COLOURF(COLF) \
		(xuchar)((COLF) * 255.f)

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
class CMetadata;
class CSprite;
class CPlayer;
class CMenuScreen;
class CMap;
class CFont;
class CLobbyScreen;

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
	ScreenIndex_LogoScreen,
	ScreenIndex_WarningScreen,
	ScreenIndex_MenuScreen,
	ScreenIndex_GameScreen,
	ScreenIndex_SelectionScreen,
	ScreenIndex_LobbyScreen,
};

// Layer index.
enum t_LayerIndex
{
	// Menu.
	LayerIndex_Background			= 0,
	LayerIndex_Elements				= 1,

	// Game.
	LayerIndex_Map					= 0,
	LayerIndex_Items				= 1,
	LayerIndex_Player				= 2,
	LayerIndex_Effects				= 3,
	LayerIndex_Radar				= 4,
};

// Renderable types.
enum t_RenderableType
{
	RenderableType_Background,
	RenderableType_Sprite,
	RenderableType_Map,
	RenderableType_Player,
	RenderableType_Text,
	RenderableType_Dialog,
};

// Network packet types.
enum t_NetworkStreamType
{
	NetworkStreamType_PlayerInfo,
};

// Common list types.
typedef XLIST<CMetadata*> t_MetadataList;
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
using namespace fastdelegate;

//##############################################################################
#pragma endregion

#pragma region Global
//##############################################################################
//
//                                   GLOBAL
//
//##############################################################################
class CGlobal
{
public:
	// Singleton instance.
	static inline CGlobal& Get() 
	{
		static CGlobal s_Instance;
		return s_Instance;
	}

	// The menu screen.
	CMenuScreen* pMenu;

	// The lobby screen.
	CLobbyScreen* pLobby;
    
	// The currently active map.
	CMap* pActiveMap;

	// The player list.
	t_PlayerList lpPlayers;

	// The currently active player on the local machine.
	CPlayer* pActivePlayer;

	// The music spectrum energy to determine how fast colours transition.
	xfloat fMusicEnergy;

	// The map overall alpha.
	xfloat fWorldAlpha;

	// The global game font.
	CFont* pGameFont;

	// The global game strings.
	CMetadata* pLocale;

	// The next screen to show.
	t_ScreenIndex iNextScreen;
};

//##############################################################################
#pragma endregion