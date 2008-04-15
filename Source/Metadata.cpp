#pragma region Include
//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Global.
#include <Global.h>

// Local.
#include <Metadata.h>

//##############################################################################
#pragma endregion

#pragma region Definition
//##############################################################################
//
//                                 DEFINITION
//
//##############################################################################

// =============================================================================
// Author: Nat Ryall                                           Date: 29-Jan-2008
// =============================================================================
CMetadata::CMetadata(const XCHAR* pFilePath, XBOOL bExecute) 
: 
  CDataset(NULL, "Metadata", NULL),
  m_iTask(ST_LoadFile), 
  m_iPercent(0),
  m_pError(NULL),
	m_pFile(NULL),
  m_hFile(INVALID_HANDLE_VALUE),
	m_iFileSize(0),
	m_iBytesRead(0),
	m_pData(NULL),
  m_iTokenOffset(0),
  m_iTokenIndex(0),
	m_pDataset(NULL),
	m_pProperty(NULL)
{
  m_pFile = (XCHAR*)pFilePath;
  m_pDataset = this;

  if (bExecute)
  {
    while (!IsCompleted())
      Update();
  }
}

// =============================================================================
// Author: Nat Ryall                                           Date: 29-Jan-2008
// =============================================================================
void CMetadata::Update(XUINT iMaxTime, XUINT iChunkSize)
{
  XUINT32 iTimeout = GetTickCount() + iMaxTime;

  while (GetTickCount() < iTimeout)
  {
    switch (m_iTask)
    {
      case ST_LoadFile:
        UpdateLoad(iChunkSize);
      break;

      case ST_TokeniseData:
        UpdateTokenise();
      break;

      case ST_ParseTokens:
        UpdateParse();
      break;

      default:
        return;
    }
  }
}

// =============================================================================
// Author: Nat Ryall                                           Date: 29-Jan-2008
// =============================================================================
XUINT CMetadata::GetProgress()
{
  switch (m_iTask)
  {
    case ST_LoadFile:
      return (m_iBytesRead * 100) / m_iFileSize;

    case ST_TokeniseData:
      return (m_iTokenOffset * 100) / m_iFileSize;

    case ST_ParseTokens:
      return (m_iTokenIndex * 100) / m_lpTokens.size();
  }

  return 0;
}

// =============================================================================
// Author: Nat Ryall                                           Date: 29-Jan-2008
// =============================================================================
void CMetadata::UpdateLoad(XUINT iChunkSize)
{
  // Initialise the file.
  if (m_hFile == INVALID_HANDLE_VALUE)
  {
    m_hFile = CreateFileA(m_pFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);

    if (m_hFile == INVALID_HANDLE_VALUE)
    {
      SetError(XFORMAT("Failed to open the file.", m_pFile));
      return;
    }

    m_iFileSize = GetFileSize(m_hFile, NULL);

    if (m_iFileSize == 0)
    {
      SetError("File returned zero bytes.");
      return;
    }

    m_pData = new XCHAR[m_iFileSize + 1];
    m_pData[m_iFileSize] = NULL;

    if (!m_pData)
    {
      SetError("Failed to allocate file buffer.");
      return;
    }

    m_iBytesRead = 0;
  }

  // Read the file contents.
  DWORD iBytesRead = 0;

  if (!ReadFile(m_hFile, &m_pData[m_iBytesRead], iChunkSize, &iBytesRead, NULL))
  {
    SetError(XFORMAT("Failed when reading file chunk (Error: #%d).", GetLastError()));
    return;
  }

  m_iBytesRead += iBytesRead;

  // Check if we're finished.
  if (m_iBytesRead == m_iFileSize)
  {
    m_iTask = ST_TokeniseData;

    CloseHandle(m_hFile);
    m_hFile = INVALID_HANDLE_VALUE;
  }
}

// =============================================================================
// Author: Nat Ryall                                           Date: 29-Jan-2008
// =============================================================================
void CMetadata::UpdateTokenise()
{
  // Ignore comments.
  if (m_pData[m_iTokenOffset] == '/' && m_pData[m_iTokenOffset + 1] == '/')
  {
    m_iTokenOffset += 2;

    while (m_pData[m_iTokenOffset] != '\n' && m_pData[m_iTokenOffset] != NULL)
      m_iTokenOffset++;

    return;
  }
  
  // Find the next token.
  XUINT iTokenStart = m_iTokenOffset;
  XBOOL bQuotes = false;

  while ((bQuotes || !iswspace(m_pData[m_iTokenOffset])) && m_pData[m_iTokenOffset] != NULL)
  {
    if (m_pData[m_iTokenOffset] == '"')
    {
      if (bQuotes)
        break;
      else
      {
        iTokenStart++;
        bQuotes = true;
      }
    }

    m_iTokenOffset++;
  }

  XUINT iTokenLen = m_iTokenOffset - iTokenStart;

  if (iTokenLen)
  {
    // Terminate the string and add it to the token list.
    m_pData[m_iTokenOffset] = NULL;
    m_lpTokens.push_back(&m_pData[iTokenStart]);

    if (m_iTokenOffset == m_iFileSize)
      m_iTask = ST_ParseTokens;
  }
  else
    m_iTokenOffset++;
}

// =============================================================================
// Author: Nat Ryall                                           Date: 29-Jan-2008
// =============================================================================
void CMetadata::UpdateParse()
{
  XCHAR* pToken = m_lpTokens[m_iTokenIndex];

  if (!pToken)
    return;

	switch (pToken[0])
	{
	case '.':
		{
			m_pProperty = new CProperty(m_pDataset, &pToken[1]);
			m_pDataset->m_lpProperties.push_back(m_pProperty);
		}
		break;

	case '{':
		{
			m_pDataset->m_lpProperties.pop_back();

			m_pDataset = new CDataset(m_pDataset, m_pProperty->GetType(), m_pProperty->GetValueCount() ? m_pProperty->GetString() : NULL);
			m_pDataset->m_pParent->m_lpDatasets.push_back(m_pDataset);

			delete m_pProperty;
			m_pProperty = NULL;
		}
		break;

	case '}':
		{
			m_pProperty = NULL;
			m_pDataset = m_pDataset->m_pParent;
		}
		break;

	default:
		{
			if (m_pProperty)
				m_pProperty->m_lpValues.push_back(pToken);
		}
	}

  if (++m_iTokenIndex == m_lpTokens.size())
    m_iTask = ST_Completed;
}

// =============================================================================
// Author: Nat Ryall                                           Date: 29-Jan-2008
// =============================================================================
void CMetadata::SetError(const XCHAR* pError)
{
  Cleanup();

  m_iTask = ST_Error;
  m_pError = (XCHAR*)pError;

	XEXCEPTION(pError);
}

// =============================================================================
// Author: Nat Ryall                                           Date: 29-Jan-2008
// =============================================================================
void CMetadata::Cleanup()
{
  if (m_hFile != INVALID_HANDLE_VALUE)
  {
    CloseHandle(m_hFile);
    m_hFile = INVALID_HANDLE_VALUE;
  }

  if (m_pData)
  {
    delete [] m_pData;
    m_pData = NULL;
  }
}

//##############################################################################
#pragma endregion