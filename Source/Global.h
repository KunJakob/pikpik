#pragma once

/**
* @file Global.h
* @author Nat Ryall
* @date 23/03/2008
*
* Copyright © SAPIAN
*/

//##############################################################################

// RakNet.
#include <RakNet/TCPInterface.h>
#include <RakNet/HTTPConnection.h>
#include <RakNet/RakNetworkFactory.h>
#include <RakNet/RakPeerInterface.h>
#include <RakNet/MessageIdentifiers.h>
#include <RakNet/BitStream.h>

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

// STL.
#include <list>
#include <vector>
#include <map>

// Other.
#include <limits.h>
#include <float.h>

//##############################################################################

// Screen options.
#define _SWIDTH					1024
#define _SHEIGHT				768
#define _HSWIDTH				512
#define _HSHEIGHT				384
#define _SRECT					xrect(0, 0, _SWIDTH, _SHEIGHT)

// Game options.
#define _GID					"PikPik-1"
#define _HOSTPORT				20557
#define _MAXPLAYERS				5
#define _MAXNAMELEN				15
#define _MAXNAMECHARS			16

// Shortcuts.
#define _HGE					Application::GetInterface()
#define _FMOD					CSoundManager::Get().GetSoundSystem()
#define _TERMINATE				Application::Terminate()
#define _TIMEMS					GetTickCount()
#define _TIMEDELTA				Application::GetTimeDelta()
#define _TIMEDELTAF				_HGE->Timer_GetDelta()
#define _LOCALE(NAME)			Global.GetLocale(NAME)

// Colour manipulations.
#define _ARGB(A, R, G, B)		ARGB(A, R, G, B)
#define _RGB(R, G, B)			ARGB(255, R, G, B)
#define _COLF(F)				(xuchar)((F) * 255.f)
#define _ARGBF(A, R, G, B)		ARGB(_COLF(A), _COLF(R), _COLF(G), _COLF(B))
#define _RGBF(R, G, B)			ARGB(255, _COLF(R), _COLF(G), _COLF(B))

// Metadata control.
#if XDEBUG
#define _METADATA(FNAME)		new CMetadata(".\\Metadata\\" FNAME ".mta", NULL, true)
#else
#define _METADATA(FNAME)		new CMetadata(".\\Metadata\\" FNAME ".emta", CryptManager.GetEncryptionKey(), true)
#endif

// Singletons.
#define Global					CGlobal::Get()

//##############################################################################

// Predeclare.
class CSprite;
class CPlayer;
class CMap;
class CFont;

// The player identifier type.
enum t_PlayerType
{
	PlayerType_Ghost,
	PlayerType_Pacman,
	
	PlayerType_Max,
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
	ScreenIndex_CharacterScreen,
	ScreenIndex_VisorScreen,

	ScreenIndex_Max,
};

// Layer index.
enum t_LayerIndex
{
	// Menu.
	LayerIndex_Background,
	LayerIndex_Elements,

	// Game.
	LayerIndex_Map,
	LayerIndex_Items,
	LayerIndex_Player,
	LayerIndex_Effects,
	LayerIndex_Radar,

	// Overlays.
	LayerIndex_Interface,

	// Max.
	LayerIndex_Max,
};

// Menu layer index list.
enum t_MenuLayerIndex
{
	MenuLayerIndex_Background,
	MenuLayerIndex_Interface,

	MenuLayerIndex_Max,
};

// Lobby layer index list.
enum t_LobbyLayerIndex
{
	LobbyLayerIndex_Background,
	LobbyLayerIndex_Listing,
	LobbyLayerIndex_Interface,

	LobbyLayerIndex_Max,
};

// Game layer index list.
enum t_GameLayerIndex
{
	GameLayerIndex_Background,
	GameLayerIndex_Map,
	GameLayerIndex_Items,
	GameLayerIndex_Player,
	GameLayerIndex_Effects,
	GameLayerIndex_GhostMask,
	GameLayerIndex_Radar,

	GameLayerIndex_PathDebug,

	GameLayerIndex_Max,
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
	RenderableType_Minimap,

	RenderableType_Max,
};

// Network packet types.
enum t_NetworkStreamType
{
	NetworkStreamType_PlayerInfo,
	NetworkStreamType_StartGame,
	NetworkStreamType_PlayerUpdate,
};

// The lobby start mode.
enum t_LobbyStartMode
{
	LobbyStartMode_JoinPublic,
	LobbyStartMode_JoinPrivate,
	LobbyStartMode_CreatePublic,
	LobbyStartMode_CreatePrivate,
};

// The collision groups in the game.
enum t_CollisionGroup
{
	CollisionGroup_Player,
	CollisionGroup_Trap,
	CollisionGroup_Power,

	CollisionGroup_Max,
};

// Common list types.
typedef xlist<CMetadata*> t_MetadataList;
typedef xlist<CSprite*> t_SpriteList;
typedef xarray<CPlayer*> t_PlayerList;

//##############################################################################
using namespace Xen;
using namespace fastdelegate;

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

	// Get a locale string by name.
	// ~return The locale string or the identifier name if the locale cannot be found.
	const xchar* GetLocale(const xchar* pName);

	// Process and substitute a string if it is a locale variable.
	const xchar* GetLocaleFromVar(const xchar* pInput);

	// Determine the list of players for the active map and position them.
	void ResetActivePlayers();

	// The current focus status of the game window.
	xbool m_bWindowFocused;

	// The overall screen alpha.
	xfloat m_fMapAlpha;

	// The global game font.
	CFont* m_pGameFont;

	// The global game strings.
	CMetadata* m_pLocale;

	// The currently active map.
	CMap* m_pActiveMap;

	// The list of all players available to the game.
	t_PlayerList m_lpPlayers;

	// The list of all active players in the current game.
	t_PlayerList m_lpActivePlayers;

	// The currently active player on the local machine.
	CPlayer* m_pLocalPlayer;

	// The music spectrum energy to determine how fast colours transition.
	xfloat m_fMusicEnergy;

#if !XRETAIL
	// The debug controls metadata.
	CMetadata* m_pDebugMetadata;
#endif
};

//##############################################################################
