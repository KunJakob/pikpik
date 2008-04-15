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
  XBOOL Update();

  /**
  * The game render callback.
  */
  XBOOL Render();

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
	XUINT GetTimeDelta();
}

//##############################################################################
#pragma endregion