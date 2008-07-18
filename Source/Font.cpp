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

//##############################################################################
//
//                                 FONT FILE
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         24-Apr-2008
// =============================================================================
CFontFile::CFontFile(const xchar* pFile) : CResourceFile(ResourceType_Font, pFile)
{
	pResource = new hgeFont(pFile);
}

// =============================================================================
// Nat Ryall                                                         24-Apr-2008
// =============================================================================
CFontFile::~CFontFile()
{
	delete GetResource();
}

//##############################################################################

//##############################################################################
//
//                               FONT METADATA
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         24-Apr-2008
// =============================================================================
CFontMetadata::CFontMetadata(CDataset* pDataset) : CResourceMetadata(ResourceType_Font, pDataset)
{
	// File.
	pFile = (CFontFile*)ResourceManager::AllocResourceFile(ResourceType_Font, pDataset->GetProperty("File")->GetString());

	// Info.
	fScale = 1.f;
	iColour = 0xFFFFFFFF;
	fSpacing = 1.f;
	fTracking = 0.f;
	xShadowOffset = xpoint();
	iShadowColour = 0xFF000000;

	if (_PROPERTY_EXISTS(pDataset, "Scale"))
		fScale = _PROPERTY->GetFloat();

	if (_PROPERTY_EXISTS(pDataset, "Colour"))
		iColour = ARGB(255, _PROPERTY->GetInt(0), _PROPERTY->GetInt(1), _PROPERTY->GetInt(2));

	if (_PROPERTY_EXISTS(pDataset, "Spacing"))
		fSpacing = _PROPERTY->GetFloat();

	if (_PROPERTY_EXISTS(pDataset, "Tracking"))
		fTracking = _PROPERTY->GetFloat();

	if (CDataset* pShadowDataset = pDataset->GetDataset("Shadow"))
	{
		if (_PROPERTY_EXISTS(pShadowDataset, "Offset"))
			xShadowOffset = _PROPERTY->GetPoint();

		if (_PROPERTY_EXISTS(pShadowDataset, "Colour"))
			iShadowColour = ARGB(255, _PROPERTY->GetInt(0), _PROPERTY->GetInt(1), _PROPERTY->GetInt(2));
	}
}

// =============================================================================
// Nat Ryall                                                         24-Apr-2008
// =============================================================================
CFontMetadata::~CFontMetadata()
{
	ResourceManager::ReleaseResourceFile(pFile);
}

//##############################################################################

//##############################################################################
//
//                                    FONT
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         24-Apr-2008
// =============================================================================
void CFont::Render(const xchar* pString, xpoint xPosition, xuint iAlign, xfloat fAlpha)
{
	hgeFont* pFont = m_pMetadata->GetFont();

	pFont->SetScale(m_pMetadata->fScale);
	pFont->SetSpacing(m_pMetadata->fSpacing);
	pFont->SetTracking(m_pMetadata->fTracking);

	if (!m_pMetadata->xShadowOffset.IsZero())
	{
		xpoint xShadowPosition = xPosition + m_pMetadata->xShadowOffset;

		pFont->SetColor(SETA(m_pMetadata->iShadowColour, (xchar)(fAlpha * 255.f)));
		pFont->Render((xfloat)xShadowPosition.iX, (xfloat)xShadowPosition.iY, iAlign, pString);
	}

	pFont->SetColor(SETA(m_pMetadata->iColour, (xchar)(fAlpha * 255.f)));
	pFont->Render((xfloat)xPosition.iX, (xfloat)xPosition.iY, iAlign, pString);
}

// =============================================================================
// Nat Ryall                                                         13-May-2008
// =============================================================================
void CFont::Render(const xchar* pString, xrect xRect, xuint iAlign, xfloat fAlpha)
{
	hgeFont* pFont = m_pMetadata->GetFont();

	pFont->SetScale(m_pMetadata->fScale);
	pFont->SetSpacing(m_pMetadata->fSpacing);
	pFont->SetTracking(m_pMetadata->fTracking);

	if (!m_pMetadata->xShadowOffset.IsZero())
	{
		xrect xShadowRect = xRect + m_pMetadata->xShadowOffset;

		pFont->SetColor(SETA(m_pMetadata->iShadowColour, (xchar)(fAlpha * 255.f)));
		pFont->printfb((xfloat)xShadowRect.iLeft, (xfloat)xShadowRect.iTop, (xfloat)xShadowRect.Width(), (xfloat)xShadowRect.Height(), iAlign, pString);
	}

	pFont->SetColor(SETA(m_pMetadata->iColour, (xchar)(fAlpha * 255.f)));
	pFont->printfb((xfloat)xRect.iLeft, (xfloat)xRect.iTop, (xfloat)xRect.Width(), (xfloat)xRect.Height(), iAlign, pString);
}

//##############################################################################

//##############################################################################
//
//                                    TEXT
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         24-Apr-2008
// =============================================================================
CText::CText(CFontMetadata* pMetadata) : CFont(pMetadata),
	m_iAlignment(HGETEXT_LEFT)
{
}

// =============================================================================
// Nat Ryall                                                         24-Apr-2008
// =============================================================================
void CText::Render()
{
	CFont::Render(m_xString.c_str(), m_xPosition, m_iAlignment);
}

//##############################################################################