#pragma once

/**
* @file Radar.h
* @author Nat Ryall
* @date 27/05/2008
* @brief Rendering of the map radar.
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

//##############################################################################

//##############################################################################
//
//                                RADAR SCREEN
//
//##############################################################################
class CRadarScreen
{
public:
protected:
};

//##############################################################################

// The minimap element bit flags.
enum t_MinimapElement
{
	MinimapElement_Floor		= XBIT(0),
	MinimapElement_Walls		= XBIT(1),
	MinimapElement_Base			= XBIT(2),
	MinimapElement_Pellets		= XBIT(3),
	MinimapElement_Powers		= XBIT(4),
	MinimapElement_Traps		= XBIT(5),
	MinimapElement_Pacman		= XBIT(6),
	MinimapElement_Ghost		= XBIT(7),
	MinimapElement_PacmanStart	= XBIT(8),
	MinimapElement_Scanline		= XBIT(9),
};

/*class CMinimap
{
public:
	// Constructor.
	CMinimap(CMap* pMap); // Create a new texture and initialise.

	// Destructor.
	virtual ~CMinimap(); // Free the texture.

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
	
	// The scanline offset.
	xint iScanlineOffset;
};*/