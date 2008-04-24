#pragma region Include
//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Global.
#include <Global.h>

// Local.
#include <Background.h>

// Other.
#include <Resource.h>

//##############################################################################
#pragma endregion

#pragma region Background Image
//##############################################################################
//
//                              BACKGROUND IMAGE
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         21-Apr-2008
// =============================================================================
CBackgroundImage::CBackgroundImage(const XCHAR* pSpriteName) :
	iScrollInterval(25),
	xScrollSpeed(XPOINT(1, 1)),
	m_pSprite(NULL),
	m_iTimer(0)
{
	m_pSprite = new CBasicSprite(_SPRITE(pSpriteName));
	m_xArea = XRECT(0, 0, _SWIDTH + m_pSprite->GetImageWidth(), _SHEIGHT + m_pSprite->GetImageHeight());
}

// =============================================================================
// Nat Ryall                                                         21-Apr-2008
// =============================================================================
CBackgroundImage::~CBackgroundImage()
{
	delete m_pSprite;
}

// =============================================================================
// Nat Ryall                                                         21-Apr-2008
// =============================================================================
void CBackgroundImage::Update()
{
	m_iTimer += _TIMEDELTA;

	if (m_iTimer > iScrollInterval)
	{
		m_iTimer %= iScrollInterval;

		m_xOffset += xScrollSpeed;
		m_xOffset %= XPOINT(m_pSprite->GetImageWidth(), m_pSprite->GetImageHeight());
	}
}

// =============================================================================
// Nat Ryall                                                         21-Apr-2008
// =============================================================================
void CBackgroundImage::Render()
{
	m_pSprite->Render(m_xArea, XPOINT(), m_xOffset * -1, 1.f, 0.f);
}

//##############################################################################
#pragma endregion