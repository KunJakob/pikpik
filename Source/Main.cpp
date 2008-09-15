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
#include <Collision.h>
#include <Player.h>
#include <Visor.h>
#include <Crypt.h>
#include <Sound.h>

// Crypto.
#include <Crypto/cryptlib.h>

//##############################################################################

//##############################################################################
//
//                                   TYPES
//
//##############################################################################

// Lists.
typedef xlist<CScreen*> t_ScreenList;

//##############################################################################

//##############################################################################
//
//                                   STATIC
//
//##############################################################################

// HGE.
static HGE* s_pInterface = NULL;

// Flow Control.
static xbool s_bTerminate = false;

// Timer.
static xuint s_iTimeDelta = 0;

// Screen List.
::t_ScreenList s_lpScreens;

//##############################################################################

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

	s_pInterface->System_SetState(HGE_FRAMEFUNC, &Application::OnUpdate);
	s_pInterface->System_SetState(HGE_RENDERFUNC, &Application::OnRender);
	s_pInterface->System_SetState(HGE_FOCUSLOSTFUNC, &Application::OnBlur);
	s_pInterface->System_SetState(HGE_FOCUSGAINFUNC, &Application::OnFocus);
	s_pInterface->System_SetState(HGE_TITLE, "PikPik");
	s_pInterface->System_SetState(HGE_USESOUND, false);
	s_pInterface->System_SetState(HGE_WINDOWED, true);
	s_pInterface->System_SetState(HGE_SCREENWIDTH, _SWIDTH);
	s_pInterface->System_SetState(HGE_SCREENHEIGHT, _SHEIGHT);
	s_pInterface->System_SetState(HGE_SCREENBPP, 32);
	s_pInterface->System_SetState(HGE_SHOWSPLASH, false);
	s_pInterface->System_SetState(HGE_FPS, 60);
	s_pInterface->System_SetState(HGE_DONTSUSPEND, true);

	if (s_pInterface->System_Initiate())
	{
		HWND hWindow = s_pInterface->System_GetState(HGE_HWND);

		try
		{
			Application::Initialise();

			s_pInterface->System_Start();
		}
		catch (Xen::CException xException)
		{
			_TERMINATE;

			MessageBox(hWindow, xException.GetDetailedMessage().c_str(), "Application Error", MB_OK | MB_ICONERROR | MB_APPLMODAL);
			XLOG(xException.GetDetailedMessage().c_str());
		}
		catch (CryptoPP::Exception xException)
		{
			_TERMINATE;

			MessageBox(hWindow, xException.what(), "Cryptographic Error", MB_OK | MB_ICONERROR | MB_APPLMODAL);
			XLOG(xException.what());
		}

		Application::Deinitialise();
	}

	s_pInterface->System_Shutdown();

	s_pInterface->Release();
	s_pInterface = NULL;

	return 0;
}

//##############################################################################

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
	// Seed the random number generator.
	srand(_TIMEMS);

	// Initialise global vars.
	Global.m_bWindowFocused = true;
	Global.m_pActiveMap = NULL;
	Global.m_pLocalPlayer = NULL;
	Global.m_fMusicEnergy = 0.f;
	Global.m_fWorldAlpha = 1.f;

	// Add all required modules to the game.
	XMODULE(&SoundManager);
	XMODULE(&ResourceManager);
	XMODULE(&NetworkManager);
	XMODULE(&MatchManager);
	XMODULE(&MapManager);
	XMODULE(&ScreenManager);
	XMODULE(&CollisionManager);
	XMODULE(&InterfaceManager);
	XMODULE(&RenderManager);
	
	// Initialise all modules.
	ModuleManager.Initialise();

	// Load all relevant metadata.
	ResourceManager.Load(_METADATA("Sprites"));
	ResourceManager.Load(_METADATA("Fonts"));
	ResourceManager.Load(_METADATA("Sounds"));

	// Load the locale.
	Global.m_pLocale = _METADATA("Strings");

	// Initialise the game font.
	Global.m_pGameFont = new CFont(_FONT("Default"));

	// Create all the screen instances.
	s_lpScreens.push_back(new CLogoScreen);
	s_lpScreens.push_back(new CWarningScreen);
	s_lpScreens.push_back(new CMenuScreen);
	s_lpScreens.push_back(new CGameScreen);
	s_lpScreens.push_back(new CLobbyScreen);
	s_lpScreens.push_back(new CCharacterScreen);
	s_lpScreens.push_back(new CVisorScreen);

	// Load all screen instances and set go to the logo screen.
	ScreenManager.LoadScreens();
	ScreenManager.Set(ScreenIndex_LogoScreen, true);

	// Create all the available players.
	Global.m_lpPlayers.push_back(new CPacman());
	Global.m_lpPlayers.push_back(new CGhost(0xFF40F0F0));
	Global.m_lpPlayers.push_back(new CGhost(0xFFF04040));
	Global.m_lpPlayers.push_back(new CGhost(0xFF4040F0));
	Global.m_lpPlayers.push_back(new CGhost(0xFFF0F040));
	Global.m_lpPlayers.push_back(new CGhost(0xFFF040F0));
	
	// Execute the first frame update.
	OnUpdate();
}

// =============================================================================
// Nat Ryall                                                          8-Apr-2008
// =============================================================================
void Application::Deinitialise()
{
	// Free the string metadata.
	delete Global.m_pLocale;

	// Free the font.
	delete Global.m_pGameFont;

	// Unload all loaded screens.
	ScreenManager.UnloadScreens();

	// Deinitialise all modules.
	ModuleManager.Deinitialise();

	// Free all screen instances.
	XEN_LIST_ERASE_ALL(s_lpScreens);

	// Free all the players.
	XEN_LIST_ERASE_ALL(Global.m_lpPlayers);
}

// =============================================================================
// Nat Ryall                                                         23-Mar-2008
// =============================================================================
xbool Application::OnUpdate()
{
	if (!s_bTerminate)
	{
		s_iTimeDelta = (xuint)(_TIMEDELTAF * 1000.f);

		ModuleManager.Update();

		hgeInputEvent hgEvent;

		while (s_pInterface->Input_GetEvent(&hgEvent))
			ScreenManager.Event(hgEvent.type, &hgEvent);
	}

	return s_bTerminate;
}

// =============================================================================
// Nat Ryall                                                         23-Mar-2008
// =============================================================================
xbool Application::OnRender()
{
	if (!s_bTerminate)
	{
		s_pInterface->Gfx_BeginScene();

		ModuleManager.Render();

		s_pInterface->Gfx_EndScene();
	}
		
	return false;
}

// =============================================================================
// Nat Ryall                                                         30-Jul-2008
// =============================================================================
xbool Application::OnBlur()
{
	Global.m_bWindowFocused = false;

	return false;
}

// =============================================================================
// Nat Ryall                                                         30-Jul-2008
// =============================================================================
xbool Application::OnFocus()
{
	Global.m_bWindowFocused = true;

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
xuint Application::GetTimeDelta()
{
	return s_iTimeDelta;
}

//##############################################################################


