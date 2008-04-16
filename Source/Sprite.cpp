#pragma region Include
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
#pragma endregion

#pragma region Sprite
//##############################################################################
//
//                                   SPRITE
//
//##############################################################################

// =============================================================================
// Author: Nat Ryall                                           Date: 19-Feb-2008
// =============================================================================
CSprite::CSprite(CSpriteTemplate* pTemplate) : CRenderable(RenderableType_Sprite), CResource(ResourceType_Sprite),
	m_pSpriteTemplate(pTemplate),
	m_fAlpha(1.0f),
	m_fRotation(0.0f),
	m_pArea(NULL)
{
}

// =============================================================================
// Author: Nat Ryall                                           Date: 12-Feb-2008
// =============================================================================
void CSprite::Render()
{
	hgeSprite* pSurface = GetSurfaceTemplate()->pSurface;

	XRECT xRect = m_pArea ? m_pArea->xRect : XRECT(0, 0, GetWidth(), GetHeight());

	pSurface->SetHotSpot((float)xRect.GetWidth() / 2.f, (float)xRect.GetHeight() / 2.f);
	pSurface->SetColor(SETA(pSurface->GetColor(), (XCHAR)(m_fAlpha * 255.f)));
	pSurface->SetTextureRect((float)xRect.iLeft, (float)xRect.iTop, (float)xRect.GetWidth(), (float)xRect.GetHeight());

	if (m_fRotation)
		pSurface->RenderEx((float)m_xPosition.iX, (float)m_xPosition.iY, m_fRotation);
	else
		pSurface->Render((float)m_xPosition.iX, (float)m_xPosition.iY);
}

// =============================================================================
// Author: Nat Ryall                                           Date: 11-Feb-2008
// =============================================================================
CSurfaceTemplate::CArea* CSprite::FindArea(CSurfaceTemplate* pTemplate, const XCHAR* pName)
{
	XEN_LIST_FOREACH(CSurfaceTemplate::t_AreaList, ppArea, pTemplate->lpAreas)
	{
		if (strcmp((*ppArea)->pName, pName) == 0)
			return *ppArea;
	}

	return NULL;
}

//##############################################################################
#pragma endregion

#pragma region Animated Sprite
//##############################################################################
//
//                               ANIMATED SPRITE
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         16-Apr-2008
// =============================================================================
CAnimatedSprite::CAnimatedSprite(CSpriteTemplate* pTemplate) : CSprite(pTemplate),
	m_iTimer(0),
	m_bPlaying(false),
	m_fpEvent(NULL),
	m_pEventObject(NULL),
	m_pFrame(NULL),
	m_pAnimation(NULL)
{
}

// =============================================================================
// Nat Ryall                                                         16-Apr-2008
// =============================================================================
void CAnimatedSprite::Render()
{
	if (m_bPlaying)
	{
		if (m_pFrame->iDelay)
			m_iTimer += _TIMEDELTA;

		if (m_iTimer >= m_pFrame->iDelay)
		{
			if (m_pFrame->pNextFrame)
			{
				m_iTimer %= m_pFrame->iDelay;
				SetFrame(m_pFrame->pNextFrame);
			}
			else
				m_bPlaying = false;
		}
	}

	CSprite::Render();
}

// =============================================================================
// Author: Nat Ryall                                           Date: 12-Feb-2008
// =============================================================================
CSpriteTemplate::CAnimation* CAnimatedSprite::FindAnimation(CSpriteTemplate* pTemplate, const XCHAR* pName)
{
	XEN_LIST_FOREACH(CSpriteTemplate::t_AnimationList, ppAnimation, pTemplate->lpAnimations)
	{
		if (strcmp((*ppAnimation)->pName, pName) == 0)
			return *ppAnimation;
	}

	return NULL;
}

// =============================================================================
// Nat Ryall                                                         16-Apr-2008
// =============================================================================
void CAnimatedSprite::SetAnimation(CSpriteTemplate::CAnimation* pAnimation)
{
	m_pAnimation = pAnimation;
	SetFrame(pAnimation->lpFrames.front());
}

// =============================================================================
// Author: Nat Ryall                                           Date: 19-Feb-2008
// =============================================================================
void CAnimatedSprite::Play(CSpriteTemplate::CAnimation* pAnimation)
{
	SetAnimation(pAnimation);
	Play();
}

// =============================================================================
// Nat Ryall                                                         16-Apr-2008
// =============================================================================
void CAnimatedSprite::Play()
{
	m_iTimer = 0;
	m_bPlaying = true;
}

// =============================================================================
// Author: Nat Ryall                                           Date: 19-Feb-2008
// =============================================================================
void CAnimatedSprite::Stop()
{
	SetFrame(m_pAnimation->lpFrames.front());

	m_iTimer = 0;
	m_bPlaying = false;
}

// =============================================================================
// Author: Nat Ryall                                           Date: 12-Feb-2008
// =============================================================================
CSpriteTemplate::CFrame* CAnimatedSprite::FindFrame(CSpriteTemplate::CAnimation* pAnimation, const XCHAR* pName)
{
	XEN_LIST_FOREACH(CSpriteTemplate::t_FrameList, ppFrame, pAnimation->lpFrames)
	{
		if (strcmp((*ppFrame)->pName, pName) == 0)
			return *ppFrame;
	}

	return NULL;
}

// =============================================================================
// Nat Ryall                                                         16-Apr-2008
// =============================================================================
void CAnimatedSprite::SetFrame(CSpriteTemplate::CFrame* pFrame)
{
	m_pFrame = pFrame;
	m_pArea = m_pFrame->pArea;

	if (m_pFrame->pEvent && m_fpEvent)
		m_fpEvent(m_pFrame->pEvent, m_pEventObject);
}

//##############################################################################
#pragma endregion