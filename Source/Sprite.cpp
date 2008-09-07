//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Global.
#include <Global.h>

// Local.
#include <Sprite.h>

//##############################################################################

//##############################################################################
//
//                                 SPRITE FILE
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         22-Apr-2008
// =============================================================================
CSpriteFile::CSpriteFile(const xchar* pFile) : CResourceFile(ResourceType_Sprite, pFile)
{
	HTEXTURE hTexture = _HGE->Texture_Load(pFile);
	m_pResource = new hgeSprite(hTexture, 0.0f, 0.0f, (float)_HGE->Texture_GetWidth(hTexture), (float)_HGE->Texture_GetHeight(hTexture));
}

// =============================================================================
// Nat Ryall                                                         22-Apr-2008
// =============================================================================
CSpriteFile::~CSpriteFile()
{
	_HGE->Texture_Free(GetResource()->GetTexture());
	delete GetResource();
}

//##############################################################################

//##############################################################################
//
//                               SPRITE METADATA
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         22-Apr-2008
// =============================================================================
CSpriteMetadata::CSpriteMetadata(CDataset* pDataset) : CResourceMetadata(ResourceType_Sprite, pDataset)
{
	// File.
	m_pFile = (CSpriteFile*)ResourceManager::CreateResourceFile(ResourceType_Sprite, pDataset->GetProperty("File")->GetString());

	// Areas.
	_DATASET_FOREACH(pAreaDataset, pDataset, "Area", NULL)
	{
		CArea* pArea = new CArea;
		m_lpAreas.push_back(pArea);

		pArea->m_pName = pAreaDataset->GetName();

		if (_PROPERTY_EXISTS(pAreaDataset, "Rect"))
			pArea->m_xRect = _PROPERTY->GetRect();
		else if (_PROPERTY_EXISTS(pAreaDataset, "Size"))
			pArea->m_xRect = xrect(_PROPERTY->GetPoint(0), _PROPERTY->GetPoint(0) + _PROPERTY->GetPoint(1));
		else if (_PROPERTY_EXISTS(pAreaDataset, "Tile"))
			pArea->m_xRect = xrect(_PROPERTY->GetInt(0) * _PROPERTY->GetInt(1), 0, (_PROPERTY->GetInt(0) + 1) * _PROPERTY->GetInt(1), _PROPERTY->GetInt(2));
	}

	// Animations.
	_DATASET_FOREACH(pAnimationDataset, pDataset, "Animation", NULL)
	{
		CAnimation* pAnimation = new CAnimation;
		m_lpAnimations.push_back(pAnimation);

		pAnimation->m_pName = pAnimationDataset->GetName();
		pAnimation->m_iAnimationTime = 0;

		xuint iAnimationDelay = 0;

		if (_PROPERTY_EXISTS(pAnimationDataset, "Delay"))
			iAnimationDelay = _PROPERTY->GetInt();

		CFrame* pPrevFrame = NULL;

		// Frames.
		_DATASET_FOREACH(pFrameDataset, pAnimationDataset, "Frame", NULL)
		{
			CFrame* pFrame = new CFrame;
			pAnimation->m_lpFrames.push_back(pFrame);

			pFrame->m_pName = NULL;
			pFrame->m_iDelay = iAnimationDelay;
			pFrame->m_pEvent = NULL;
			pFrame->m_pNextFrame = NULL;

			if (pFrameDataset->GetName())
				pFrame->m_pName = pFrameDataset->GetName();

			pFrame->m_pArea = FindArea(pFrameDataset->GetProperty("Area")->GetString());

			if (_PROPERTY_EXISTS(pAnimationDataset, "Delay"))
				pFrame->m_iDelay = (xuint)_PROPERTY->GetInt();

			if (_PROPERTY_EXISTS(pFrameDataset, "Event"))
				pFrame->m_pEvent = _PROPERTY->GetString();

			if (pPrevFrame && pPrevFrame->m_pNextFrame == NULL)
				pPrevFrame->m_pNextFrame = pFrame;

			pFrame->m_pNextFrame = NULL;
			pPrevFrame = pFrame;

			if (_PROPERTY_EXISTS(pFrameDataset, "Loop"))
				pFrame->m_pNextFrame = pAnimation->m_lpFrames.front();
			else if (_PROPERTY_EXISTS(pFrameDataset, "Goto"))
				pFrame->m_pNextFrame = FindFrame(pAnimation, _PROPERTY->GetString());

			pAnimation->m_iAnimationTime += pFrame->m_iDelay;
		}
	}
}

// =============================================================================
// Nat Ryall                                                         23-Apr-2008
// =============================================================================
CSpriteMetadata::~CSpriteMetadata()
{
	ResourceManager::ReleaseResourceFile(m_pFile);

	XEN_LIST_ERASE_ALL(m_lpAreas);

	while (m_lpAnimations.size())
	{
		XEN_LIST_ERASE_ALL(m_lpAnimations.front()->m_lpFrames);

		delete m_lpAnimations.front();
		m_lpAnimations.pop_front();
	}
}

// =============================================================================
// Nat Ryall                                                         23-Apr-2008
// =============================================================================
CSpriteMetadata::CArea* CSpriteMetadata::FindArea(const xchar* pName)
{
	XEN_LIST_FOREACH(t_AreaList, ppArea, m_lpAreas)
	{
		if (String::IsMatch((*ppArea)->m_pName, pName))
			return *ppArea;
	}

	return NULL;
}

// =============================================================================
// Nat Ryall                                                         23-Apr-2008
// =============================================================================
CSpriteMetadata::CFrame* CSpriteMetadata::FindFrame(CAnimation* pAnimation, const xchar* pName)
{
	XEN_LIST_FOREACH(t_FrameList, ppFrame, pAnimation->m_lpFrames)
	{
		if (String::IsMatch((*ppFrame)->m_pName, pName))
			return *ppFrame;
	}

	return NULL;
}

// =============================================================================
// Nat Ryall                                                         23-Apr-2008
// =============================================================================
CSpriteMetadata::CAnimation* CSpriteMetadata::FindAnimation(const xchar* pName)
{
	XEN_LIST_FOREACH(t_AnimationList, ppAnimation, m_lpAnimations)
	{
		if (String::IsMatch((*ppAnimation)->m_pName, pName))
			return *ppAnimation;
	}

	return NULL;
}

//##############################################################################

//##############################################################################
//
//                                BASIC SPRITE
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         23-Apr-2008
// =============================================================================
void CBasicSprite::Render(xpoint xPosition, xpoint xAnchor, xrect xArea, xfloat fAlpha, xfloat fAngle)
{
	hgeSprite* pSprite = m_pMetadata->GetSprite();

	pSprite->SetHotSpot((float)xAnchor.iX, (float)xAnchor.iY);
	pSprite->SetColor(SETA(pSprite->GetColor(), (xchar)(fAlpha * 255.f)));
	pSprite->SetTextureRect((float)xArea.iLeft, (float)xArea.iTop, (float)xArea.Width(), (float)xArea.Height());

	if (fAngle)
		pSprite->RenderEx((float)xPosition.iX, (float)xPosition.iY, fAngle);
	else
		pSprite->Render((float)xPosition.iX, (float)xPosition.iY);
};

// =============================================================================
// Nat Ryall                                                         21-May-2008
// =============================================================================
void CBasicSprite::RenderTiled(xpoint xPosition, xpoint xSize, xpoint xAnchor, xrect xArea, xfloat fAlpha)
{
	xpoint xOffset;
	xpoint xTile;

	if (xSize.iY == 0)
		xSize.iY = xArea.Height();

	if (xSize.iX == 0)
		xSize.iX = xArea.Width();

	while (xOffset.iY < xSize.iY)
	{
		xTile.iY = Math::Clamp<xint>(xSize.iY - xOffset.iY, 0, xArea.Height());

		while (xOffset.iX < xSize.iX)
		{
			xTile.iX = Math::Clamp<xint>(xSize.iX - xOffset.iX, 0, xArea.Width());

			xrect xTileArea = xArea + xrect(0, 0, xTile.iX - xArea.Width(), xTile.iY - xArea.Height());
			Render(xPosition + xOffset, xAnchor, xTileArea, fAlpha, 0.f);

			xOffset.iX += xTile.iX;
		}

		xOffset.iX = 0;
		xOffset.iY += xTile.iY;
	}
}

//##############################################################################

//##############################################################################
//
//                                   SPRITE
//
//##############################################################################

// =============================================================================
// Author: Nat Ryall                                           Date: 19-Feb-2008
// =============================================================================
CSprite::CSprite(CSpriteMetadata* pMetadata) : CBasicSprite(pMetadata), CRenderable(RenderableType_Sprite),
	m_pArea(NULL),
	m_fAlpha(1.f),
	m_fAngle(0.f)
{
}

// =============================================================================
// Author: Nat Ryall                                           Date: 12-Feb-2008
// =============================================================================
void CSprite::Render()
{
	CBasicSprite::Render(m_xPosition, m_xAnchor, m_pArea ? m_pArea->m_xRect : GetImageRect(), m_fAlpha, m_fAngle);
}

//##############################################################################

//##############################################################################
//
//                               ANIMATED SPRITE
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         16-Apr-2008
// =============================================================================
CAnimatedSprite::CAnimatedSprite(CSpriteMetadata* pMetadata) : CSprite(pMetadata),
	m_pFrame(NULL),
	m_pAnimation(NULL),
	m_bPlaying(false),
	m_iFrameTimer(0),
	m_fpEvent(NULL)
{
}

// =============================================================================
// Nat Ryall                                                         23-Apr-2008
// =============================================================================
void CAnimatedSprite::Update()
{
	if (m_bPlaying)
	{
		if (m_pFrame->m_iDelay)
			m_iFrameTimer += _TIMEDELTA;

		while (m_iFrameTimer >= m_pFrame->m_iDelay)
		{
			if (m_pFrame->m_pNextFrame)
			{
				m_iFrameTimer -= m_pFrame->m_iDelay;
				SetFrame(m_pFrame->m_pNextFrame);
			}
			else
			{
				m_bPlaying = false;
				break;
			}
		}
	}
}

// =============================================================================
// Nat Ryall                                                         16-Apr-2008
// =============================================================================
void CAnimatedSprite::Render()
{
	CSprite::Render();
}

// =============================================================================
// Nat Ryall                                                         16-Apr-2008
// =============================================================================
void CAnimatedSprite::SetFrame(CSpriteMetadata::CFrame* pFrame)
{
	m_pFrame = pFrame;
	m_pArea = m_pFrame->m_pArea;

	if (m_pFrame->m_pEvent && m_fpEvent)
		m_fpEvent(this, m_pFrame->m_pEvent);
}

// =============================================================================
// Nat Ryall                                                         16-Apr-2008
// =============================================================================
void CAnimatedSprite::SetAnimation(CSpriteMetadata::CAnimation* pAnimation, xbool bPlay)
{
	m_bPlaying = bPlay;
	m_iFrameTimer = 0;

	m_pAnimation = pAnimation;
	SetFrame(pAnimation->m_lpFrames.front());
}

//##############################################################################
