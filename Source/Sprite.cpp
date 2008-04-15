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

#pragma region Definition
//##############################################################################
//
//                                 DEFINITION
//
//##############################################################################

// =============================================================================
// Author: Nat Ryall                                           Date: 19-Feb-2008
// =============================================================================
CSprite::CSprite(CSpriteTemplate* pTemplate) :
	CRenderable(RenderableType_Sprite),
  CResource(ResourceType_Sprite),
  m_pSpriteTemplate(pTemplate),
	m_fAlpha(1.0f),
	m_fRotation(0.0f),
  m_iTimer(0),
  m_bPlaying(false),
	m_pArea(NULL),
	m_pFrame(NULL),
  m_pAnimation(NULL)
{
}

// =============================================================================
// Author: Nat Ryall                                           Date: 12-Feb-2008
// =============================================================================
void CSprite::Render()
{
	// Process the current frame.
  if (m_bPlaying)
  {
    if (m_pFrame->iDelay)
      m_iTimer += _TIMEDELTA;

    if (m_iTimer >= m_pFrame->iDelay)
    {
      if (m_pFrame->pNextFrame)
      {
        m_iTimer %= m_pFrame->iDelay;
        m_pFrame = m_pFrame->pNextFrame;

				//if (m_pFrame->pEvent)
				//	ExecuteEvent(m_pFrame->pEvent);
      }
      else
        m_bPlaying = false;
    }
  }
  
	// Determine the surface area to render.
	XRECT xRect(0, 0, GetWidth(), GetHeight());
		
	if (m_pAnimation && m_pFrame->pArea)
		xRect = m_pFrame->pArea->xRect;
	else if (m_pArea)
		xRect = m_pArea->xRect;

	// Render the sprite.
	hgeSprite* pSurface = GetSurfaceTemplate()->pSurface;
	pSurface->SetColor(SETA(pSurface->GetColor(), (XCHAR)(m_fAlpha * 255.f)));

	pSurface->SetHotSpot((float)xRect.GetWidth() / 2.f, (float)xRect.GetHeight() / 2.f);
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

// =============================================================================
// Author: Nat Ryall                                           Date: 12-Feb-2008
// =============================================================================
CSpriteTemplate::CAnimation* CSprite::FindAnimation(const XCHAR* pName)
{
  XEN_LIST_FOREACH(CSpriteTemplate::t_AnimationList, ppAnimation, m_pSpriteTemplate->lpAnimations)
  {
    if (strcmp((*ppAnimation)->pName, pName) == 0)
      return *ppAnimation;
  }

  return NULL;
}

// =============================================================================
// Author: Nat Ryall                                           Date: 12-Feb-2008
// =============================================================================
CSpriteTemplate::CFrame* CSprite::FindFrame(CSpriteTemplate::CAnimation* pAnimation, const XCHAR* pName)
{
  XEN_LIST_FOREACH(CSpriteTemplate::t_FrameList, ppFrame, pAnimation->lpFrames)
  {
    if (strcmp((*ppFrame)->pName, pName) == 0)
      return *ppFrame;
  }

  return NULL;
}

// =============================================================================
// Author: Nat Ryall                                           Date: 19-Feb-2008
// =============================================================================
void CSprite::Play(CSpriteTemplate::CAnimation* pAnimation)
{
	m_pArea = NULL;
  m_pAnimation = pAnimation;
  m_pFrame = pAnimation->lpFrames.front();

  m_iTimer = 0;
  m_bPlaying = true;
}

// =============================================================================
// Author: Nat Ryall                                           Date: 19-Feb-2008
// =============================================================================
void CSprite::Stop()
{
  m_pAnimation = NULL;
	m_pFrame = NULL;

  m_iTimer = 0;
  m_bPlaying = false;
}

//##############################################################################
#pragma endregion