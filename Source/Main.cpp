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

// Screens.
#include <Splash.h>
#include <Menu.h>
#include <Game.h>

// Other.
#include <Windows.h>
#include <Resource.h>

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
  s_pInterface->System_SetState(HGE_TITLE, "PMO");
  s_pInterface->System_SetState(HGE_USESOUND, false);
  s_pInterface->System_SetState(HGE_WINDOWED, true);
  s_pInterface->System_SetState(HGE_SCREENWIDTH, _SWIDTH);
  s_pInterface->System_SetState(HGE_SCREENHEIGHT, _SHEIGHT);
  s_pInterface->System_SetState(HGE_SCREENBPP, 32);
	s_pInterface->System_SetState(HGE_SHOWSPLASH, false);
	s_pInterface->System_SetState(HGE_FPS, 60);

  if(s_pInterface->System_Initiate())
	{
		try
		{
			Application::Initialise();
			s_pInterface->System_Start();
			Application::Deinitialise();
		}
		catch (Xen::CException xException)
		{
			XLOG(xException.GetDetailedMessage().c_str());
		}
  }

  s_pInterface->System_Shutdown();
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

	// Load all relevant metadata.
	ResourceManager::RegisterMetadata(new CMetadata(".\\Metadata\\Sprites.mta"));
	ResourceManager::RegisterMetadata(new CMetadata(".\\Metadata\\Font.mta"));

	// Initialise all modules.
	Xen::ModuleManager::Initialise();

	// Create all the screen instances.
	s_lpScreens.push_back(new CSplashScreen);
	s_lpScreens.push_back(new CMenuScreen);
	s_lpScreens.push_back(new CGameScreen);

	ScreenManager::Set(ScreenIndex_SplashScreen);
}

// =============================================================================
// Nat Ryall                                                          8-Apr-2008
// =============================================================================
void Application::Deinitialise()
{
	// Deinitialise all modules.
	Xen::ModuleManager::Deinitialise();

	// Free all screen instances.
	XEN_LIST_ERASE_ALL(s_lpScreens);
}

// =============================================================================
// Nat Ryall                                                         23-Mar-2008
// =============================================================================
XBOOL Application::Update()
{
	s_iTimeDelta = (XUINT)(_TIMEDELTAF * 1000.f);

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

//##############################################################################
#pragma endregion

