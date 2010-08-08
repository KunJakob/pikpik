//##############################################################################

// Global.
#include <Global.h>

// Local.
#include <Background.h>

// Other.
#include <Resource.h>

//##############################################################################

// =============================================================================
CBackgroundImage::CBackgroundImage(const xchar* pSpriteName) :
	m_iScrollInterval(25),
	m_xScrollVector(xpoint(1, 1)),
	m_pSprite(NULL),
	m_iTimer(0)
{
	m_pSprite = new CBasicSprite(_SPRITE(pSpriteName));
	m_xArea = xrect(0, 0, _SWIDTH + m_pSprite->GetImageWidth(), _SHEIGHT + m_pSprite->GetImageHeight());
}

// =============================================================================
CBackgroundImage::~CBackgroundImage()
{
	delete m_pSprite;
}

// =============================================================================
void CBackgroundImage::Update()
{
	m_iTimer += _TIMEDELTA;

	if (m_iTimer > m_iScrollInterval)
	{
		m_iTimer %= m_iScrollInterval;
		m_xOffset += m_xScrollVector;

		if (m_xOffset.m_tX < 0)
			m_xOffset.m_tX += m_pSprite->GetImageWidth();
		else if (m_xOffset.m_tX > (xint)m_pSprite->GetImageWidth())
			m_xOffset.m_tX -= m_pSprite->GetImageWidth();

		if (m_xOffset.m_tY < 0)
			m_xOffset.m_tY += m_pSprite->GetImageHeight();
		else if (m_xOffset.m_tY > (xint)m_pSprite->GetImageHeight())
			m_xOffset.m_tY -= m_pSprite->GetImageHeight();
	}
}

// =============================================================================
void CBackgroundImage::OnRender()
{
	m_pSprite->Render(m_xOffset * -1, m_xArea);
}

//##############################################################################