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
	m_pResource = new hgeFont(pFile);
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
	m_pFile = (CFontFile*)ResourceManager.CreateResourceFile(ResourceType_Font, pDataset->GetProperty("File")->GetString());

	// Info.
	m_fScale = 1.f;
	m_iColour = 0xFFFFFFFF;
	m_fSpacing = 1.f;
	m_fTracking = 0.f;
	m_xShadowOffset = xpoint();
	m_iShadowColour = 0xFF000000;

	if (XEN_METADATA_PROPERTY_EXISTS(pDataset, "Scale"))
		m_fScale = XEN_METADATA_PROPERTY->GetFloat();

	if (XEN_METADATA_PROPERTY_EXISTS(pDataset, "Colour"))
		m_iColour = ARGB(255, XEN_METADATA_PROPERTY->GetInt(0), XEN_METADATA_PROPERTY->GetInt(1), XEN_METADATA_PROPERTY->GetInt(2));

	if (XEN_METADATA_PROPERTY_EXISTS(pDataset, "Spacing"))
		m_fSpacing = XEN_METADATA_PROPERTY->GetFloat();

	if (XEN_METADATA_PROPERTY_EXISTS(pDataset, "Tracking"))
		m_fTracking = XEN_METADATA_PROPERTY->GetFloat();

	if (CDataset* pShadowDataset = pDataset->GetDataset("Shadow"))
	{
		if (XEN_METADATA_PROPERTY_EXISTS(pShadowDataset, "Offset"))
			m_xShadowOffset = XEN_METADATA_PROPERTY->GetPoint();

		if (XEN_METADATA_PROPERTY_EXISTS(pShadowDataset, "Colour"))
			m_iShadowColour = ARGB(255, XEN_METADATA_PROPERTY->GetInt(0), XEN_METADATA_PROPERTY->GetInt(1), XEN_METADATA_PROPERTY->GetInt(2));
	}
}

// =============================================================================
// Nat Ryall                                                         24-Apr-2008
// =============================================================================
CFontMetadata::~CFontMetadata()
{
	ResourceManager.ReleaseResourceFile(m_pFile);
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

	pFont->SetScale(m_pMetadata->m_fScale);
	pFont->SetSpacing(m_pMetadata->m_fSpacing);
	pFont->SetTracking(m_pMetadata->m_fTracking);

	if (!m_pMetadata->m_xShadowOffset.IsZero())
	{
		xpoint xShadowPosition = xPosition + m_pMetadata->m_xShadowOffset;

		pFont->SetColor(SETA(m_pMetadata->m_iShadowColour, (xchar)(fAlpha * 255.f)));
		pFont->Render((xfloat)xShadowPosition.iX, (xfloat)xShadowPosition.iY, iAlign, pString);
	}

	pFont->SetColor(SETA(m_pMetadata->m_iColour, (xchar)(fAlpha * 255.f)));
	pFont->Render((xfloat)xPosition.iX, (xfloat)xPosition.iY, iAlign, pString);
}

// =============================================================================
// Nat Ryall                                                         13-May-2008
// =============================================================================
void CFont::Render(const xchar* pString, xrect xRect, xuint iAlign, xfloat fAlpha)
{
	hgeFont* pFont = m_pMetadata->GetFont();

	pFont->SetScale(m_pMetadata->m_fScale);
	pFont->SetSpacing(m_pMetadata->m_fSpacing);
	pFont->SetTracking(m_pMetadata->m_fTracking);

	if (!m_pMetadata->m_xShadowOffset.IsZero())
	{
		xrect xShadowRect = xRect + m_pMetadata->m_xShadowOffset;

		pFont->SetColor(SETA(m_pMetadata->m_iShadowColour, (xchar)(fAlpha * 255.f)));
		pFont->printfb((xfloat)xShadowRect.iLeft, (xfloat)xShadowRect.iTop, (xfloat)xShadowRect.Width(), (xfloat)xShadowRect.Height(), iAlign, pString);
	}

	pFont->SetColor(SETA(m_pMetadata->m_iColour, (xchar)(fAlpha * 255.f)));
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
CText::CText(CFontMetadata* pMetadata) : CFont(pMetadata), CRenderable(RenderableType_Text),
	m_iAlignment(HGETEXT_LEFT)
{
}

// =============================================================================
// Nat Ryall                                                         24-Apr-2008
// =============================================================================
void CText::OnRender()
{
	CFont::Render(m_xString.c_str(), m_xPosition, m_iAlignment);
}

//##############################################################################