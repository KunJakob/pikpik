#pragma once

/**
* @file Sprite.h
* @author Nat Ryall
* @date 10/02/2008
*
*	Sprites are management objects for specific 2D images that can be rendered
* to the screen with various parameters. Sprites are based off of sprite
* metadata that is loaded and parsed in the ResourceManager.
*
* Copyright © SAPIAN
*/

//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Global.
#include <Global.h>

// Other.
#include <Metadata.h>
#include <Resource.h>
#include <Renderer.h>

//##############################################################################

//##############################################################################
//
//                                   MACROS
//
//##############################################################################

// Find a specific sprite metadata using the resource manager.
#define _SPRITE(NAME) ((CSpriteMetadata*)ResourceManager::FindResource(ResourceType_Sprite, NAME))

//##############################################################################

//##############################################################################
//
//                                 SPRITE FILE
//
//##############################################################################

/**
* The sprite resource file.
*/
class CSpriteFile : public CResourceFile
{
public:
	/**
	* Constructor: Initialise the file.
	*/
	CSpriteFile(const XCHAR* pFile);

	/**
	* Destructor: Clean up the file memory.
	*/
	virtual ~CSpriteFile();

	/**
	* Get the sprite object.
	*/
	hgeSprite* GetResource()
	{
		return (hgeSprite*)pResource;
	}
};

//##############################################################################

//##############################################################################
//
//                               SPRITE METADATA
//
//##############################################################################

/**
* The sprite metadata.
*/
class CSpriteMetadata : public CResourceMetadata
{
public:
	// Predeclare.
	class CArea;
	class CFrame;
	class CAnimation;

	// Types.
	typedef XLIST<CArea*> t_AreaList;
	typedef XLIST<CFrame*> t_FrameList;
	typedef XLIST<CAnimation*> t_AnimationList;

	/**
	* Constructor: Initialise the metadata.
	*/
	CSpriteMetadata(CDataset* pDataset);

	/**
	* Destructor: Deinitialise the metadata.
	*/
	virtual ~CSpriteMetadata();

	/**
	* Get the sprite object.
	*/
	hgeSprite* GetSprite()
	{
		return pFile->GetResource();
	}

	/**
	* Find an area by name.
	*/
	CArea* FindArea(const XCHAR* pName);

	/**
	* Find a frame in the specified animation.
	*/
	CFrame* FindFrame(CAnimation* pAnimation, const XCHAR* pName);

	/**
	* Find an animation by name.
	*/
	CAnimation* FindAnimation(const XCHAR* pName);

public:
	CSpriteFile* pFile;								// The sprite file used for rendering.

	/**
	* Specifies an area within a surface.
	*/
	class CArea
	{
	public:
		const XCHAR* pName;							// The name of the area to reference with.
		XRECT xRect;										// Rect defining the pixels on the image to render.
	};

	t_AreaList lpAreas;								// The list of areas specified for the surface.

	/**
	* Describes a single frame of an animation.
	*/
	class CFrame
	{
	public:
		const XCHAR* pName;							// The named identifier for the frame.
		CArea* pArea;										// The area to render for the animation frame.
		XUINT iDelay;										// The amount of time to hold the animation frame.
		const XCHAR* pEvent;						// The event string for this frame. This is NULL unless an event is set.
		CFrame* pNextFrame;							// The next frame in the animation sequence. If this is NULL, the animation stops here.
	};

	/**
	* Describes an animation of frames using the specified surface.
	*/
	class CAnimation
	{
	public:
		const XCHAR* pName;							// The named identifier of the animation.
		XUINT iAnimationTime;						// The total animation time for all frames.
		t_FrameList lpFrames;						// A list of frames used in the animation.
	};

	t_AnimationList lpAnimations;			// The animations available in the sprite.
};

//##############################################################################

//##############################################################################
//
//                                BASIC SPRITE
//
//##############################################################################
class CBasicSprite
{
public:
	/**
	* Constructor.
	*/
	CBasicSprite(CSpriteMetadata* pMetadata) : m_pMetadata(pMetadata) {}

	/**
	* Destructor.
	*/
	virtual ~CBasicSprite() {}

	/**
	* Render the sprite with the specified attributes.
	* @param xPosition The screen position to render at.
	* @param xAnchor The area relative anchor point to use when positioning and rotating the sprite.
	* @param xArea The coordinates within the image to render.
	* @param fAlpha The alpha level between 0.0 and 1.0 to render the sprite at.
	* @param fAngle The angle in radians to render at using the centre of the image as an anchor.
	*/
	void Render(XPOINT xPosition, XPOINT xAnchor, XRECT xArea, XFLOAT fAlpha, XFLOAT fAngle);

  /**
  * Render the sprite with the specified attributes.
  * @param xAnchor The area relative anchor point to use when positioning and rotating the sprite.
  * @param xPosition The screen position to render at.
	* @param xArea The coordinates within the image to render.
  */
  inline void Render(XPOINT xPosition, XPOINT xAnchor, XRECT xArea)
  {
    Render(xPosition, xAnchor, xArea, 1.f, 0.f);
  }

  /**
  * Render the sprite with the specified attributes.
  * @param xPosition The screen position to render at.
	* @param xArea The coordinates within the image to render.
  */
  inline void Render(XPOINT xPosition, XRECT xArea)
  {
    Render(xPosition, XPOINT(), xArea, 1.f, 0.f);
  }

  /**
  * Render the sprite with the specified attributes.
  * @param xPosition The screen position to render at.
  */
  inline void Render(XPOINT xPosition)
  {
    Render(xPosition, XPOINT(), GetImageRect(), 1.f, 0.f);
  }

	/**
	* Render the sprite, tiling the image to the rect.
	* @param xPosition The screen position to render at.
	* @param xSize The size of the area from the position to tile over.
	* @param xAnchor The area relative anchor point to use when positioning and rotating the sprite.
	* @param xArea The coordinates within the image to render.
	* @param fAlpha The alpha level between 0.0 and 1.0 to render the sprite at.
	*/
	void RenderTiled(XPOINT xPosition, XPOINT xSize, XPOINT xAnchor, XRECT xArea, XFLOAT fAlpha);

	/**
	* Render the sprite, tiling the image to the rect.
	* @param xPosition The screen position to render at.
	* @param xSize The size of the area from the position to tile over.
	* @param xAnchor The area relative anchor point to use when positioning and rotating the sprite.
	* @param xArea The coordinates within the image to render.
	*/
	inline void RenderTiled(XPOINT xPosition, XPOINT xSize, XPOINT xAnchor, XRECT xArea)
	{
		RenderTiled(xPosition, xSize, xAnchor, xArea, 1.f);
	}

	/**
	* Render the sprite, tiling the image to the rect.
	* @param xPosition The screen position to render at.
	* @param xSize The size of the area from the position to tile over.
	* @param xArea The coordinates within the image to render.
	*/
	inline void RenderTiled(XPOINT xPosition, XPOINT xSize, XRECT xArea)
	{
		RenderTiled(xPosition, xSize, XPOINT(), xArea, 1.f);
	}

	/**
	* Render the sprite, tiling the image to the rect.
	* @param xPosition The screen position to render at.
	* @param xSize The size of the area from the position to tile over.
	*/
	inline void RenderTiled(XPOINT xPosition, XPOINT xSize)
	{
		RenderTiled(xPosition, xSize, XPOINT(), GetImageRect(), 1.f);
	}

	/**
	* Get the width of the entire sprite image.
	*/
	XUINT GetImageWidth()
	{
		return (XUINT)_HGE->Texture_GetWidth(m_pMetadata->GetSprite()->GetTexture(), true);
	}

	/**
	* Get the height of the entire sprite image.
	*/
	XUINT GetImageHeight()
	{
		return (XUINT)_HGE->Texture_GetHeight(m_pMetadata->GetSprite()->GetTexture(), true);
	}

  /**
  * Get the rect of the entire sprite image.
  */
  XRECT GetImageRect()
  {
    return XRECT(0, 0, (XUINT)GetImageWidth(), (XUINT)GetImageHeight());
  }

	/**
	* Get the sprite metadata structure. This is for advanced use only.
	*/
	CSpriteMetadata* GetMetadata()
	{
		return m_pMetadata;
	}

protected:
	// The sprite metadata.
	CSpriteMetadata* m_pMetadata;
};

//##############################################################################

//##############################################################################
//
//                                   SPRITE
//
//##############################################################################
class CSprite : public CBasicSprite, public CRenderable
{
public:
	/**
	* Constructor.
	*/
	CSprite(CSpriteMetadata* pMetadata);

	/**
	* Render the sprite with all current attributes.
	*/
	virtual void Render();

	/**
	* Set the current area to render.
	*/
	void SetArea(CSpriteMetadata::CArea* pArea)
	{
		m_pArea = pArea;
	}

	/**
	* Set the current area to render.
	*/
	void SetArea(const XCHAR* pName)
	{
		SetArea(m_pMetadata->FindArea(pName));
	}

	/**
	* Get the current area set for the sprite.
	*/
	CSpriteMetadata::CArea* GetArea()
	{
		return m_pArea;
	}

	/**
	* Set the area-relative anchor point to use when positioning and rotating the sprite.
	*/
	void SetAnchor(XPOINT xAnchor)
	{
		m_xAnchor = xAnchor;
	}

	/**
	* Get the area-relative anchor point to use when positioning and rotating the sprite.
	*/
	XPOINT GetAnchor()
	{
		return m_xAnchor;
	}

	/**
	* Set the sprite position. 
	* @note The sprite is rendered from the anchor point at the specified screen position.
	*/
	void SetPosition(XPOINT xPosition)
	{
		m_xPosition = xPosition;
	}

	/**
	* Add an offset to the current position.
	* @note The sprite is rendered from the anchor point at the specified screen position.
	*/
	void OffsetPosition(XPOINT xOffset)
	{
		SetPosition(GetPosition() + xOffset);
	}

	/**
	* Get the current sprite position in screen coordinates.
	* @note The sprite is rendered from the anchor point at the specified screen position.
	*/
	XPOINT GetPosition()
	{
		return m_xPosition;
	}

	/**
	* Set the alpha-blending level of the sprite.
	* @param iAlpha Value between 0.0f (transparent) and 1.0f (opaque).
	*/
	void SetAlpha(XFLOAT iAlpha)
	{
		m_fAlpha = Math::Clamp(iAlpha, 0.f, 1.f);
	}

	/**
	* Get the current alpha level of the sprite.
	* @return Value between 0.0f (transparent) and 1.0f (opaque).
	*/
	XFLOAT GetAlpha()
	{
		return m_fAlpha;
	}

	/**
	* Set the rotation angle of the sprite.
	* @param bDegrees Specify true if the angle is in degrees, otherwise use radians.
	*/
	void SetAngle(XFLOAT fAngle, XBOOL bDegrees = false)
	{
		m_fAngle = !bDegrees ? fAngle : (fAngle / 180.0f) * M_PI;
	}

	/**
	* Get the rotation angle of the sprite.
	* @param bDegrees Specify true to get the angle in degrees, otherwise returns radians.
	*/
	XFLOAT GetAngle(XBOOL bDegrees = false)
	{
		return !bDegrees ? m_fAngle : m_fAngle * (180.0f / M_PI);
	}

	/**
	* Get the width of the current area or the entire image if there is no active area.
	*/
	XUINT GetAreaWidth()
	{
		return m_pArea ? m_pArea->xRect.Width() : GetImageWidth();
	}

	/**
	* Get the height of the current area or the entire image if there is no active area.
	*/
	XUINT GetAreaHeight()
	{
		return m_pArea ? m_pArea->xRect.Height() : GetImageHeight();
	}

	/**
	* Get the centre point of the current area of the sprite.
	*/
	XPOINT GetAreaCentre()
	{
		return XPOINT(GetAreaWidth() / 2, GetAreaHeight() / 2);
	}

	/**
	* Get the screen area for the current area of the sprite.
	*/
	XRECT GetScreenRect()
	{
		XPOINT xTopLeft = m_xPosition - m_xAnchor;
		return XRECT(xTopLeft.iX, xTopLeft.iY, xTopLeft.iX + GetAreaWidth(), xTopLeft.iY + GetAreaHeight());
	}

protected:
	// The current sprite area to render.
	CSpriteMetadata::CArea* m_pArea;

	// The current anchor point of the sprite.
	XPOINT m_xAnchor;

	// The current position of the sprite.
	XPOINT m_xPosition;

	// The current alpha level of the sprite.
	XFLOAT m_fAlpha;

	// The current rotation angle in radians.
	XFLOAT m_fAngle;
};

//##############################################################################

//##############################################################################
//
//                               ANIMATED SPRITE
//
//##############################################################################
class CAnimatedSprite : public CSprite
{
public:
	// Callbacks.
	typedef xfunction(2)<CAnimatedSprite* /*Sprite*/, const XCHAR* /*Event*/> t_AnimationEventCallback;

	/**
	* Constructor.
	*/
	CAnimatedSprite(CSpriteMetadata* pMetadata);

	/**
	* Update the sprite to the current animation frame.
	*/
	virtual void Update();

	/**
	* Render the sprite with all current attributes using the current animation frame.
	*/
	virtual void Render();

	/**
	* Jump to a specific animation frame in the current animation.
	*/
	void SetFrame(CSpriteMetadata::CFrame* pFrame);

	/**
	* Jump to a specific animation frame in the current animation.
	*/
	void SetFrame(const XCHAR* pName)
	{
		SetFrame(m_pMetadata->FindFrame(m_pAnimation, pName));
	}

	/**
	* Get the currently active animation frame.
	*/
	CSpriteMetadata::CFrame* GetFrame()
	{
		return m_pFrame;
	}

	/**
	* Check if the active animation frame name matches the name specified.
	*/
	XBOOL IsActiveFrame(const XCHAR* pName)
	{
		return m_pFrame && strcmp(m_pFrame->pName, pName) == 0;
	}

	/**
	* Set the active animation and reset states and timings to the first animation frame.
	* @param bPlay Start the animation if true or pause otherwise.
	*/
	void SetAnimation(CSpriteMetadata::CAnimation* pAnimation, XBOOL bPlay = false);

	/**
	* Set the active animation and reset states and timings to the first animation frame.
	* @param bPlay Start the animation if true or pause otherwise.
	*/
	void SetAnimation(const XCHAR* pName, XBOOL bPlay = false)
	{
		SetAnimation(m_pMetadata->FindAnimation(pName), bPlay);
	}

	/**
	* Get the currently active animation.
	*/
	CSpriteMetadata::CAnimation* GetAnimation()
	{
		return m_pAnimation;
	}

	/**
	* Check if the active animation name matches the name specified.
	*/
	XBOOL IsActiveAnimation(const XCHAR* pName)
	{
		return m_pAnimation && strcmp(m_pAnimation->pName, pName) == 0;
	}

	/**
	* Play a specific animation from the first animation frame.
	*/
	void Play(CSpriteMetadata::CAnimation* pAnimation)
	{
		SetAnimation(pAnimation, true);
	}

	/**
	* Play a specific animation from the first animation frame.
	*/
	void Play(const XCHAR* pName)
	{
		SetAnimation(pName, true);
	}

	/**
	* Play the active animation from the first animation frame.
	*/
	void Play()
	{
		SetAnimation(m_pAnimation, true);
	}

	/**
	* Stop the active animation and reset to the first animation frame.
	*/
	void Stop()
	{
		SetAnimation(m_pAnimation, false);
	}

	/**
	* Pause the active animation keeping all current animation frame states and timings.
	*/
	void Pause()
	{
		m_bPlaying = false;
	}

	/**
	* Resume the active animation using the last known animation frame states and timings.
	*/
	void Resume()
	{
		m_bPlaying = true;
	}

	/**
	* Check if any active animation is playing.
	*/
	XBOOL IsPlaying()
	{
		return m_bPlaying;
	}

	/**
	* Set the callback to be executed if any events are specified
	* @param fpCallback Specify NULL to disable this callback.
	*/
	void SetEventCallback(t_AnimationEventCallback fpCallback)
	{
		m_fpEvent = fpCallback;
	}

protected:
	// The current frame of the animation we are playing.
	CSpriteMetadata::CFrame* m_pFrame;

	// The current animation we are playing.
	CSpriteMetadata::CAnimation* m_pAnimation;

	// The playback control flag.
	XBOOL m_bPlaying;

	// The delay timer used for frame timings.
	XUINT m_iFrameTimer;

	// The animation event callback.
	t_AnimationEventCallback m_fpEvent;
};

////##############################################################################