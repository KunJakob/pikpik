#pragma region Include
//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Global.
#include <Global.h>

// Local.
#include <Resource.h>

// Other.
#include <Dataset.h>
#include <Sprite.h>

//##############################################################################
#pragma endregion

#pragma region Types
//##############################################################################
//
//                                   TYPES
//
//##############################################################################

// Lists.
typedef XLIST<CMetadata*> t_MetadataList;
typedef XLIST<CResourceTemplate*> t_ResourceTemplateList;
typedef XLIST<CResource*> t_ResourceList;

//##############################################################################
#pragma endregion

#pragma region Static
//##############################################################################
//
//                                   STATIC
//
//##############################################################################

// Metadata Lists.
static t_MetadataList s_lpMetadata;
static t_ResourceTemplateList s_lpTemplates;
static t_ResourceList s_lpResources;

//##############################################################################
#pragma endregion

#pragma region Local
//##############################################################################
//
//                                   LOCAL
//
//##############################################################################

// =============================================================================
// Author: Nat Ryall                                           Date: 18-Feb-2008
// =============================================================================
CResourceTemplate* FindTemplate(t_ResourceType iType, const XCHAR* pName)
{
	XEN_LIST_FOREACH(t_ResourceTemplateList, ppTemplate, s_lpTemplates)
	{
		if ((*ppTemplate)->iResourceType == iType && strcmp((*ppTemplate)->pResourceName, pName) == 0)
			return *ppTemplate;
	}

	return NULL;
}

// =============================================================================
// Author: Nat Ryall                                           Date: 18-Feb-2008
// =============================================================================
void FreeTemplate(CResourceTemplate* pResourceTemplate)
{
	s_lpTemplates.remove(pResourceTemplate);

	switch (pResourceTemplate->iResourceType)
	{
	case ResourceType_Surface:
		{
			CSurfaceTemplate* pTemplate = (CSurfaceTemplate*)pResourceTemplate;

			_HGE->Texture_Free(pTemplate->pSurface->GetTexture());
			delete pTemplate->pSurface;

			XEN_LIST_ERASEMEM(pTemplate->lpAreas);
			delete pTemplate;
		}
		break;

	case ResourceType_Sprite:
		{
			CSpriteTemplate* pTemplate = (CSpriteTemplate*)pResourceTemplate;

			XEN_LIST_FOREACH(CSpriteTemplate::t_AnimationList, ppAnimation, pTemplate->lpAnimations)
			{
				XEN_LIST_ERASEMEM((*ppAnimation)->lpFrames);
				delete *ppAnimation;
			}

			delete pTemplate;
		}
		break;
	}
}

// =============================================================================
// Author: Nat Ryall                                           Date: 19-Feb-2008
// =============================================================================
void FreeTemplate(t_ResourceType iType, const XCHAR* pName)
{
	XEN_LIST_FOREACH(t_ResourceTemplateList, ppTemplate, s_lpTemplates)
	{
		if ((*ppTemplate)->iResourceType == iType && strcmp((*ppTemplate)->pResourceName, pName) == 0)
		{
			FreeTemplate(*ppTemplate);
			return;
		}
	}
}

// =============================================================================
// Author: Nat Ryall                                           Date: 11-Feb-2008
// =============================================================================
CSurfaceTemplate::CArea* GetArea(CSurfaceTemplate* pTemplate, const XCHAR* pName)
{
	XEN_LIST_FOREACH(CSurfaceTemplate::t_AreaList, ppArea, pTemplate->lpAreas)
	{
		if (strcmp((*ppArea)->pName, pName) == 0)
			return *ppArea;
	}

	return NULL;
}

// =============================================================================
// Author: Nat Ryall                                           Date: 18-Feb-2008
// =============================================================================
CSurfaceTemplate* CreateSurfaceTemplate(CDataset* pDataset)
{
	CSurfaceTemplate* pTemplate = new CSurfaceTemplate;
	s_lpTemplates.push_back(pTemplate);

	// Resource.
	pTemplate->iResourceType = ResourceType_Surface;
	pTemplate->pResourceName = pDataset->GetName();

	// Flags.
	pTemplate->iFlags = 0;

	if (CProperty* pProperty = pDataset->GetProperty("Flags"))
	{
		for (XUINT iA = 0; iA < pProperty->GetValueCount(); ++iA)
		{
			if (strcmp(pProperty->GetString(iA), "None") == 0)
				XFLAGSET(pTemplate->iFlags, CSurfaceTemplate::STF_None);
		}
	}

	// Surface.
	HTEXTURE hTexture = _HGE->Texture_Load(pDataset->GetProperty("File")->GetString());
	pTemplate->pSurface = new hgeSprite(hTexture, 0.0f, 0.0f, (float)_HGE->Texture_GetWidth(hTexture), (float)_HGE->Texture_GetHeight(hTexture));

	// Areas.
	_SUBSET_FOREACH(pAreaDataset, pDataset, "Area", NULL)
	{
		CSurfaceTemplate::CArea* pArea = new CSurfaceTemplate::CArea;

		pArea->pName = pAreaDataset->GetName();
		pArea->xRect = pAreaDataset->GetProperty("Rect")->GetRect();

		pTemplate->lpAreas.push_back(pArea);
	}

	return pTemplate;
}

// =============================================================================
// Author: Nat Ryall                                           Date: 18-Feb-2008
// =============================================================================
CSpriteTemplate* CreateSpriteTemplate(CDataset* pDataset)
{
	CSpriteTemplate* pTemplate = new CSpriteTemplate;
	s_lpTemplates.push_back(pTemplate);

	// Resource.
	pTemplate->iResourceType = ResourceType_Sprite;
	pTemplate->pResourceName = pDataset->GetName();

	// Animations.
	if (CProperty* pProperty = pDataset->GetProperty("Surface"))
		pTemplate->pSurfaceTemplate = (CSurfaceTemplate*)FindTemplate(ResourceType_Surface, pProperty->GetString());

	_SUBSET_FOREACH(pAnimDataset, pDataset, "Animation", NULL)
	{
		CSpriteTemplate::CAnimation* pAnimation = new CSpriteTemplate::CAnimation;

		pAnimation->pName = pAnimDataset->GetName();

		if (CProperty* pProperty = pAnimDataset->GetProperty("Surface"))
			pAnimation->pSurfaceTemplate = (CSurfaceTemplate*)FindTemplate(ResourceType_Surface, pProperty->GetString());
		else
			pAnimation->pSurfaceTemplate = pTemplate->pSurfaceTemplate;

		pAnimation->iTime = 0;

		pTemplate->lpAnimations.push_back(pAnimation);

		// Frames.
		CSpriteTemplate::CFrame* pPrevFrame = NULL;

		XUINT iDefaultDelay = 0;

		if (CProperty* pProperty = pAnimDataset->GetProperty("Delay"))
			iDefaultDelay = (XUINT)pProperty->GetInt();

		_SUBSET_FOREACH(pFrameDataset, pAnimDataset, "Frame", NULL)
		{
			CSpriteTemplate::CFrame* pFrame = new CSpriteTemplate::CFrame;

			if (pFrameDataset->GetName())
				pFrame->pName = pFrameDataset->GetName();

			pFrame->pArea = CSprite::FindArea(pAnimation->pSurfaceTemplate, pFrameDataset->GetProperty("Area")->GetString());

			if (CProperty* pProperty = pFrameDataset->GetProperty("Delay"))
				pFrame->iDelay = (XUINT)pProperty->GetInt();
			else
				pFrame->iDelay = iDefaultDelay;

			pAnimation->iTime += pFrame->iDelay;

			if (CProperty* pProperty = pFrameDataset->GetProperty("Event"))
				pFrame->pEvent = pProperty->GetString();
			else
				pFrame->pEvent = NULL;

			pFrame->pNextFrame = NULL;

			if (pPrevFrame && pPrevFrame->pNextFrame == NULL)
				pPrevFrame->pNextFrame = pFrame;

			pFrame->pNextFrame = NULL;
			pPrevFrame = pFrame;

			if (CProperty* pProperty = pFrameDataset->GetProperty("Loop"))
				pFrame->pNextFrame = pAnimation->lpFrames.front();
			else if (CProperty* pProperty = pFrameDataset->GetProperty("Goto"))
				pFrame->pNextFrame = CAnimatedSprite::FindFrame(pAnimation, pProperty->GetString());

			pAnimation->lpFrames.push_back(pFrame);
		}
	}

	// Defaults.
	if (CProperty* pProperty = pDataset->GetProperty("Position"))
		pTemplate->xInitialPosition = pProperty->GetPoint();

	if (CProperty* pProperty = pDataset->GetProperty("Animation"))
		pTemplate->pInitialAnimation = CAnimatedSprite::FindAnimation(pTemplate, pProperty->GetString());
	else if (pTemplate->lpAnimations.size())
		pTemplate->pInitialAnimation = pTemplate->lpAnimations.front();
	else
		pTemplate->pInitialAnimation = NULL;

	return pTemplate;
}

//##############################################################################
#pragma endregion

#pragma region ModuleManager
//##############################################################################
//
//                                   MODULE
//
//##############################################################################
static class CResourceModule : public Xen::CModule
{
public:
	// Constructor.
	CResourceModule()
	{
		XMODULE(this);
	}

	// Deinitialise.
	virtual void Deinitialise()
	{
		ResourceManager::Reset();
	}
} 
s_ResourceModule;

//##############################################################################
#pragma endregion

#pragma region Definition
//##############################################################################
//
//                                 DEFINITION
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                          8-Apr-2008
// =============================================================================
void ResourceManager::Reset()
{
	s_lpMetadata.clear();

	while (s_lpTemplates.size())
		FreeTemplate(s_lpTemplates.front());

	while (s_lpResources.size())
		ResourceManager::FreeResource(s_lpResources.front());
}

// =============================================================================
// Author: Nat Ryall                                           Date: 18-Feb-2008
// =============================================================================
void ResourceManager::LoadMetadata(CMetadata* pMetadata)
{
	_SUBSET_FOREACH(pDataset, pMetadata, "Surface", NULL)
		CreateSurfaceTemplate(pDataset);

	_SUBSET_FOREACH(pDataset, pMetadata, "Sprite", NULL)
		CreateSpriteTemplate(pDataset);

	s_lpMetadata.push_back(pMetadata);
}

// =============================================================================
// Author: Nat Ryall                                           Date: 18-Feb-2008
// =============================================================================
void ResourceManager::UnloadMetadata(CMetadata* pMetadata)
{
	_SUBSET_FOREACH(pDataset, pMetadata, "Surface", NULL)
		FreeTemplate(ResourceType_Surface, pDataset->GetName());

	_SUBSET_FOREACH(pDataset, pMetadata, "Sprite", NULL)
		FreeTemplate(ResourceType_Sprite, pDataset->GetName());

	s_lpMetadata.remove(pMetadata);
}

// =============================================================================
// Author: Nat Ryall                                           Date: 18-Feb-2008
// =============================================================================
CSprite* ResourceManager::CreateSprite(const XCHAR* pName)
{
	if (CSpriteTemplate* pTemplate = (CSpriteTemplate*)FindTemplate(ResourceType_Sprite, pName))
	{
		CSprite* pSprite = new CSprite(pTemplate);
		s_lpResources.push_back(pSprite);

		pSprite->SetPosition(pTemplate->xInitialPosition);

		return pSprite;
	}

	return NULL;
}

// =============================================================================
// Nat Ryall                                                         16-Apr-2008
// =============================================================================
CAnimatedSprite* ResourceManager::CreateAnimatedSprite(const XCHAR* pName)
{
	if (CSpriteTemplate* pTemplate = (CSpriteTemplate*)FindTemplate(ResourceType_Sprite, pName))
	{
		CAnimatedSprite* pSprite = new CAnimatedSprite(pTemplate);
		s_lpResources.push_back(pSprite);

		pSprite->SetPosition(pTemplate->xInitialPosition);

		if (pTemplate->pInitialAnimation)
			pSprite->Play(pTemplate->pInitialAnimation);

		return pSprite;
	}

	return NULL;
}

// =============================================================================
// Author: Nat Ryall                                           Date: 18-Feb-2008
// =============================================================================
void ResourceManager::FreeResource(CResource* pResource)
{
	XEN_LIST_FOREACH(t_ResourceList, ppResource, s_lpResources)
	{
		if (*ppResource == pResource)
		{
			s_lpResources.erase(ppResource);
			delete pResource;

			return;
		}
	}
}

//##############################################################################
#pragma endregion