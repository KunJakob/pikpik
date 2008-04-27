#pragma once

//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Global.
#include <Global.h>

// Other.
#include <Renderer.h>
#include <Resource.h>
#include <Sprite.h>

//##############################################################################

//##############################################################################
//
//                                   DIALOG
//
//##############################################################################
class CDialog : public CRenderable
{
public:
  /**
  * 
  */
  CDialog();

  /**
  * 
  */
  virtual ~CDialog();

  /**
  * 
  */
  virtual void Render();


  /**
  * 
  */
  void SetPosition(XPOINT xPosition)
  {
    m_xPosition = xPosition;
  }

  /**
  * 
  */
  XPOINT GetPosition()
  {
    return m_xPosition;
  }

  /**
  * 
  */
  void SetSize(XUINT iWidth, XUINT iHeight)
  {
    m_iWidth = iWidth;
    m_iHeight = iHeight;
  }

  /**
  * 
  */
  XUINT GetWidth()
  {
    return m_iWidth;
  }

  /**
  * 
  */
  XUINT GetHeight()
  {
    return m_iHeight;
  }

protected:
  //
  CBasicSprite* m_pSprite;

  //
  XPOINT m_xPosition;

  //
  XUINT m_iWidth;

  //
  XUINT m_iHeight;
};

//##############################################################################