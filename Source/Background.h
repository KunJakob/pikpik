#pragma once

#pragma region Include
//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Global.
#include <Global.h>

// Other.
#include <Renderer.h>

//##############################################################################
#pragma endregion

#pragma region Base
//##############################################################################
//
//                                    BASE
//
//##############################################################################
class CBackground : public CRenderable
{
public:
	/**
	* Constructor.
	*/
	CBackground() : CRenderable(RenderableType_Background) {}

	/**
	* Render the object.
	*/
	virtual void Render()
	{
		_HGE->Gfx_Clear(0);
	}
};

//##############################################################################
#pragma endregion