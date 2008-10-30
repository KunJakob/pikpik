#pragma once

/**
* @file Minimap.h
* @author Nat Ryall
* @date 24/07/2008
* @brief Allowing rendering of a map to a texture.
*
* Copyright © SAPIAN
*/

//##############################################################################
//
//                                  INCLUDE
//
//##############################################################################

// Global.
#include <Global.h>

// Other.
#include <Renderer.h>
#include <Map.h>

//##############################################################################

//##############################################################################
//
//                                   TYPES
//
//##############################################################################

// The minimap element bit flags.
enum t_MinimapElement
{
	MinimapElement_Floor		= XBIT(0),
	MinimapElement_Walls		= XBIT(1),
	MinimapElement_GhostWalls	= XBIT(2),
	MinimapElement_GhostBase	= XBIT(3),
	MinimapElement_Pellets		= XBIT(4),
	MinimapElement_Powers		= XBIT(5),
	MinimapElement_Traps		= XBIT(6),
	MinimapElement_Pacman		= XBIT(7),
	MinimapElement_Ghost		= XBIT(8),
	MinimapElement_PacmanSpawn	= XBIT(9),
	MinimapElement_Scanline		= XBIT(10),
};

//##############################################################################

//##############################################################################
//
//                                   MINIMAP
//
//##############################################################################
class CMinimap : public CRenderable
{
public:
	// Constructor.
	CMinimap(CMap* pMap);

	// Destructor.
	virtual ~CMinimap();

	// Render the minimap.
	virtual void Render();

	// Generate a minimap with the specified t_MinimapElement mask.
	HTEXTURE Generate(xuint iElementMask);

	// Get the minimap texture.
	inline HTEXTURE GetTexture()
	{
		return m_hMinimap;
	}

protected:
	// The current map we're working with.
	CMap* m_pMap;

	// The minimap texture.
	HTEXTURE m_hMinimap;

	// The minimap sprite.
	hgeSprite* m_pSprite;

	// The scanline offset.
	xint m_iScanlineOffset;

	// The actual dimensions of the minimap.
	xint m_iWidth;
	xint m_iHeight;

	// The texture dimensions of the minimap.
	xint m_iTextureWidth;
	xint m_iTextureHeight;
};

//##############################################################################