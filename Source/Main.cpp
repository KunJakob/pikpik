//##############################################################################

// Global.
#include <Global.h>

// Local.
#include <Main.h>

// System.
#include <Windows.h>

// Other.
#include <Resource.h>
#include <Collision.h>
#include <Crypt.h>
#include <Sound.h>
#include <Splash.h>
#include <Menu.h>
#include <Game.h>

// Crypto.
#include <Crypto/cryptlib.h>

//##############################################################################

// Lists.
typedef xlist<CScreen*> t_ScreenList;

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

// =============================================================================
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	s_pInterface = hgeCreate(HGE_VERSION);

	s_pInterface->System_SetState(HGE_FRAMEFUNC, &Application::OnUpdate);
	s_pInterface->System_SetState(HGE_RENDERFUNC, &Application::OnRender);
	s_pInterface->System_SetState(HGE_FOCUSLOSTFUNC, &Application::OnBlur);
	s_pInterface->System_SetState(HGE_FOCUSGAINFUNC, &Application::OnFocus);
	s_pInterface->System_SetState(HGE_TITLE, "PikPik - Pre-Release - Copyright (c) Sketch Spark 2010");
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

		#if XRETAIL
			MessageBox(hWindow, xException.GetMessage(), "Application Error", MB_OK | MB_ICONERROR | MB_APPLMODAL);
		#endif
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

// =============================================================================
void Application::Initialise()
{
	// Seed the random number generator.
	srand(_TIMEMS);

	// Initialise global vars.
	Global.m_bWindowFocused = true;

	// Add all required modules to the game.
	XMODULE(&SoundManager);
	XMODULE(&RenderManager);
	XMODULE(&ResourceManager);
	XMODULE(&InterfaceManager);
	XMODULE(&CollisionManager);
	XMODULE(&ScreenManager);
	
	// Initialise all modules.
	ModuleManager.Initialise();

	// Load all relevant metadata.
	ResourceManager.Load(_METADATA("Sprites"));
	ResourceManager.Load(_METADATA("Fonts"));
	ResourceManager.Load(_METADATA("Sounds"));

	// Load the locale.
	Global.m_pLocale = _METADATA("Strings");

	// Load the debug metadata.
	Global.m_pDebugMetadata = _METADATA("Interface");

	// Initialise the game font.
	Global.m_pGameFont = new CFont(_FONT("Default"));

	// Create all the screen instances.
	s_lpScreens.push_back(new CLogoScreen);
	s_lpScreens.push_back(new CWarningScreen);
	s_lpScreens.push_back(new CMenuScreen);
	s_lpScreens.push_back(new CGameScreen);

	// Load all screen instances and set go to the logo screen.
	ScreenManager.LoadScreens();
	ScreenManager.Set(ScreenIndex_LogoScreen, true);

	// Execute the first frame update.
	OnUpdate();
}

// =============================================================================
void Application::Deinitialise()
{
	// Free the string metadata.
	delete Global.m_pLocale;

	// Free the debug metadata.
	delete Global.m_pDebugMetadata;

	// Free the font.
	delete Global.m_pGameFont;

	// Unload all loaded screens.
	ScreenManager.UnloadScreens();

	// Deinitialise all modules.
	ModuleManager.Deinitialise();

	// Free all screen instances.
	XEN_LIST_ERASE_ALL(s_lpScreens);
}

// =============================================================================
xbool Application::OnUpdate()
{
	if (!s_bTerminate)
	{
		s_iTimeDelta = (xuint)(_TIMEDELTAF * 1000.f);

		hgeInputEvent hgEvent;

		while (s_pInterface->Input_GetEvent(&hgEvent))
			ModuleManager.Event(hgEvent.type, &hgEvent);

		ModuleManager.Update();
	}

	return s_bTerminate;
}

// =============================================================================
xbool Application::OnRender()
{
	if (!s_bTerminate)
	{
		s_pInterface->Gfx_BeginScene();

		ScreenManager.PreRender();
		RenderManager.Render();
		ScreenManager.PostRender();

		// TODO: This should be moved to a layer (CFadeOverlay).
		//if (Global.m_fScreenAlpha != 1.f)
		//{
		//	xfloat fAlpha = (1.f - Math::Clamp(Global.m_fScreenAlpha, 0.f, 1.f)) * 255.f;
		//	RenderManager.RenderBox(true, xrect(0, 0, _SWIDTH, _SHEIGHT), _ARGB((xuchar)fAlpha, 0, 0, 0));
		//}

		s_pInterface->Gfx_EndScene();
	}
		
	return false;
}

// =============================================================================
xbool Application::OnBlur()
{
	Global.m_bWindowFocused = false;

	return false;
}

// =============================================================================
xbool Application::OnFocus()
{
	Global.m_bWindowFocused = true;

	return false;
}

// =============================================================================
void Application::Terminate()
{
	s_bTerminate = true;
}

// =============================================================================
HGE* Application::GetInterface()
{
	return s_pInterface;
}

// =============================================================================
xuint Application::GetTimeDelta()
{
	return s_iTimeDelta;
}

//##############################################################################