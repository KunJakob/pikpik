#pragma once

/**
* @file Sprite.h
* @author Nat Ryall
* @date 10/02/2008
*
* Sprites are management objects for specific 2D images that can be rendered
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

// Find a sprite resource.
#define _SPRITE(NAME) ((CSpriteMetadata*)ResourceManager.GetResourceMetadata(ResourceType_Sprite, NAME))

//##############################################################################

//##############################################################################
//
//                                 SPRITE FILE
//
//##############################################################################
class CSpriteFile : public CResourceFile
{
public:
	// Constructor: Initialise the file.
	CSpriteFile(const xchar* pFile);

	// Destructor: Clean up the file memory.
	virtual ~CSpriteFile();

	// Get the sprite object.
	hgeSprite* GetResource()
	{
		return (hgeSprite*)m_pResource;
	}
};

//##############################################################################

//##############################################################################
//
//                               SPRITE METADATA
//
//##############################################################################
class CSpriteMetadata : public CResourceMetadata
{
public:
	// Predeclare.
	class CArea;
	class CFrame;
	class CAnimation;

	// Types.
	typedef xlist<CArea*> t_AreaList;
	typedef xlist<CFrame*> t_FrameList;
	typedef xlist<CAnimation*> t_AnimationList;

	// Constructor: Initialise the metadata.
	CSpriteMetadata(CDataset* pDataset);

	// Destructor: Deinitialise the metadata.
	virtual ~CSpriteMetadata();

	// Get the sprite object.
	hgeSprite* GetSprite()
	{
		return m_pFile->GetResource();
	}

	// Find an area by name.
	CArea* FindArea(const xchar* pName);

	// Find a frame in the specified animation.
	CFrame* FindFrame(CAnimation* pAnimation, const xchar* pName);

	// Find an animation by name.
	CAnimation* FindAnimation(const xchar* pName);

	// The sprite file used for rendering.
	CSpriteFile* m_pFile;

	/**
	* Specifies an area within a surface.
	*/
	class CArea
	{
	public:
		// The name of the area to reference with.
		const xchar* m_pName;

		// Rect defining the pixels on the image to render.
		xrect m_xRect;
	};

	// The list of areas specified for the surface.
	t_AreaList m_lpAreas;

	/**
	* Describes a single frame of an animation.
	*/
	class CFrame
	{
	public:
		// The named identifier for the frame.
		const xchar* m_pName;

		// The area to render for the animation frame.
		CArea* m_pArea;			

		// The amount of time to hold the animation frame.
		xuint m_iDelay;

		// The event string for this frame. This is NULL unless an event is set.
		const xchar* m_pEvent;

		// The next frame in the animation sequence. If this is NULL, the animation stops here.
		CFrame* m_pNextFrame;
	};

	/**
	* Describes an animation of frames using the specified surface.
	*/
	class CAnimation
	{
	public:
		// The named identifier of the animation.
		const xchar* m_pName;

		// The total animation time for all frames.
		xuint m_iAnimationTime;

		// A list of frames used in the animation.
		t_FrameList m_lpFrames;
	};

	// The animations available in the sprite.
	t_AnimationList m_lpAnimations;			
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
	// Constructor.
	CBasicSprite(CSpriteMetadata* pMetadata) : m_pMetadata(pMetadata) {}

	// Destructor.
	virtual ~CBasicSprite() {}

	// Render the sprite with the specified attributes.
	// ~xPosition The screen position to render at.
	// ~xAnchor The area relative anchor point to use when positioning and rotating the sprite.
	// ~xArea The coordinates within the image to render.
	// ~fAlpha The alpha level between 0.0 and 1.0 to render the sprite at.
	// ~fAngle The angle in radians to render at using the centre of the image as an anchor.
	void Render(xpoint xPosition, xpoint xAnchor, xrect xArea, xfloat fAlpha, xfloat fAngle);

	// Render the sprite with the specified attributes.
	// ~xAnchor The area relative anchor point to use when positioning and rotating the sprite.
	// ~xPosition The screen position to render at.
	// ~xArea The coordinates within the image to render.
	inline void Render(xpoint xPosition, xpoint xAnchor, xrect xArea)
	{
		Render(xPosition, xAnchor, xArea, 1.f, 0.f);
	}

	// Render the sprite with the specified attributes.
	// ~xPosition The screen position to render at.
	// ~xArea The coordinates within the image to render.
	inline void Render(xpoint xPosition, xrect xArea)
	{
		Render(xPosition, xpoint(), xArea, 1.f, 0.f);
	}

	// Render the sprite with the specified attributes.
	// ~xPosition The screen position to render at.
	inline void Render(xpoint xPosition)
	{
		Render(xPosition, xpoint(), GetImageRect(), 1.f, 0.f);
	}

	// Render the sprite, tiling the image to the rect.
	// ~xPosition The screen position to render at.
	// ~xSize The size of the area from the position to tile over.
	// ~xAnchor The area relative anchor point to use when positioning and rotating the sprite.
	// ~xArea The coordinates within the image to render.
	// ~fAlpha The alpha level between 0.0 and 1.0 to render the sprite at.
	void RenderTiled(xpoint xPosition, xpoint xSize, xpoint xAnchor, xrect xArea, xfloat fAlpha);

	// Render the sprite, tiling the image to the rect.
	// ~xPosition The screen position to render at.
	// ~xSize The size of the area from the position to tile over.
	// ~xAnchor The area relative anchor point to use when positioning and rotating the sprite.
	// ~xArea The coordinates within the image to render.
	inline void RenderTiled(xpoint xPosition, xpoint xSize, xpoint xAnchor, xrect xArea)
	{
		RenderTiled(xPosition, xSize, xAnchor, xArea, 1.f);
	}

	// Render the sprite, tiling the image to the rect.
	// ~xPosition The screen position to render at.
	// ~xSize The size of the area from the position to tile over.
	// ~xArea The coordinates within the image to render.
	inline void RenderTiled(xpoint xPosition, xpoint xSize, xrect xArea)
	{
		RenderTiled(xPosition, xSize, xpoint(), xArea, 1.f);
	}

	// Render the sprite, tiling the image to the rect.
	// ~xPosition The screen position to render at.
	// ~xSize The size of the area from the position to tile over.
	inline void RenderTiled(xpoint xPosition, xpoint xSize)
	{
		RenderTiled(xPosition, xSize, xpoint(), GetImageRect(), 1.f);
	}

	// Get the width of the entire sprite image.
	xint GetImageWidth()
	{
		return _HGE->Texture_GetWidth(m_pMetadata->GetSprite()->GetTexture(), true);
	}

	// Get the height of the entire sprite image.
	xint GetImageHeight()
	{
		return _HGE->Texture_GetHeight(m_pMetadata->GetSprite()->GetTexture(), true);
	}

	// Get the width and height of the image.
	xpoint GetImageSize()
	{
		return xpoint(GetImageWidth(), GetImageHeight());
	}

	// Get the rect of the entire sprite image.
	xrect GetImageRect()
	{
		return xrect(0, 0, (xuint)GetImageWidth(), (xuint)GetImageHeight());
	}

	// Get the sprite metadata structure. This is for advanced use only.
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
	// Constructor.
	CSprite(CSpriteMetadata* pMetadata);

	// Render the sprite manually with all current attributes.
	virtual void Render()
	{
		OnRender();
	}

	// Automatic render called by the render manager.
	virtual void OnRender();

	// Set the current area to render.
	void SetArea(CSpriteMetadata::CArea* pArea)
	{
		m_pArea = pArea;
	}

	// Set the current area to render.
	void SetArea(const xchar* pName)
	{
		SetArea(m_pMetadata->FindArea(pName));
	}

	// Get the current area set for the sprite.
	CSpriteMetadata::CArea* GetArea()
	{
		return m_pArea;
	}

	// Set the area-relative anchor point to use when positioning and rotating the sprite.
	void SetAnchor(xpoint xAnchor)
	{
		m_xAnchor = xAnchor;
	}

	// Get the area-relative anchor point to use when positioning and rotating the sprite.
	xpoint GetAnchor()
	{
		return m_xAnchor;
	}

	// Set the sprite position. 
	// ~note The sprite is rendered from the anchor point at the specified screen position.
	void SetPosition(xpoint xPosition)
	{
		m_xPosition = xPosition;
	}

	// Add an offset to the current position.
	// ~note The sprite is rendered from the anchor point at the specified screen position.
	void OffsetPosition(xpoint xOffset)
	{
		SetPosition(GetPosition() + xOffset);
	}

	// Get the current sprite position in screen coordinates.
	// ~note The sprite is rendered from the anchor point at the specified screen position.
	xpoint GetPosition()
	{
		return m_xPosition;
	}

	// Set the alpha-blending level of the sprite.
	// ~iAlpha Value between 0.0f (transparent) and 1.0f (opaque).
	void SetAlpha(xfloat iAlpha)
	{
		m_fAlpha = Math::Clamp(iAlpha, 0.f, 1.f);
	}

	// Get the current alpha level of the sprite.
	// ~return Value between 0.0f (transparent) and 1.0f (opaque).
	xfloat GetAlpha()
	{
		return m_fAlpha;
	}

	// Set the rotation angle of the sprite.
	// ~bDegrees Specify true if the angle is in degrees, otherwise use radians.
	void SetAngle(xfloat fAngle, xbool bDegrees = false)
	{
		m_fAngle = !bDegrees ? fAngle : Math::Radians(fAngle);
	}

	// Get the rotation angle of the sprite.
	// ~bDegrees Specify true to get the angle in degrees, otherwise returns radians.
	xfloat GetAngle(xbool bDegrees = false)
	{
		return !bDegrees ? m_fAngle : Math::Degrees(m_fAngle);
	}

	// Get the width of the current area or the entire image if there is no active area.
	xint GetAreaWidth()
	{
		return m_pArea ? m_pArea->m_xRect.Width() : GetImageWidth();
	}

	// Get the height of the current area or the entire image if there is no active area.
	xint GetAreaHeight()
	{
		return m_pArea ? m_pArea->m_xRect.Height() : GetImageHeight();
	}

	// Get the centre point of the current area of the sprite.
	xpoint GetAreaCentre()
	{
		return xpoint(GetAreaWidth() / 2, GetAreaHeight() / 2);
	}

	// Get the screen area for the current area of the sprite.
	xrect GetScreenRect()
	{
		xpoint xTopLeft = m_xPosition - m_xAnchor;
		return xrect(xTopLeft.iX, xTopLeft.iY, xTopLeft.iX + GetAreaWidth(), xTopLeft.iY + GetAreaHeight());
	}

protected:
	// The current sprite area to render.
	CSpriteMetadata::CArea* m_pArea;

	// The current anchor point of the sprite.
	xpoint m_xAnchor;

	// The current position of the sprite.
	xpoint m_xPosition;

	// The current alpha level of the sprite.
	xfloat m_fAlpha;

	// The current rotation angle in radians.
	xfloat m_fAngle;
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
	typedef xfunction(2)<CAnimatedSprite* /*Sprite*/, const xchar* /*Event*/> t_AnimationEventCallback;

	// Constructor.
	CAnimatedSprite(CSpriteMetadata* pMetadata);

	// Update the sprite to the current animation frame.
	virtual void Update();

	// Render the sprite manually with all current attributes using the current animation frame.
	virtual void Render()
	{
		OnRender();
	}

	// Automatic render called by the render manager.
	virtual void OnRender();

	// Jump to a specific animation frame in the current animation.
	void SetFrame(CSpriteMetadata::CFrame* pFrame);

	// Jump to a specific animation frame in the current animation.
	void SetFrame(const xchar* pName)
	{
		SetFrame(m_pMetadata->FindFrame(m_pAnimation, pName));
	}

	// Get the currently active animation frame.
	CSpriteMetadata::CFrame* GetFrame()
	{
		return m_pFrame;
	}

	// Check if the active animation frame name matches the name specified.
	xbool IsActiveFrame(const xchar* pName)
	{
		return m_pFrame && String::IsMatch(m_pFrame->m_pName, pName);
	}

	// Set the active animation and reset states and timings to the first animation frame.
	// ~bPlay Start the animation if true or pause otherwise.
	void SetAnimation(CSpriteMetadata::CAnimation* pAnimation, xbool bPlay = false);

	// Set the active animation and reset states and timings to the first animation frame.
	// ~bPlay Start the animation if true or pause otherwise.
	void SetAnimation(const xchar* pName, xbool bPlay = false)
	{
		SetAnimation(m_pMetadata->FindAnimation(pName), bPlay);
	}

	// Get the currently active animation.
	CSpriteMetadata::CAnimation* GetAnimation()
	{
		return m_pAnimation;
	}

	// Check if the active animation name matches the name specified.
	xbool IsActiveAnimation(const xchar* pName)
	{
		return m_pAnimation && String::IsMatch(m_pAnimation->m_pName, pName);
	}

	// Play a specific animation from the first animation frame.
	void Play(CSpriteMetadata::CAnimation* pAnimation)
	{
		SetAnimation(pAnimation, true);
	}

	// Play a specific animation from the first animation frame.
	void Play(const xchar* pName)
	{
		SetAnimation(pName, true);
	}

	// Play the active animation from the first animation frame.
	void Play()
	{
		SetAnimation(m_pAnimation, true);
	}

	// Stop the active animation and reset to the first animation frame.
	void Stop()
	{
		SetAnimation(m_pAnimation, false);
	}

	// Pause the active animation keeping all current animation frame states and timings.
	void Pause()
	{
		m_bPlaying = false;
	}

	// Resume the active animation using the last known animation frame states and timings.
	void Resume()
	{
		m_bPlaying = true;
	}

	// Check if any active animation is playing.
	xbool IsPlaying()
	{
		return m_bPlaying;
	}

	// Set the callback to be executed if any events are specified
	// ~fpCallback Specify NULL to disable this callback.
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
	xbool m_bPlaying;

	// The delay timer used for frame timings.
	xuint m_iFrameTimer;

	// The animation event callback.
	t_AnimationEventCallback m_fpEvent;
};

////##############################################################################
