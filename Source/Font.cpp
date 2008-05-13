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
CFontFile::CFontFile(const XCHAR* pFile) : CResourceFile(ResourceType_Font, pFile)
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
	xShadowOffset = XPOINT();
	iShadowColour = 0xFF000000;

	if (CProperty* pProperty = pDataset->GetProperty("Scale"))
		fScale = pProperty->GetFloat();

	if (CProperty* pProperty = pDataset->GetProperty("Colour"))
		iColour = ARGB(255, pProperty->GetInt(0), pProperty->GetInt(1), pProperty->GetInt(2));

	if (CDataset* pShadowDataset = pDataset->GetDataset("Shadow"))
	{
		if (CProperty* pProperty = pShadowDataset->GetProperty("Offset"))
			xShadowOffset = pProperty->GetPoint();

		if (CProperty* pProperty = pShadowDataset->GetProperty("Colour"))
			iShadowColour = ARGB(255, pProperty->GetInt(0), pProperty->GetInt(1), pProperty->GetInt(2));
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
void CFont::Render(const xchar* pString, xpoint xPosition, xuint iAlign)
{
	hgeFont* pFont = m_pMetadata->GetFont();

	pFont->SetScale(m_pMetadata->fScale);

	if (!m_pMetadata->xShadowOffset.IsZero())
	{
		xpoint xShadowPosition = xPosition + m_pMetadata->xShadowOffset;

		pFont->SetColor(m_pMetadata->iShadowColour);
		pFont->Render((xfloat)xShadowPosition.iX, (xfloat)xShadowPosition.iY, iAlign, pString);
	}

	pFont->SetColor(m_pMetadata->iColour);
	pFont->Render((xfloat)xPosition.iX, (xfloat)xPosition.iY, iAlign, pString);
}

// =============================================================================
// Nat Ryall                                                         13-May-2008
// =============================================================================
void CFont::Render(const xchar* pString, xrect xRect, xuint iAlign)
{
	hgeFont* pFont = m_pMetadata->GetFont();

	pFont->SetScale(m_pMetadata->fScale);
	
	if (!m_pMetadata->xShadowOffset.IsZero())
	{
		xrect xShadowRect = xRect + m_pMetadata->xShadowOffset;

		pFont->SetColor(m_pMetadata->iShadowColour);
		pFont->printfb((xfloat)xShadowRect.iLeft, (xfloat)xShadowRect.iTop, (xfloat)xShadowRect.Width(), (xfloat)xShadowRect.Height(), iAlign, pString);
	}

	pFont->SetColor(m_pMetadata->iColour);
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