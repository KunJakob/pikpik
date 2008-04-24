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
#include <Sprite.h>

//##############################################################################
#pragma endregion

#pragma region Background
//##############################################################################
//
//                                 BACKGROUND
//
//##############################################################################
class CBackground : public CRenderable
{
public:
	/**
	* Constructor.
	*/
	CBackground() : CRenderable(RenderableType_Background), iBackgroundColour(0xFF000000) {}

	/**
	* Destructor.
	*/
	virtual ~CBackground() {}

	/**
	* Render the background.
	*/
	virtual void Render()
	{
		_HGE->Gfx_Clear(iBackgroundColour);
	}

	// The background colour.
	XUINT iBackgroundColour;
};

//##############################################################################
#pragma endregion

#pragma region Background Image
//##############################################################################
//
//                              BACKGROUND IMAGE
//
//##############################################################################
class CBackgroundImage : public CBackground
{
public:
	/**
	* Constructor.
	*/
	CBackgroundImage(const XCHAR* pSpriteName);

	/**
	* Destructor.
	*/
	virtual ~CBackgroundImage();

	/**
	* Update the background.
	*/
	virtual void Update();

	/**
	* Render the background.
	*/
	virtual void Render();

	// The interval to scroll at.
	XUINT iScrollInterval;

	// The background scrolling speed.
	XPOINT xScrollSpeed;

protected:
	// The background sprite.
	CBasicSprite* m_pSprite;

	// The background sprite image area.
	XRECT m_xArea;

	// The internal timer used to scroll the background.
	XUINT m_iTimer;

	// The menu scrolling offset.
	XPOINT m_xOffset;
};

//##############################################################################
#pragma endregion
