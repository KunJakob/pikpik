#include <Global.h>

//##############################################################################
// 
//	Copyright © 1998 - 2007, SAPIAN
//	--------------------------------------------------------------------------
//	Please see the corresponding header for further details.
// 
//##############################################################################

// Common
#include <Xen/Common.h>

// Local.
#include <Xen/Module.h>

//##############################################################################

// =============================================================================
Xen::CModuleManager::CModuleManager() :
	m_bInitialised(false)
{
}

// =============================================================================
Xen::t_ModuleError Xen::CModuleManager::Add(CModule* pModule)
{
	XMASSERT(pModule, "An invalid module was supplied.");

	if (m_bInitialised)
		return ModuleError_AlreadyInitialised;

	if (pModule == NULL)
		return ModuleError_InvalidModule;

	m_lpModules.push_back(pModule);

	return ModuleError_Success;
}

// =============================================================================
Xen::t_ModuleError Xen::CModuleManager::Initialise()
{
	XMASSERT(m_lpModules.size(), "There are no modules to initialise.");

	if (m_lpModules.size() == 0)
		return ModuleError_ListEmpty;

	if (m_bInitialised)
		return ModuleError_AlreadyInitialised;

	// Initialise in order.
	XEN_LIST_FOREACH(t_ModuleList, ppModule, m_lpModules)
		(*ppModule)->OnInitialise();

	m_bInitialised = true;

	return ModuleError_Success;
}

// =============================================================================
Xen::t_ModuleError Xen::CModuleManager::Deinitialise()
{
	if (!m_bInitialised)
		return ModuleError_NotInitialised;

	// Deinitialise in reverse order.
	XEN_LIST_FOREACH_R(t_ModuleList, ppModule, m_lpModules)
		(*ppModule)->OnDeinitialise();

	m_bInitialised = false;
	m_lpModules.clear();

	return ModuleError_Success;
}

// =============================================================================
Xen::t_ModuleError Xen::CModuleManager::Update()
{
	if (!m_bInitialised)
		return ModuleError_NotInitialised;

	// Update in order.
	XEN_LIST_FOREACH(t_ModuleList, ppModule, m_lpModules)
		(*ppModule)->OnUpdate();

	return ModuleError_Success;
}

// =============================================================================
Xen::t_ModuleError Xen::CModuleManager::Event(xint iEventType, void* pEventInfo)
{
	if (!m_bInitialised)
		return ModuleError_NotInitialised;

	// Notify in order.
	XEN_LIST_FOREACH(t_ModuleList, ppModule, m_lpModules)
	{
		if ((*ppModule)->OnEvent(iEventType, pEventInfo))
			return ModuleError_Success;
	}

	return ModuleError_NotConsumed;
}
//##############################################################################