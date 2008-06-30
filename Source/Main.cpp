#pragma region Include
//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Global.
#include <Global.h>

// Local.
#include <Main.h>

// Other.
#include <Splash.h>
#include <Menu.h>
#include <Game.h>
#include <Lobby.h>
#include <Windows.h>
#include <Resource.h>
#include <Network.h>
#include <Character.h>
#include <Match.h>

//##############################################################################
#pragma endregion

#pragma region Types
//##############################################################################
//
//                                   TYPES
//
//##############################################################################

// Lists.
typedef XLIST<CScreen*> t_ScreenList;

//##############################################################################
#pragma endregion

#pragma region Static
//##############################################################################
//
//                                   STATIC
//
//##############################################################################

// HGE.
static HGE* s_pInterface = NULL;

// Sound.
static FMOD::System* s_pSoundSystem = NULL;

// Flow Control.
static XBOOL s_bTerminate = false;

// Timer.
static XUINT s_iTimeDelta = 0;

// Screen List.
static t_ScreenList s_lpScreens;

//##############################################################################
#pragma endregion

#pragma region Main
//##############################################################################
//
//                                    MAIN
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         23-Mar-2008
// =============================================================================
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	s_pInterface = hgeCreate(HGE_VERSION);

	s_pInterface->System_SetState(HGE_FRAMEFUNC, &Application::Update);
	s_pInterface->System_SetState(HGE_RENDERFUNC, &Application::Render);
	s_pInterface->System_SetState(HGE_TITLE, "PikPik");
	s_pInterface->System_SetState(HGE_USESOUND, false);
	s_pInterface->System_SetState(HGE_WINDOWED, true);
	s_pInterface->System_SetState(HGE_SCREENWIDTH, _SWIDTH);
	s_pInterface->System_SetState(HGE_SCREENHEIGHT, _SHEIGHT);
	s_pInterface->System_SetState(HGE_SCREENBPP, 32);
	s_pInterface->System_SetState(HGE_SHOWSPLASH, false);
	s_pInterface->System_SetState(HGE_FPS, 60);
	s_pInterface->System_SetState(HGE_DONTSUSPEND, true);

	if (FMOD::System_Create(&s_pSoundSystem) == FMOD_OK)
	{
		FMOD_RESULT eSoundResult = s_pSoundSystem->init(32, FMOD_INIT_NORMAL, NULL);

		if (eSoundResult == FMOD_OK && s_pInterface->System_Initiate())
		{
			try
			{
				Application::Initialise();
				s_pInterface->System_Start();
				Application::Deinitialise();
			}
			catch (Xen::CException xException)
			{
				MessageBox(s_pInterface->System_GetState(HGE_HWND), xException.GetDetailedMessage().c_str(), "Error", MB_OK | MB_ICONERROR);
				XLOG(xException.GetDetailedMessage().c_str());
			}

			s_pInterface->System_Shutdown();
		}

		s_pSoundSystem->release();
		s_pSoundSystem = NULL;
	}

	s_pInterface->Release();
	s_pInterface = NULL;

	return 0;
}

//##############################################################################
#pragma endregion

#pragma region Declaration
//##############################################################################
//
//                                 DECLARATION
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                          8-Apr-2008
// =============================================================================
void Application::Initialise()
{
	srand(GetTickCount());

	// Load the strings.
	_GLOBAL.pLocale = new CMetadata(".\\Metadata\\Strings.mta");

	// Load all relevant metadata.
	ResourceManager::RegisterMetadata(new CMetadata(".\\Metadata\\Sprites.mta"));
	ResourceManager::RegisterMetadata(new CMetadata(".\\Metadata\\Font.mta"));

	// Initialise the game font.
	_GLOBAL.pGameFont = new CFont(_FONT("Default"));

	// Initialise all modules.
	Xen::ModuleManager::Initialise();

	// Create all the screen instances.
	s_lpScreens.push_back(new CLogoScreen);
	s_lpScreens.push_back(new CWarningScreen);
	s_lpScreens.push_back(new CMenuScreen);
	s_lpScreens.push_back(new CGameScreen);
	s_lpScreens.push_back(new CLobbyScreen);
	s_lpScreens.push_back(new CCharacterScreen);

	ScreenManager::Set(ScreenIndex_LogoScreen);

	// Create all the available players.
	//_GLOBAL.lpPlayers.push_back(new CPacMan(_GLOBAL.pActiveMap->GetSpawnBlock(PlayerType_PacMan)));
	//_GLOBAL.lpPlayers.push_back(new CGhost(_GLOBAL.pActiveMap->GetSpawnBlock(PlayerType_Ghost), 0xFF40F0F0));
	//_GLOBAL.lpPlayers.push_back(new CGhost(_GLOBAL.pActiveMap->GetSpawnBlock(PlayerType_Ghost), 0xFFF0F040));
	//_GLOBAL.lpPlayers.push_back(new CGhost(_GLOBAL.pActiveMap->GetSpawnBlock(PlayerType_Ghost), 0xFFF040F0));

	// Initialise the matchmaking system.
	MatchManager.Initialise();
	MatchManager.CreateSession(5);
}

// =============================================================================
// Nat Ryall                                                          8-Apr-2008
// =============================================================================
void Application::Deinitialise()
{
	// Clear the screen.
	_GLOBAL.iNextScreen = ScreenIndex_Invalid;

	// Free the string metadata.
	delete _GLOBAL.pLocale;

	// Free the font.
	delete _GLOBAL.pGameFont;

	// Deinitialise all modules.
	Xen::ModuleManager::Deinitialise();

	// Free all screen instances.
	XEN_LIST_ERASE_ALL(s_lpScreens);

	// Free all the players.
	XEN_LIST_ERASE_ALL(_GLOBAL.lpPlayers);
}

// =============================================================================
// Nat Ryall                                                         23-Mar-2008
// =============================================================================
XBOOL Application::Update()
{
	if (_GLOBAL.iNextScreen != ScreenIndex_Invalid)
	{
		Xen::ScreenManager::Set(_GLOBAL.iNextScreen);
		_GLOBAL.iNextScreen = ScreenIndex_Invalid;
	}

	s_iTimeDelta = (XUINT)(_TIMEDELTAF * 1000.f);

	Network.Update();

	s_pSoundSystem->update();

	Xen::ModuleManager::Update();

	hgeInputEvent hgEvent;

	while (s_pInterface->Input_GetEvent(&hgEvent))
		Xen::ScreenManager::Event(hgEvent.type, &hgEvent);

	return s_bTerminate;
}

// =============================================================================
// Nat Ryall                                                         23-Mar-2008
// =============================================================================
XBOOL Application::Render()
{
	s_pInterface->Gfx_BeginScene();

	Xen::ModuleManager::Render();

	s_pInterface->Gfx_EndScene();

	return false;
}

// =============================================================================
// Nat Ryall                                                          6-Apr-2008
// =============================================================================
void Application::Terminate()
{
	s_bTerminate = true;
}

// =============================================================================
// Nat Ryall                                                          6-Apr-2008
// =============================================================================
HGE* Application::GetInterface()
{
	return s_pInterface;
}

// =============================================================================
// Nat Ryall                                                          8-Apr-2008
// =============================================================================
XUINT Application::GetTimeDelta()
{
	return s_iTimeDelta;
}

// =============================================================================
// Nat Ryall                                                          2-Jun-2008
// =============================================================================
FMOD::System* Application::GetSoundSystem()
{
	return s_pSoundSystem;
}

//##############################################################################
#pragma endregion


