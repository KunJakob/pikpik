#pragma once

//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Xen.
#include <Xen/Xen.h>

// HGE.
#include <hge/hge.h>

//##############################################################################

//##############################################################################
//
//                                   MACROS
//
//##############################################################################

// Add a module to the module system.
#define _MODULE(MODULECLASS) \
		{ static MODULECLASS xModuleInstance; XMODULE(&xModuleInstance); }

//##############################################################################

//##############################################################################
//
//                                   TYPES
//
//##############################################################################

// Predeclare
namespace FMOD
{
	class System;
}

// Namespace.
using namespace Xen;

//##############################################################################

//##############################################################################
//
//                                 DECLARATION
//
//##############################################################################
namespace Application
{
	// Initialise the application.
	void Initialise();

	// Deinitialise the application.
	void Deinitialise();

	// The game update callback.
	xbool OnUpdate();

	// The game render callback.
	xbool OnRender();

	// The focus was lost from the application.
	xbool OnBlur();

	// The focus was gained on the application.
	xbool OnFocus();

	// Call to terminate the application.
	void Terminate();

	// Get the application renderer interface.
	HGE* GetInterface();

	// Get the current time delta in milliseconds.
	xuint GetTimeDelta();
}

//##############################################################################