//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Global.
#include <Global.h>

// Local.
#include <Interface.h>

//##############################################################################

//##############################################################################
//
//                                   MODULE
//
//##############################################################################
/*static class CInterfaceModule : public Xen::CModule
{
public:
  // Constructor.
  CInterfaceModule()
  {
    XMODULE(this);
  }

  // Initialise.
  virtual void Initialise()
  {
  }

  // Update.
  virtual void Update()
  {
  }

  // Render.
  virtual void Render()
  {
  }

  // Deinitialise.
  virtual void Deinitialise()
  {
  }
} 
s_Module;*/

//##############################################################################

//##############################################################################
//
//                                   DIALOG
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         27-Apr-2008
// =============================================================================
CDialog::CDialog() : CRenderable(RenderableType_Dialog),
  m_iWidth(1),
  m_iHeight(1)
{
  m_pSprite = new CBasicSprite(_SPRITE("Dialog-Tiles"));
}

// =============================================================================
// Nat Ryall                                                         27-Apr-2008
// =============================================================================
CDialog::~CDialog()
{
  delete m_pSprite;
}

// =============================================================================
// Nat Ryall                                                         27-Apr-2008
// =============================================================================
void CDialog::Render()
{
  CSpriteMetadata* pMetadata = m_pSprite->GetMetadata();

  XUINT iRealWidth = 8 + (m_iWidth * 8);
  XUINT iRealHeight = 8 + (m_iHeight * 8);

  m_pSprite->Render(pMetadata->FindArea("TL")->xRect, XPOINT(), m_xPosition, 1.f, 0.f);
  m_pSprite->Render(pMetadata->FindArea("TR")->xRect, XPOINT(), m_xPosition + XPOINT(iRealWidth, 0), 1.f, 0.f);
  m_pSprite->Render(pMetadata->FindArea("BL")->xRect, XPOINT(), m_xPosition + XPOINT(0, iRealHeight), 1.f, 0.f);
  m_pSprite->Render(pMetadata->FindArea("BR")->xRect, XPOINT(), m_xPosition + XPOINT(iRealWidth, iRealHeight), 1.f, 0.f);

  for (XUINT iA = 0; iA < m_iWidth; ++iA)
  {
    m_pSprite->Render(pMetadata->FindArea("TM")->xRect, XPOINT(), m_xPosition + XPOINT(8 + (iA * 8), 0), 1.f, 0.f);
    m_pSprite->Render(pMetadata->FindArea("BM")->xRect, XPOINT(), m_xPosition + XPOINT(8 + (iA * 8), iRealHeight), 1.f, 0.f);
  }

  for (XUINT iA = 0; iA < m_iHeight; ++iA)
  {
    m_pSprite->Render(pMetadata->FindArea("ML")->xRect, XPOINT(), m_xPosition + XPOINT(0, 8 + (iA * 8)), 1.f, 0.f);
    m_pSprite->Render(pMetadata->FindArea("MR")->xRect, XPOINT(), m_xPosition + XPOINT(iRealWidth, 8 + (iA * 8)), 1.f, 0.f);
  }

  for (XUINT iY = 0; iY < m_iHeight; ++iY)
  {
    for (XUINT iX = 0; iX < m_iWidth; ++iX)
      m_pSprite->Render(pMetadata->FindArea("MM")->xRect, XPOINT(), m_xPosition + XPOINT(8 + (iX * 8), 8 + (iY * 8)), 1.f, 0.f);
  }
}

//##############################################################################