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
	/**
	* Initialise the application.
	*/
	void Initialise();

	/**
	* Deinitialise the application.
	*/
	void Deinitialise();

	/**
	* The game update callback.
	*/
	xbool Update();

	/**
	* The game render callback.
	*/
	xbool Render();

	/**
	* Call to terminate the application.
	*/
	void Terminate();

	/**
	* Get the application renderer interface.
	*/
	HGE* GetInterface();

	/**
	* Get the current time delta in milliseconds.
	*/
	xuint GetTimeDelta();

	/**
	* Get the sound interface.
	*/
	FMOD::System* GetSoundSystem();
}

//##############################################################################