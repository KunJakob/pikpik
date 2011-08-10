#pragma once

//##############################################################################
// 
//	//*************************************************************************
//	File: Module
//	//*************************************************************************
//	Part of the Xen Engine.
// 
//	Author: Nat Ryall
//	Creation Date: Tuesday, 9 October 2007 (10:56 AM [GMT +10])
//	--------------------------------------------------------------------------
//	Copyright © 1998 - 2007, SAPIAN
// 
//##############################################################################

// Shortcuts.
#define ModuleManager \
		CModuleManager::Get()

// Add a new module.
#define XEN_MODULE \
		ModuleManager.Add

//##############################################################################
namespace Xen
{
	// Predeclare.
	class CModule;
	class CModuleManager;

	// Error types.
	enum t_ModuleError
	{
		ModuleError_Success,				// The operation was a success.
		ModuleError_InvalidModule,			// The module supplied was invalid.
		ModuleError_ListEmpty,				// There are no modules to process.
		ModuleError_NotInitialised,			// The module manager has not been initialised yet.
		ModuleError_AlreadyInitialised,		// The module manager is already initialised.
		ModuleError_NotConsumed,			// No modules consumed the specified event.
	};

	// Lists.
	typedef xarray<CModule*> t_ModuleList;
}

//##############################################################################
namespace Xen
{
	class CModule
	{
	public:
		// Friends.
		friend class CModuleManager;

	protected:
		// Initialise.
		virtual void OnInitialise() {}

		// Deinitialise.
		virtual void OnDeinitialise() {}

		// Event.
		// ~return Specifies if the event should be consumed.
		virtual xbool OnEvent(xint iEventType, void* pEventInfo) 
		{
			return false;
		}

		// Update.
		virtual void OnUpdate() {}
	};
}

//##############################################################################
namespace Xen
{
	class CModuleManager
	{
	public:
		// Constructor.
		CModuleManager();

		// Singleton instance.
		static inline CModuleManager& Get() 
		{
			static CModuleManager s_Instance;
			return s_Instance;
		}

		// Add a module to the system. 
		// ~note All modules must be added before starting the system.
		t_ModuleError Add(CModule* pModule);

		// Initialise all modules in the order they were added.
		t_ModuleError Initialise();

		// Deinitialise all modules in the order they were added.
		t_ModuleError Deinitialise();

		// Notify all modules of an event in the order they were added.
		t_ModuleError Event(xint iEventType, void* pEventInfo);

		// Update all modules in the order they were added.
		t_ModuleError Update();

	protected:
		// Specifies if the module manage has initialised yet.
		xbool m_bInitialised;

		// The list of managed modules.
		t_ModuleList m_lpModules;
	};
}

//##############################################################################