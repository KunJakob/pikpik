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
CSpriteFile::CSpriteFile(const XCHAR* pFile) : CResourceFile(ResourceType_Sprite, pFile)
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
		pArea->xRect = pAreaDataset->GetProperty("Rect")->GetRect();
	}

	// Animations.
	_DATASET_FOREACH(pAnimationDataset, pDataset, "Animation", NULL)
	{
		CAnimation* pAnimation = new CAnimation;
		lpAnimations.push_back(pAnimation);

		pAnimation->pName = pAnimationDataset->GetName();
		pAnimation->iAnimationTime = 0;

		XUINT iAnimationDelay = 0;

		if (CProperty* pProperty = pAnimationDataset->GetProperty("Delay"))
			iAnimationDelay = pProperty->GetInt();

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

			if (CProperty* pProperty = pAnimationDataset->GetProperty("Delay"))
				pFrame->iDelay = (XUINT)pProperty->GetInt();

			if (CProperty* pProperty = pFrameDataset->GetProperty("Event"))
				pFrame->pEvent = pProperty->GetString();
				
			if (pPrevFrame && pPrevFrame->pNextFrame == NULL)
				pPrevFrame->pNextFrame = pFrame;

			pFrame->pNextFrame = NULL;
			pPrevFrame = pFrame;

			if (CProperty* pProperty = pFrameDataset->GetProperty("Loop"))
				pFrame->pNextFrame = pAnimation->lpFrames.front();
			else if (CProperty* pProperty = pFrameDataset->GetProperty("Goto"))
				pFrame->pNextFrame = FindFrame(pAnimation, pProperty->GetString());

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

	XEN_LIST_ERASEMEM(lpAreas);

	while (lpAnimations.size())
	{
		XEN_LIST_ERASEMEM(lpAnimations.front()->lpFrames);
		lpAnimations.pop_front();
	}
}

// =============================================================================
// Nat Ryall                                                         23-Apr-2008
// =============================================================================
CSpriteMetadata::CArea* CSpriteMetadata::FindArea(const XCHAR* pName)
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
CSpriteMetadata::CFrame* CSpriteMetadata::FindFrame(CAnimation* pAnimation, const XCHAR* pName)
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
CSpriteMetadata::CAnimation* CSpriteMetadata::FindAnimation(const XCHAR* pName)
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
void CBasicSprite::Render(XRECT xArea, XPOINT xAnchor, XPOINT xPosition, XFLOAT fAlpha, XFLOAT fAngle)
{
	hgeSprite* pSprite = m_pMetadata->GetSprite();

	pSprite->SetHotSpot((float)xAnchor.iX, (float)xAnchor.iY);
	pSprite->SetColor(SETA(pSprite->GetColor(), (XCHAR)(fAlpha * 255.f)));
	pSprite->SetTextureRect((float)xArea.iLeft, (float)xArea.iTop, (float)xArea.GetWidth(), (float)xArea.GetHeight());

	if (fAngle)
		pSprite->RenderEx((float)xPosition.iX, (float)xPosition.iY, fAngle);
	else
		pSprite->Render((float)xPosition.iX, (float)xPosition.iY);
};

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
	XRECT xArea = m_pArea ? m_pArea->xRect : XRECT(0, 0, (XUINT)GetImageWidth(), (XUINT)GetImageHeight());
	CBasicSprite::Render(xArea, m_xAnchor, m_xPosition, m_fAlpha, m_fAngle);
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
	m_fpEvent(NULL),
	m_pEventData(NULL)
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

		if (m_iFrameTimer >= m_pFrame->iDelay)
		{
			if (m_pFrame->pNextFrame)
			{
				m_iFrameTimer %= m_pFrame->iDelay;
				SetFrame(m_pFrame->pNextFrame);
			}
			else
				m_bPlaying = false;
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
		m_fpEvent(this, m_pFrame->pEvent, m_pEventData);
}

// =============================================================================
// Nat Ryall                                                         16-Apr-2008
// =============================================================================
void CAnimatedSprite::SetAnimation(CSpriteMetadata::CAnimation* pAnimation, XBOOL bPlay)
{
	m_bPlaying = bPlay;
	m_iFrameTimer = 0;

	m_pAnimation = pAnimation;
	SetFrame(pAnimation->lpFrames.front());
}

//##############################################################################