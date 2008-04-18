#pragma once

#pragma region Include
//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Global
#include <Global.h>

//##############################################################################
#pragma endregion

#pragma region Font
//##############################################################################
//
//                                    FONT
//
//##############################################################################
class CShadowFont : public hgeFont
{
public:
	/**
	* Constructor.
	*/
	CShadowFont(const XCHAR* pFontFile, XBOOL bMipmap = false) : hgeFont(pFontFile, bMipmap), m_iShadowColour(0), m_xShadowOffset(XPOINT(2, 2)) {}

	/**
	* Render the font with a shadow.
	*/
	void RenderShadowed(float fX, float fY, int iAlign, const XCHAR* pText);

	/**
	* Set the shadow text colour.
	*/
	void SetShadowColor(DWORD iShadowColour)
	{
		m_iShadowColour = iShadowColour;	
	}

	/**
	* Get the shadow text colour.
	*/
	DWORD GetShadowColor()
	{
		return m_iShadowColour;
	}

	/**
	* Set the offset of the shadow to the text.
	*/
	void SetShadowOffset(XPOINT xShadowOffset)
	{
		m_xShadowOffset = xShadowOffset;
	}

	/**
	* Get the offset of the shadow to the text.
	*/
	XPOINT GetShadowOffset()
	{
		return m_xShadowOffset;
	}

protected:
	// The colour of the shadow.
	DWORD m_iShadowColour;

	// The offset of the shadow.
	XPOINT m_xShadowOffset;
};

//##############################################################################
#pragma endregion