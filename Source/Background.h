#pragma once

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
	xuint iBackgroundColour;
};

//##############################################################################

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
	CBackgroundImage(const xchar* pSpriteName);

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
	xuint iScrollInterval;

	// The background scrolling speed.
	xpoint xScrollVector;

protected:
	// The background sprite.
	CBasicSprite* m_pSprite;

	// The background sprite image area.
	xrect m_xArea;

	// The internal timer used to scroll the background.
	xuint m_iTimer;

	// The menu scrolling offset.
	xpoint m_xOffset;
};

//##############################################################################
