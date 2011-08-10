#include <Global.h>

//##############################################################################
// 
//	Copyright © 1998 - 2007, SAPIAN
//	--------------------------------------------------------------------------
//	Please see the corresponding header for further details.
// 
//##############################################################################

// Common.
#include <Xen/Common.h>

// Local.
#include <Xen/Screen.h>

// Xen.
#include <Xen/Module.h>

//##############################################################################

// =============================================================================
Xen::CScreen::CScreen(xint iScreenIndex) :
	m_iScreenIndex(iScreenIndex),
	m_pScreenParent(NULL),
	m_bScreenLoaded(false),
	m_bScreenActive(false),
	m_bScreenAwake(false)
{
	ScreenManager.RegisterScreen(this);
}

// =============================================================================
Xen::CScreen::~CScreen()
{
	ScreenManager.DeregisterScreen(this);
}

//##############################################################################

// =============================================================================
void Xen::CScreenManager::OnInitialise()
{
	m_lpPendingScreens.reserve(16);
	m_lpActiveScreens.reserve(16);
}

// =============================================================================
void Xen::CScreenManager::OnDeinitialise()
{
	Clear(true);
}

// =============================================================================
xbool Xen::CScreenManager::OnEvent(xint iEventType, void* pEventInfo)
{
	if (m_lpActiveScreens.size() > 0)
		return m_lpActiveScreens.back()->OnEvent(iEventType, pEventInfo);

	return false;
}

// =============================================================================
void Xen::CScreenManager::OnUpdate()
{
	ApplyPendingChanges();

	if (m_lpActiveScreens.size() > 0)
		m_lpActiveScreens.back()->OnUpdate();
}

// =============================================================================
void Xen::CScreenManager::PreRender()
{
	if (m_lpActiveScreens.size() > 0)
		m_lpActiveScreens.back()->OnPreRender();
}

// =============================================================================
void Xen::CScreenManager::Render()
{
	if (m_lpActiveScreens.size() > 0)
		m_lpActiveScreens.back()->OnRender();
}

// =============================================================================
void Xen::CScreenManager::PostRender()
{
	if (m_lpActiveScreens.size() > 0)
		m_lpActiveScreens.back()->OnPostRender();
}


// =============================================================================
void Xen::CScreenManager::LoadScreen(CScreen* pScreen)
{
	if (!pScreen->m_bScreenLoaded)
	{
		pScreen->OnLoad();
		pScreen->m_bScreenLoaded = true;
	}
}

// =============================================================================
void Xen::CScreenManager::LoadScreens()
{
	XEN_LIST_FOREACH(t_ScreenList, ppScreen, m_lpRegisteredScreens)
		LoadScreen(*ppScreen);
}

// =============================================================================
void Xen::CScreenManager::UnloadScreen(CScreen* pScreen)
{
	if (pScreen->m_bScreenLoaded)
	{
		pScreen->OnUnload();
		pScreen->m_bScreenLoaded = false;
	}
}

// =============================================================================
void Xen::CScreenManager::UnloadScreens()
{
	XEN_LIST_FOREACH(t_ScreenList, ppScreen, m_lpRegisteredScreens)
		UnloadScreen(*ppScreen);
}

// =============================================================================
void Xen::CScreenManager::ApplyPendingChanges()
{
	xint iActiveScreenCount = (xint)m_lpActiveScreens.size();
	xint iPendingScreenCount = (xint)m_lpPendingScreens.size();

	xint iFirstDifference = 0;
	xint iIterations = Math::Min(iActiveScreenCount, iPendingScreenCount);

	// Find the first difference between the pending and active screen list.
	while (iFirstDifference < iIterations && m_lpActiveScreens[iFirstDifference] == m_lpPendingScreens[iFirstDifference])
		++iFirstDifference;

	// If there are changes.
	if (iFirstDifference != iActiveScreenCount || iFirstDifference != iPendingScreenCount)
	{
		// Put the top most screen to sleep.
		if (m_lpActiveScreens.size())
		{
			m_lpActiveScreens.back()->OnSleep();
			m_lpActiveScreens.back()->m_bScreenAwake = false;
		}

		// Remove all the screens from the active list after the first difference in reverse order.
		for (xint iA = iActiveScreenCount; iA > iFirstDifference; --iA)
		{
			// Deactive the screen we're removing.
			m_lpActiveScreens.back()->OnDeactivate();
			m_lpActiveScreens.back()->m_bScreenActive = false;
			m_lpActiveScreens.back()->m_pScreenParent = NULL;

			// Remove the screen.
			m_lpActiveScreens.pop_back();
		}

		// Add all the newly added pending screens after the first difference.
		for (xint iA = iFirstDifference; iA < iPendingScreenCount; ++iA)
		{
			// Determine the screen's parent (if any).
			m_lpPendingScreens[iA]->m_pScreenParent = m_lpActiveScreens.size() ? m_lpActiveScreens.back() : NULL;

			// Add the screen.
			m_lpActiveScreens.push_back(m_lpPendingScreens[iA]);

			// Activate the screen we've adding.
			m_lpActiveScreens.back()->OnActivate();
			m_lpActiveScreens.back()->m_bScreenActive = true;
		}

		// Wake up the new top most screen.
		if (m_lpActiveScreens.size())
		{
			m_lpActiveScreens.back()->OnWake();
			m_lpActiveScreens.back()->m_bScreenAwake = true;
		}
	}
}

// =============================================================================
void Xen::CScreenManager::Clear(xbool bImmediate)
{
	m_lpPendingScreens.clear();

	if (bImmediate)
		ApplyPendingChanges();
}

// =============================================================================
Xen::t_ScreenError Xen::CScreenManager::Set(CScreen* pScreen, xbool bImmediate)
{
	Clear(bImmediate);
	return Push(pScreen, bImmediate);
}

// =============================================================================
Xen::t_ScreenError Xen::CScreenManager::Set(xint iScreenIndex, xbool bImmediate)
{
	return Set(FindScreen(iScreenIndex), bImmediate);
}

// =============================================================================
Xen::t_ScreenError Xen::CScreenManager::Push(CScreen* pScreen, xbool bImmediate)
{
	if (!pScreen)
		return ScreenError_InvalidState;

	m_lpPendingScreens.push_back(pScreen);

	if (bImmediate)
		ApplyPendingChanges();

	return bImmediate ? ScreenError_Success : ScreenError_Pending;
}

// =============================================================================
Xen::t_ScreenError Xen::CScreenManager::Push(xint iScreenIndex, xbool bImmediate)
{
	return Push(FindScreen(iScreenIndex), bImmediate);
}

// =============================================================================
Xen::t_ScreenError Xen::CScreenManager::Pop(xbool bImmediate)
{
	if (m_lpPendingScreens.size() == 0)
		return ScreenError_ListEmpty;

	m_lpPendingScreens.pop_back();

	if (bImmediate)
		ApplyPendingChanges();

	return bImmediate ? ScreenError_Success : ScreenError_Pending;
}

// =============================================================================
Xen::CScreen* Xen::CScreenManager::GetParent(CScreen* pScreen)
{
	return pScreen->m_pScreenParent;
}

// =============================================================================
Xen::CScreen* Xen::CScreenManager::FindScreen(xint iScreenIndex)
{
	XEN_LIST_FOREACH(t_ScreenList, ppScreen, m_lpRegisteredScreens)
	{
		if ((*ppScreen)->m_iScreenIndex == iScreenIndex)
			return *ppScreen;
	}

	return NULL;
}

// =============================================================================
void Xen::CScreenManager::RegisterScreen(Xen::CScreen* pScreen)
{
	XMASSERT(FindScreen(pScreen->GetScreenIndex()) == NULL, XFORMAT("Screen index %d is already used elsewhere.", pScreen->GetScreenIndex()));

	if (FindScreen(pScreen->GetScreenIndex()) == NULL)
		m_lpRegisteredScreens.push_back(pScreen);
}

// =============================================================================
void Xen::CScreenManager::DeregisterScreen(Xen::CScreen* pScreen)
{
	XEN_LIST_REMOVE(t_ScreenList, m_lpRegisteredScreens, pScreen);
}

//##############################################################################
