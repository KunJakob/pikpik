#pragma once

//##############################################################################
// 
//	**************************************************************************
//	File: Screen System
//	**************************************************************************
//	Part of the Xen Engine.
// 
//	Author: Nat Ryall
//	Creation Date: Tuesday, 9 October 2007 (2:49 PM [GMT +10])
//	--------------------------------------------------------------------------
//	Copyright © 1998 - 2007, SAPIAN
// 
//##############################################################################

// Common.
#include <Xen/Common.h>

//##############################################################################

// Shortcuts.
#define ScreenManager \
		CScreenManager::Get()

//##############################################################################
namespace Xen
{
	// Screen error types.
	enum t_ScreenError
	{
		ScreenError_Success,
		ScreenError_Pending,
		ScreenError_InvalidState,
		ScreenError_ListEmpty,
	};

	// Predeclare.
	class CScreen;
	class CScreenManager;

	// Lists.
	typedef xarray<CScreen*> t_ScreenList;
}

//##############################################################################
namespace Xen
{
	// Screen base class.
	class CScreen
	{
	public:
		// Friends.
		friend class CScreenManager;

		// Constructor.
		// ~iScreenIndex The unique screen index.
		CScreen(xint iScreenIndex);

		// Destructor.
		virtual ~CScreen();

		// Get the unique screen index for this screen.
		inline xint GetScreenIndex()
		{
			return m_iScreenIndex;
		}

		// Determine if the screen is currently loaded.
		inline xbool IsLoaded()
		{
			return m_bScreenLoaded;
		}

		// Determine if the screen is currently active.
		inline xbool IsActive()
		{
			return m_bScreenActive;
		}

		// Determine if the screen is currently awake.
		inline xbool IsAwake()
		{
			return m_bScreenAwake;
		}

	protected:
		// Called to load the screen resources.
		virtual void OnLoad() {}

		// Called to unload the screen resources.
		virtual void OnUnload() {}

		// Called when the screen is added to the screen stack.
		virtual void OnActivate() {}

		// Called when the screen is removed from the screen stack.
		virtual void OnDeactivate() {}

		// Called when the screen is active and is the immediate screen in the stack.
		virtual void OnWake() {}

		// Called when the screen is no longer the immediate screen in the stack but is still active.
		virtual void OnSleep() {}

		// Called when a specific event is executed.
		// ~return Specifies if the event was consumed.
		virtual xbool OnEvent(xint iEventType, void* pEventInfo) 
		{
			return false;
		}

		// Called to update the screen.
		virtual void OnUpdate() {}

		// Called immediately before the render is executed.
		virtual void OnPreRender() {}

		// Called to render the screen.
		virtual void OnRender() {}

		// Called immediately after the render has executed.
		virtual void OnPostRender() {}

		// Called to notify the immediate parent screen in the stack of an event.
		inline xbool ParentEvent(xint iEventType, void* pEventInfo)
		{
			if (m_pScreenParent)
				return m_pScreenParent->OnEvent(iEventType, pEventInfo);
			else
				return false;
		}

		// Called to update the immediate parent screen in the stack.
		inline void ParentUpdate()
		{
			if (m_pScreenParent)
				m_pScreenParent->OnUpdate();
		}

		// Called to pre-render the immediate parent screen in the stack.
		inline void ParentPreRender()
		{
			if (m_pScreenParent)
				m_pScreenParent->OnPreRender();
		}

		// Called to render the immediate parent screen in the stack.
		inline void ParentRender()
		{
			if (m_pScreenParent)
				m_pScreenParent->OnRender();
		}

		// Called to post-render the immediate parent screen in the stack.
		inline void ParentPostRender()
		{
			if (m_pScreenParent)
				m_pScreenParent->OnPostRender();
		}

	private:
		// The unique screen index.
		xint m_iScreenIndex;

		// The immediate screen parent.
		CScreen* m_pScreenParent;

		// The screen loaded status.
		xbool m_bScreenLoaded;

		// The screen activation status.
		xbool m_bScreenActive;

		// The screen awake status.
		xbool m_bScreenAwake;
	};
}

//##############################################################################
namespace Xen
{
	class CScreenManager : public CModule
	{
	public:
		// Friends.
		friend class CScreen;

		// Singleton instance.
		static inline CScreenManager& Get() 
		{
			static CScreenManager s_Instance;
			return s_Instance;
		}

		// Called immediately before the render is executed.
		void PreRender();

		// Render the screen.
		void Render();

		// Called immediately after the render has executed.
		void PostRender();

		// Load a specific screen.
		void LoadScreen(CScreen* pScreen);

		// Load all registered screens if not already loaded.
		void LoadScreens();

		// Unload a specific screen.
		void UnloadScreen(CScreen* pScreen);

		// Unload all registered screens if not already unloaded.
		void UnloadScreens();

		// Remove all screens from the screen manager.
		// Screen changes are pending until the next frame unless bImmediate is set to true.
		void Clear(xbool bImmediate = false);

		// Clear the existing stack and add the specified screen.
		// Screen changes are pending until the next frame unless bImmediate is set to true.
		t_ScreenError Set(CScreen* pScreen, xbool bImmediate = false);

		// Clear the existing stack and add the screen with the specified index.
		// Screen changes are pending until the next frame unless bImmediate is set to true.
		t_ScreenError Set(xint iScreenIndex, xbool bImmediate = false);

		// Add the specified screen to the top of the screen stack.
		// Screen changes are pending until the next frame unless bImmediate is set to true.
		t_ScreenError Push(CScreen* pScreen, xbool bImmediate = false);

		// Add the screen with the specified index to the top of the screen stack.
		// Screen changes are pending until the next frame unless bImmediate is set to true.
		t_ScreenError Push(xint iScreenIndex, xbool bImmediate = false);

		// Remove the screen at the top of the screen stack.
		// Screen changes are pending until the next frame unless bImmediate is set to true.
		t_ScreenError Pop(xbool bImmediate = false);

		// Get the screen below the specified screen on the stack.
		// ~return The screen below the specified screen or NULL if the specified screen is the lowest on the stack.
		CScreen* GetParent(CScreen* pScreen);

		// Find a screen by screen index
		// ~return The screen instance with the specified index or NULL if the index does not exist.
		CScreen* FindScreen(xint iScreenIndex);

	protected:
		// Initialise the screen manager.
		virtual void OnInitialise();

		// Deinitialise the screen manager.
		virtual void OnDeinitialise();

		// Execute a defined event for the screen at the top of the stack.
		virtual xbool OnEvent(xint iEventType, void* pEventInfo);

		// Update the active states.
		virtual void OnUpdate();

		// OnUpdate the active screens with the pending list.
		void ApplyPendingChanges();

		// Register the screen internally.
		void RegisterScreen(CScreen* pScreen);

		// Deregister the screen.
		void DeregisterScreen(CScreen* pScreen);

		// The list of all registered screens.
		t_ScreenList m_lpRegisteredScreens;

		// The list of all pending screens.
		t_ScreenList m_lpPendingScreens;

		// The list of all active screens.
		t_ScreenList m_lpActiveScreens;
	};
}

//##############################################################################