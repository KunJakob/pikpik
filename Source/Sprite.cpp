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
	pResource = new hgeSprite(hTexture, 0.0f, 0.0f, (float)_HGE->Texture_GetWidth(hTexture), (float)_HGE->Texture_GetHeight(hTexture));
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
	pFile = (CSpriteFile*)ResourceManager::AllocResourceFile(ResourceType_Sprite, pDataset->GetProperty("File")->GetString());

	// Areas.
	_DATASET_FOREACH(pAreaDataset, pDataset, "Area", NULL)
	{
		CArea* pArea = new CArea;
		lpAreas.push_back(pArea);

		pArea->pName = pAreaDataset->GetName();

		if (_PROPERTY_EXISTS(pAreaDataset, "Rect"))
			pArea->xRect = _PROPERTY->GetRect();
		else if (_PROPERTY_EXISTS(pAreaDataset, "Size"))
			pArea->xRect = xrect(_PROPERTY->GetPoint(0), _PROPERTY->GetPoint(0) + _PROPERTY->GetPoint(1));
		else if (_PROPERTY_EXISTS(pAreaDataset, "Tile"))
			pArea->xRect = xrect(_PROPERTY->GetInt(0) * _PROPERTY->GetInt(1), 0, (_PROPERTY->GetInt(0) + 1) * _PROPERTY->GetInt(1), _PROPERTY->GetInt(2));
	}

	// Animations.
	_DATASET_FOREACH(pAnimationDataset, pDataset, "Animation", NULL)
	{
		CAnimation* pAnimation = new CAnimation;
		lpAnimations.push_back(pAnimation);

		pAnimation->pName = pAnimationDataset->GetName();
		pAnimation->iAnimationTime = 0;

		xuint iAnimationDelay = 0;

		if (_PROPERTY_EXISTS(pAnimationDataset, "Delay"))
			iAnimationDelay = _PROPERTY->GetInt();

		CFrame* pPrevFrame = NULL;

		// Frames.
		_DATASET_FOREACH(pFrameDataset, pAnimationDataset, "Frame", NULL)
		{
			CFrame* pFrame = new CFrame;
			pAnimation->lpFrames.push_back(pFrame);

			pFrame->pName = NULL;
			pFrame->iDelay = iAnimationDelay;
			pFrame->pEvent = NULL;
			pFrame->pNextFrame = NULL;

			if (pFrameDataset->GetName())
				pFrame->pName = pFrameDataset->GetName();

			pFrame->pArea = FindArea(pFrameDataset->GetProperty("Area")->GetString());

			if (_PROPERTY_EXISTS(pAnimationDataset, "Delay"))
				pFrame->iDelay = (xuint)_PROPERTY->GetInt();

			if (_PROPERTY_EXISTS(pFrameDataset, "Event"))
				pFrame->pEvent = _PROPERTY->GetString();

			if (pPrevFrame && pPrevFrame->pNextFrame == NULL)
				pPrevFrame->pNextFrame = pFrame;

			pFrame->pNextFrame = NULL;
			pPrevFrame = pFrame;

			if (_PROPERTY_EXISTS(pFrameDataset, "Loop"))
				pFrame->pNextFrame = pAnimation->lpFrames.front();
			else if (_PROPERTY_EXISTS(pFrameDataset, "Goto"))
				pFrame->pNextFrame = FindFrame(pAnimation, _PROPERTY->GetString());

			pAnimation->iAnimationTime += pFrame->iDelay;
		}
	}
}

// =============================================================================
// Nat Ryall                                                         23-Apr-2008
// =============================================================================
CSpriteMetadata::~CSpriteMetadata()
{
	ResourceManager::ReleaseResourceFile(pFile);

	XEN_LIST_ERASE_ALL(lpAreas);

	while (lpAnimations.size())
	{
		XEN_LIST_ERASE_ALL(lpAnimations.front()->lpFrames);
		lpAnimations.pop_front();
	}
}

// =============================================================================
// Nat Ryall                                                         23-Apr-2008
// =============================================================================
CSpriteMetadata::CArea* CSpriteMetadata::FindArea(const xchar* pName)
{
	XEN_LIST_FOREACH(t_AreaList, ppArea, lpAreas)
	{
		if (strcmp((*ppArea)->pName, pName) == 0)
			return *ppArea;
	}

	return NULL;
}

// =============================================================================
// Nat Ryall                                                         23-Apr-2008
// =============================================================================
CSpriteMetadata::CFrame* CSpriteMetadata::FindFrame(CAnimation* pAnimation, const xchar* pName)
{
	XEN_LIST_FOREACH(t_FrameList, ppFrame, pAnimation->lpFrames)
	{
		if (strcmp((*ppFrame)->pName, pName) == 0)
			return *ppFrame;
	}

	return NULL;
}

// =============================================================================
// Nat Ryall                                                         23-Apr-2008
// =============================================================================
CSpriteMetadata::CAnimation* CSpriteMetadata::FindAnimation(const xchar* pName)
{
	XEN_LIST_FOREACH(t_AnimationList, ppAnimation, lpAnimations)
	{
		if (strcmp((*ppAnimation)->pName, pName) == 0)
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
	CBasicSprite::Render(m_xPosition, m_xAnchor, m_pArea ? m_pArea->xRect : GetImageRect(), m_fAlpha, m_fAngle);
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
		if (m_pFrame->iDelay)
			m_iFrameTimer += _TIMEDELTA;

		while (m_iFrameTimer >= m_pFrame->iDelay)
		{
			if (m_pFrame->pNextFrame)
			{
				m_iFrameTimer -= m_pFrame->iDelay;
				SetFrame(m_pFrame->pNextFrame);
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
	m_pArea = m_pFrame->pArea;

	if (m_pFrame->pEvent && m_fpEvent)
		m_fpEvent(this, m_pFrame->pEvent);
}

// =============================================================================
// Nat Ryall                                                         16-Apr-2008
// =============================================================================
void CAnimatedSprite::SetAnimation(CSpriteMetadata::CAnimation* pAnimation, xbool bPlay)
{
	m_bPlaying = bPlay;
	m_iFrameTimer = 0;

	m_pAnimation = pAnimation;
	SetFrame(pAnimation->lpFrames.front());
}

//##############################################################################