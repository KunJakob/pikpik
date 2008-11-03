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
	// Constructor.
	CBackground() : CRenderable(RenderableType_Background), iBackgroundColour(0xFF000000) {}

	// Destructor.
	virtual ~CBackground() {}

	// Render the background.
	virtual void OnRender()
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
	// Constructor.
	CBackgroundImage(const xchar* pSpriteName);

	// Destructor.
	virtual ~CBackgroundImage();

	// Update the background.
	void Update();

	// Render the background.
	virtual void OnRender();

	// Set the scrolling properties.
	inline void SetScroll(xuint iScrollInterval, xpoint xScrollVector)
	{
		m_iScrollInterval = iScrollInterval;
		m_xScrollVector = xScrollVector;
	}

protected:
	// The background sprite.
	CBasicSprite* m_pSprite;

	// The background sprite image area.
	xrect m_xArea;

	// The internal timer used to scroll the background.
	xuint m_iTimer;

	// The menu scrolling offset.
	xpoint m_xOffset;

	// The interval to scroll at.
	xuint m_iScrollInterval;

	// The background scrolling speed.
	xpoint m_xScrollVector;
};

//##############################################################################
