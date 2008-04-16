#pragma once

/**
* @file Sprite.h
* @author Nat Ryall
* @date 10/02/2008
* @brief Manages rendering of sprites.
*
* Copyright © Creative Disorder
*/

#pragma region Include
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
#pragma endregion

#pragma region Types
//##############################################################################
//
//                                   TYPES
//
//##############################################################################

// Callbacks.
typedef void (*t_AnimationEventCallback)(const XCHAR* /*Event*/, void* /*Object*/);

//##############################################################################
#pragma endregion

#pragma region Templates
//##############################################################################
//
//                                 TEMPLATES
//
//##############################################################################

/**
* The surface template descriptor.
*/
class CSurfaceTemplate : public CResourceTemplate
{
public:
	// Available surface template flags.
	enum t_Flag
	{
		STF_None = 0x00000001, // This is a dummy flag and does nothing.
	};

	// Specifies an area within a surface.
	class CArea
	{
	public:
		// The name of the area to reference with.
		const XCHAR* pName;

		// Rect defining the pixels on the image to render.
		XRECT xRect;
	};

	// Type shortcut for an area list.
	typedef XLIST<CArea*> t_AreaList;

	// The internal surface rendering object.
	hgeSprite* pSurface;

	// The possible flags for the surface.
	XUINT iFlags;

	// The list of areas specified for the surface.
	t_AreaList lpAreas;
};

/**
* The sprite template descriptor.
*/
class CSpriteTemplate : public CResourceTemplate
{
public:
	// Describes a single frame of an animation.
	class CFrame
	{
	public:
		// The named identifier for the frame.
		const XCHAR* pName;

		// The area to render for the animation frame.
		CSurfaceTemplate::CArea* pArea;

		// The amount of time to hold the animation frame.
		XUINT iDelay;

		// The event string for this frame. This is NULL unless an event is set.
		const XCHAR* pEvent;

		// The next frame in the animation sequence. If this is NULL, the animation stops here.
		CFrame* pNextFrame;
	};

	// Type shortcut for a frame list.
	typedef XLIST<CFrame*> t_FrameList;

	// Describes an animation of frames using the specified surface.
	class CAnimation
	{
	public:
		// The named identifier of the animation.
		const XCHAR* pName;

		// The surface image to render the animation with.
		CSurfaceTemplate* pSurfaceTemplate;

		// Animation time.
		XUINT iTime;

		// A list of frames used in the animation.
		t_FrameList lpFrames;
	};

	// Type shortcut for an animation list.
	typedef XLIST<CAnimation*> t_AnimationList;

	// The default surface image to render with.
	CSurfaceTemplate* pSurfaceTemplate;

	// The default position to place the sprite at.
	XPOINT xInitialPosition;

	// The default animation for the sprite.
	CAnimation* pInitialAnimation;

	// The animations available in the sprite.
	t_AnimationList lpAnimations;
};

//##############################################################################
#pragma endregion

#pragma region Sprite
//##############################################################################
//
//                                   SPRITE
//
//##############################################################################
class CSprite : public CResource, public CRenderable
{
public:
	/**
	* Constructor: Create a new sprite from a sprite template.
	*/
	CSprite(CSpriteTemplate* pTemplate);

	/**
	* Render the sprite at the current position with the current animation.
	*/
	virtual void Render();

	/**
	* Find an area by name from a surface template.
	*/
	static CSurfaceTemplate::CArea* FindArea(CSurfaceTemplate* pTemplate, const XCHAR* pName);

	/**
	* Find an area by name from the surface.
	*/
	CSurfaceTemplate::CArea* FindArea(const XCHAR* pName)
	{
		return FindArea(GetSurfaceTemplate(), pName);
	}

	/**
	* Set the current surface area to render.
	*/
	void SetArea(CSurfaceTemplate::CArea* pArea)
	{
		m_pArea = pArea;
	}

	/**
	* Set the current surface area to render by name.
	*/
	void SetArea(const XCHAR* pName)
	{
		SetArea(FindArea(pName));
	}

	/**
	* Get the current area set for the sprite.
	*/
	CSurfaceTemplate::CArea* GetArea()
	{
		return m_pArea;
	}

	/**
	* Set the sprite position in screen coordinates.
	* @param bCentre Use the centre point of the sprite to offset the image.
	*/
	void SetPosition(XPOINT xPosition, XBOOL bCentre = true)
	{
		m_xPosition = bCentre ? xPosition : xPosition + (XPOINT(GetWidth(), GetHeight()) / 2);
	}

	/**
	* Get the current sprite position in screen coordinates.
	*/
	XPOINT GetPosition(XBOOL bCentre = true)
	{
		return bCentre ? m_xPosition : m_xPosition - (XPOINT(GetWidth(), GetHeight()) / 2);
	}

	/**
	* Set the alpha level of the sprite.
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
	* @param bDegrees Specify true if the angle is in degrees, otherwise the system assumes radians.
	*/
	void SetRotation(XFLOAT fAngle, XBOOL bDegrees = false)
	{
		m_fRotation = !bDegrees ? fAngle : (fAngle / 180.0f) * M_PI;
	}

	/**
	* Get the rotation angle of the sprite.
	* @param bDegrees Specify true to get the angle in degrees, otherwise returns the angle in radians.
	*/
	XFLOAT GetRotation(XBOOL bDegrees = false)
	{
		return !bDegrees ? m_fRotation : m_fRotation * (180.0f / M_PI);
	}

	/**
	* Get the width of the current frame.
	*/
	XUINT GetWidth()
	{
		return m_pArea ? m_pArea->xRect.GetWidth() : GetSurfaceWidth();
	}

	/**
	* Get the height of the current frame.
	*/
	XUINT GetHeight()
	{
		return m_pArea ? m_pArea->xRect.GetHeight() : GetSurfaceHeight();
	}

	/**
	* Get the width of the entire surface.
	*/
	XUINT GetSurfaceWidth()
	{
		return (XUINT)GetSurfaceTemplate()->pSurface->GetWidth();
	}

	/**
	* Get the height of the entire surface.
	*/
	XUINT GetSurfaceHeight()
	{
		return (XUINT)GetSurfaceTemplate()->pSurface->GetHeight();
	}

	/**
	* Get a screen area rect for the current frame.
	*/
	XRECT GetScreenRect(XPOINT xPosition)
	{
		return XRECT(m_xPosition.iX, m_xPosition.iY, m_xPosition.iX + GetWidth(), m_xPosition.iY + GetHeight());
	}

	/**
	* Return the current sprite template. This is for advanced use only.
	*/
	CSpriteTemplate* GetSpriteTemplate()
	{
		return m_pSpriteTemplate;
	}

	/**
	* Return the current surface template. This is for advanced use only.
	*/
	virtual CSurfaceTemplate* GetSurfaceTemplate()
	{
		return m_pSpriteTemplate->pSurfaceTemplate;
	}

protected:
	// The sprite template containing rendering information.
	CSpriteTemplate* m_pSpriteTemplate;

	// The visibility status of the sprite.
	//XBOOL m_bVisible;

	// The current position of the sprite.
	XPOINT m_xPosition;

	// The current alpha level of the sprite.
	XFLOAT m_fAlpha;

	// The current rotation angle in radians.
	XFLOAT m_fRotation;

	// The current area for the surface.
	CSurfaceTemplate::CArea* m_pArea;
};

//##############################################################################
#pragma endregion

#pragma region Animated Sprite
//##############################################################################
//
//                               ANIMATED SPRITE
//
//##############################################################################
class CAnimatedSprite : public CSprite
{
public:
	/**
	* Constructor: Create a new animated sprite from a sprite template.
	*/
	CAnimatedSprite(CSpriteTemplate* pTemplate);

	/**
	* Render the sprite at the current position with the current animation.
	*/
	virtual void Render();

	/**
	* Find an animation by name from the specified sprite template.
	*/
	static CSpriteTemplate::CAnimation* FindAnimation(CSpriteTemplate* pTemplate, const XCHAR* pName);

	/**
	* Find an animation by name.
	*/
	CSpriteTemplate::CAnimation* FindAnimation(const XCHAR* pName)
	{
		return FindAnimation(m_pSpriteTemplate, pName);
	}

	/**
	* Set the current animation playing. This will not change the play state.
	*/
	void SetAnimation(CSpriteTemplate::CAnimation* pAnimation);

	/**
	* Get the current animation set within the sprite.
	*/
	CSpriteTemplate::CAnimation* GetAnimation()
	{
		return m_pAnimation;
	}

	/**
	* Check if the current animation name matches the name specified.
	*/
	XBOOL IsCurrentAnimation(const XCHAR* pName)
	{
		return strcmp(m_pAnimation->pName, pName) == 0;
	}

	/**
	* Check if the sprite is currently animated.
	*/
	XBOOL IsAnimated()
	{
		return m_pAnimation != NULL;
	}

	/**
	* Reset all frames and timings and play the specified animation.
	*/
	void Play(CSpriteTemplate::CAnimation* pAnimation);

	/**
	* Reset all frames and timings and play the specified animation.
	*/
	void Play(const XCHAR* pName)
	{
		Play(FindAnimation(pName));
	}

	/**
	* Reset all frames and timings and play the existing animation.
	*/
	void Play();

	/**
	* Stop the animation from playing but keep the current frame and timer.
	*/
	void Pause()
	{
		m_bPlaying = false;
	}

	/**
	* Resume the animation playback from a previously paused state.
	*/
	void Resume()
	{
		m_bPlaying = true;
	}

	/**
	* Stop the animation from playing and reset the animation frame back to the first.
	*/
	void Stop();

	/**
	* Check if any animation is currently playing.
	*/
	XBOOL IsPlaying()
	{
		return m_bPlaying;
	}

	/**
	* Find a frame by name from the specified animation.
	*/
	static CSpriteTemplate::CFrame* FindFrame(CSpriteTemplate::CAnimation* pAnimation, const XCHAR* pName);

	/**
	* Find a frame by name from an animation specified by name also.
	*/
	CSpriteTemplate::CFrame* FindFrame(const XCHAR* pAnimName, const XCHAR* pFrameName)
	{
		return FindFrame(FindAnimation(pAnimName), pFrameName);
	}

	/**
	* Set the current animation frame within the sprite.
	*/
	void SetFrame(CSpriteTemplate::CFrame* pFrame);

	/**
	* Set the current animation frame by name.
	*/
	void SetFrame(const XCHAR* pName)
	{
		SetFrame(FindFrame(m_pAnimation, pName));
	}

	/**
	* Get the current animation frame set within the sprite.
	*/
	CSpriteTemplate::CFrame* GetFrame()
	{
		return m_pFrame;
	}

	/**
	* Check if the current animation frame name matches the name specified.
	*/
	XBOOL IsCurrentFrame(const XCHAR* pName)
	{
		return m_pFrame->pName && strcmp(m_pFrame->pName, pName) == 0;
	}

	/**
	* Set the callback for all the sprite's animation events.
	* @param fpCallback Specify NULL to disable this callback.
	*/
	void SetEventCallback(t_AnimationEventCallback fpCallback, void* pObject = NULL)
	{
		m_fpEvent = fpCallback;
		m_pEventObject = pObject;
	}

	/**
	* Return the current surface template. This is for advanced use only.
	*/
	virtual CSurfaceTemplate* GetSurfaceTemplate()
	{
		return m_pAnimation->pSurfaceTemplate;
	}

protected:
	// The delay timer used for frame timings.
	XUINT m_iTimer;

	// The playback control flag.
	XBOOL m_bPlaying;

	// The animation event callback.
	t_AnimationEventCallback m_fpEvent;

	// The animation event custom object pointer.
	void* m_pEventObject;

	// The current frame of the animation we are playing.
	CSpriteTemplate::CFrame* m_pFrame;

	// The current animation we are playing.
	CSpriteTemplate::CAnimation* m_pAnimation;
};

//##############################################################################
#pragma endregion