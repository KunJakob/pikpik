#pragma once

//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Global
#include <Global.h>

// Other.
#include <Renderer.h>
#include <Resource.h>

//##############################################################################

//##############################################################################
//
//                                   MACROS
//
//##############################################################################

// Find a specific sprite metadata using the resource manager.
#define _FONT(NAME) ((CFontMetadata*)ResourceManager.GetResourceMetadata(ResourceType_Font, NAME))

//##############################################################################

//##############################################################################
//
//                                 FONT FILE
//
//##############################################################################
class CFontFile : public CResourceFile
{
public:
	// Constructor: Initialise the file.
	CFontFile(const xchar* pFile);

	// Destructor: Clean up the file memory.
	virtual ~CFontFile();

	// Get the sprite object.
	hgeFont* GetResource()
	{
		return (hgeFont*)m_pResource;
	}
};

//##############################################################################

//##############################################################################
//
//                               FONT METADATA
//
//##############################################################################
class CFontMetadata : public CResourceMetadata
{
public:
	// Constructor: Initialise the metadata.
	CFontMetadata(CDataset* pDataset);

	// Destructor: Deinitialise the metadata.
	virtual ~CFontMetadata();

	// Get the font object.
	hgeFont* GetFont()
	{
		return m_pFile->GetResource();
	}

	// The font file used for rendering.
	CFontFile* m_pFile;

	// The font scale.
	xfloat m_fScale;

	// The font colour.
	xuint m_iColour;

	// The spacing between lines.
	xfloat m_fSpacing;

	// The spacing between letters.
	xfloat m_fTracking;

	// The font shadow offset.
	xpoint m_xShadowOffset;

	// The font shadow colour.
	xuint m_iShadowColour;
};

//##############################################################################

//##############################################################################
//
//                                    FONT
//
//##############################################################################
class CFont
{
public:
	// Constructor.
	CFont(CFontMetadata* pMetadata) : m_pMetadata(pMetadata) {}

	// Destructor.
	virtual ~CFont() {}

	// Render the font with the specified attributes.
	// ~pString The text string to render to the screen using this font.
	// ~xRect The aligned screen area to render in.
	// ~iAlign The font alignment relative to the position. Use HGETEXT_*.
	// ~fAlpha The alpha level between 0.0 and 1.0 to render the text at.
	void Render(const xchar* pString, xrect xRect, xuint iAlign, xfloat fAlpha);

	// Render the font with the specified attributes.
	// ~pString The text string to render to the screen using this font.
	// ~xPosition The horizontally-aligned screen position to render using.
	// ~iAlign The font alignment relative to the position. Use HGETEXT_*.
	// ~fAlpha The alpha level between 0.0 and 1.0 to render the text at.
	void Render(const xchar* pString, xpoint xPosition, xuint iAlign, xfloat fAlpha);

	// Render the font with the specified attributes.
	// ~pString The text string to render to the screen using this font.
	// ~xRect The aligned screen area to render in.
	// ~iAlign The font alignment relative to the position. Use HGETEXT_*.
	inline void Render(const xchar* pString, xrect xRect, xuint iAlign)
	{
		Render(pString, xRect, iAlign, 1.f);
	}

	// Render the font with the specified attributes.
	// ~pString The text string to render to the screen using this font.
	// ~xPosition The horizontally-aligned screen position to render using.
	// ~iAlign The font alignment relative to the position. Use HGETEXT_*.
	inline void Render(const xchar* pString, xpoint xPosition, xuint iAlign)
	{
		Render(pString, xPosition, iAlign, 1.f);
	}

	// Get the font height.
	xint GetFontHeight()
	{
		return (xint)m_pMetadata->GetFont()->GetHeight();
	}

	// Get the width of a string of characters using the specified font.
	xint GetStringWidth(const xchar* pString)
	{
		return (xint)m_pMetadata->GetFont()->GetStringWidth(pString);
	}

	// Get the metadata structure. This is for advanced use only.
	CFontMetadata* GetMetadata()
	{
		return m_pMetadata;
	}

protected:
	// The metadata for this object.
	CFontMetadata* m_pMetadata;
};

//##############################################################################

//##############################################################################
//
//                                    TEXT
//
//##############################################################################
class CText : public CFont, public CRenderable
{
public:
	// Constructor.
	CText(CFontMetadata* pMetadata);

	// Automatic render called by the render manager.
	virtual void OnRender();

	// Set the text string to render.
	void SetString(const xchar* pString)
	{
		m_xString = pString;
	}

	// Get the text string to be rendered.
	const xchar* GetString()
	{
		return m_xString.c_str();
	}

	// Set the screen position to render from.
	void SetPosition(xpoint xPosition)
	{
		m_xPosition = xPosition;
	}

	// Get the screen position to render from.
	xpoint GetPosition()
	{
		return m_xPosition;
	}

	// Set the text alignment. Use HGETEXT_*.
	void SetAlignment(xuint iAlignment)
	{
		m_iAlignment = iAlignment;
	}

	// Get the text alignment.
	xuint GetAlignment()
	{
		return m_iAlignment;
	}

	// Get the width of the current text string using the specified font.
	xuint GetStringWidth()
	{
		return CFont::GetStringWidth(GetString());
	}

protected:
	// The text string to render.
	xstring m_xString;

	// The screen position to render at.
	xpoint m_xPosition;

	// The text alignment relative to the position.
	xuint m_iAlignment;
};

//##############################################################################
