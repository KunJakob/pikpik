//##############################################################################

// Global.
#include <Global.h>

// Local.
#include <Sprite.h>

//##############################################################################

// =============================================================================
CSpriteFile::CSpriteFile(const xchar* pFile) : CResourceFile(ResourceType_Sprite, pFile)
{
	HTEXTURE hTexture = _HGE->Texture_Load(pFile);
	m_pResource = new hgeSprite(hTexture, 0.0f, 0.0f, (float)_HGE->Texture_GetWidth(hTexture), (float)_HGE->Texture_GetHeight(hTexture));
}

// =============================================================================
CSpriteFile::~CSpriteFile()
{
	_HGE->Texture_Free(GetResource()->GetTexture());
	delete GetResource();
}

//##############################################################################

// =============================================================================
CSpriteMetadata::CSpriteMetadata(CDataset* pDataset) : CResourceMetadata(ResourceType_Sprite, pDataset)
{
	// File.
	m_pFile = (CSpriteFile*)ResourceManager.CreateResourceFile(ResourceType_Sprite, pDataset->GetProperty("File")->GetString());

	// Areas.
	XEN_METADATA_DATASET_FOREACH(pAreaDataset, pDataset, "Area", NULL)
	{
		CArea* pArea = new CArea;
		m_lpAreas.push_back(pArea);

		pArea->m_pName = pAreaDataset->GetName();

		if (XEN_METADATA_PROPERTY_EXISTS(pAreaDataset, "Rect"))
			pArea->m_xRect = XEN_METADATA_PROPERTY->GetRect();
		else if (XEN_METADATA_PROPERTY_EXISTS(pAreaDataset, "Size"))
			pArea->m_xRect = xrect(XEN_METADATA_PROPERTY->GetPoint(0), XEN_METADATA_PROPERTY->GetPoint(0) + XEN_METADATA_PROPERTY->GetPoint(1));
		else if (XEN_METADATA_PROPERTY_EXISTS(pAreaDataset, "Tile"))
			pArea->m_xRect = xrect(XEN_METADATA_PROPERTY->GetInt(0) * XEN_METADATA_PROPERTY->GetInt(1), 0, (XEN_METADATA_PROPERTY->GetInt(0) + 1) * XEN_METADATA_PROPERTY->GetInt(1), XEN_METADATA_PROPERTY->GetInt(2));
	}

	// Animations.
	XEN_METADATA_DATASET_FOREACH(pAnimationDataset, pDataset, "Animation", NULL)
	{
		CAnimation* pAnimation = new CAnimation;
		m_lpAnimations.push_back(pAnimation);

		pAnimation->m_pName = pAnimationDataset->GetName();
		pAnimation->m_iAnimationTime = 0;

		xuint iAnimationDelay = 0;

		if (XEN_METADATA_PROPERTY_EXISTS(pAnimationDataset, "Delay"))
			iAnimationDelay = XEN_METADATA_PROPERTY->GetInt();

		CFrame* pPrevFrame = NULL;

		// Frames.
		XEN_METADATA_DATASET_FOREACH(pFrameDataset, pAnimationDataset, "Frame", NULL)
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

			if (XEN_METADATA_PROPERTY_EXISTS(pFrameDataset, "Delay"))
				pFrame->m_iDelay = (xuint)XEN_METADATA_PROPERTY->GetInt();

			if (XEN_METADATA_PROPERTY_EXISTS(pFrameDataset, "Event"))
				pFrame->m_pEvent = XEN_METADATA_PROPERTY->GetString();

			if (pPrevFrame && pPrevFrame->m_pNextFrame == NULL)
				pPrevFrame->m_pNextFrame = pFrame;

			pFrame->m_pNextFrame = NULL;
			pPrevFrame = pFrame;

			if (XEN_METADATA_PROPERTY_EXISTS(pFrameDataset, "Loop"))
				pFrame->m_pNextFrame = pAnimation->m_lpFrames.front();
			else if (XEN_METADATA_PROPERTY_EXISTS(pFrameDataset, "Goto"))
				pFrame->m_pNextFrame = FindFrame(pAnimation, XEN_METADATA_PROPERTY->GetString());

			pAnimation->m_iAnimationTime += pFrame->m_iDelay;
		}
	}
}

// =============================================================================
CSpriteMetadata::~CSpriteMetadata()
{
	ResourceManager.ReleaseResourceFile(m_pFile);

	XEN_LIST_ERASE_ALL(m_lpAreas);

	while (m_lpAnimations.size())
	{
		XEN_LIST_ERASE_ALL(m_lpAnimations.front()->m_lpFrames);

		delete m_lpAnimations.front();
		m_lpAnimations.pop_front();
	}
}

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
CSpriteMetadata::CFrame* CSpriteMetadata::FindFrame(CAnimation* pAnimation, const xchar* pName)
{
	XEN_LIST_FOREACH(t_FrameList, ppFrame, pAnimation->m_lpFrames)
	{
		if ((*ppFrame)->m_pName && String::IsMatch((*ppFrame)->m_pName, pName))
			return *ppFrame;
	}

	return NULL;
}

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

// =============================================================================
void CBasicSprite::Render(xpoint xPosition, xpoint xAnchor, xrect xArea, xfloat fAlpha, xfloat fAngle)
{
	hgeSprite* pSprite = m_pMetadata->GetSprite();

	pSprite->SetHotSpot((float)xAnchor.m_tX, (float)xAnchor.m_tY);
	pSprite->SetColor(SETA(pSprite->GetColor(), (xchar)(fAlpha * 255.f)));
	pSprite->SetTextureRect((float)xArea.m_tLeft, (float)xArea.m_tTop, (float)xArea.GetWidth(), (float)xArea.GetHeight());

	if (fAngle)
		pSprite->RenderEx((float)xPosition.m_tX, (float)xPosition.m_tY, fAngle);
	else
		pSprite->Render((float)xPosition.m_tX, (float)xPosition.m_tY);
};

// =============================================================================
void CBasicSprite::RenderTiled(xpoint xPosition, xpoint xSize, xpoint xAnchor, xrect xArea, xfloat fAlpha)
{
	xpoint xOffset;
	xpoint xTile;

	if (xSize.m_tY == 0)
		xSize.m_tY = xArea.GetHeight();

	if (xSize.m_tX == 0)
		xSize.m_tX = xArea.GetWidth();

	while (xOffset.m_tY < xSize.m_tY)
	{
		xTile.m_tY = Math::Clamp<xint>(xSize.m_tY - xOffset.m_tY, 0, xArea.GetHeight());

		while (xOffset.m_tX < xSize.m_tX)
		{
			xTile.m_tX = Math::Clamp<xint>(xSize.m_tX - xOffset.m_tX, 0, xArea.GetWidth());

			xrect xTileArea = xArea + xrect(0, 0, xTile.m_tX - xArea.GetWidth(), xTile.m_tY - xArea.GetHeight());
			Render(xPosition + xOffset, xAnchor, xTileArea, fAlpha, 0.f);

			xOffset.m_tX += xTile.m_tX;
		}

		xOffset.m_tX = 0;
		xOffset.m_tY += xTile.m_tY;
	}
}

//##############################################################################

// =============================================================================
CSprite::CSprite(CSpriteMetadata* pMetadata) : CBasicSprite(pMetadata), CRenderable(RenderableType_Sprite),
	m_pArea(NULL),
	m_fAlpha(1.f),
	m_fAngle(0.f)
{
}

// =============================================================================
void CSprite::OnRender()
{
	CBasicSprite::Render(m_xPosition, m_xAnchor, m_pArea ? m_pArea->m_xRect : GetImageRect(), m_fAlpha, m_fAngle);
}

//##############################################################################

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
void CAnimatedSprite::OnRender()
{
	CSprite::OnRender();
}

// =============================================================================
void CAnimatedSprite::SetFrame(CSpriteMetadata::CFrame* pFrame)
{
	m_pFrame = pFrame;
	m_pArea = m_pFrame->m_pArea;

	if (m_pFrame->m_pEvent && m_fpEvent)
		m_fpEvent(this, m_pFrame->m_pEvent);
}

// =============================================================================
void CAnimatedSprite::SetAnimation(CSpriteMetadata::CAnimation* pAnimation, xbool bPlay)
{
	m_bPlaying = bPlay;
	m_iFrameTimer = 0;

	m_pAnimation = pAnimation;
	SetFrame(pAnimation->m_lpFrames.front());
}

//##############################################################################
