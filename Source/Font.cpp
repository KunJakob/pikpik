#pragma region Include
//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Global.
#include <Global.h>

// Local.
#include <Font.h>

//##############################################################################
#pragma endregion

#pragma region Font
//##############################################################################
//
//                                    FONT
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         18-Apr-2008
// =============================================================================
void CShadowFont::RenderShadowed(float fX, float fY, int iAlign, const XCHAR* pText)
{
	DWORD iColour = GetColor();
	
	SetColor(m_iShadowColour);
	Render(fX + (XFLOAT)m_xShadowOffset.iX, fY + (XFLOAT)m_xShadowOffset.iY, iAlign, pText);

	SetColor(iColour);
	Render(fX, fY, iAlign, pText);
}

//##############################################################################
#pragma endregion