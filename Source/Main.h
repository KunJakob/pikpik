#pragma once

#pragma region Include
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
#pragma endregion

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

#pragma region Declaration
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
#pragma endregion