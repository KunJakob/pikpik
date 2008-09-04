//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Global.
#include <Global.h>

// Local.
#include <Metadata.h>

// Other.
#include <Crypto/cryptlib.h>
#include <Crypto/filters.h>
#include <Crypto/aes.h>
#include <Crypto/modes.h>

//##############################################################################

//##############################################################################
//
//                                   TYPES
//
//##############################################################################

// Namespaces.
using namespace CryptoPP;

//##############################################################################

//##############################################################################
//
//                                 DEFINITION
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         29-Jan-2008
// =============================================================================
CMetadata::CMetadata(const xchar* pMetadataFile, const xchar* pEncryptionKey, xbool bProcessImmediately) : CDataset(NULL, "Metadata", NULL),
	m_iSourceType(MetadataSourceType_Raw),
	m_iTask(MetadataTask_LoadFile), 
	m_iPercent(0),
	m_pError(NULL),
	m_pFile(NULL),
	m_hFile(INVALID_HANDLE_VALUE),
	m_iFileSize(0),
	m_iBytesRead(0),
	m_pData(NULL),
	m_pEncryptionKey(NULL),
	m_iTokenOffset(0),
	m_iTokenIndex(0),
	m_pDataset(NULL),
	m_pProperty(NULL)
{
	m_pFile = (xchar*)pMetadataFile;
	m_pDataset = this;

	if (pEncryptionKey)
	{
		m_iSourceType = MetadataSourceType_Encrypted;
		m_pEncryptionKey = (xchar*)pEncryptionKey;
	}

	if (bProcessImmediately)
	{
		while (!IsCompleted())
			Update();
	}
}

// =============================================================================
// Nat Ryall                                                         29-Jan-2008
// =============================================================================
void CMetadata::Update(xuint iTargetTime, xuint iChunkSize)
{
	xuint32 iTimeout = GetTickCount() + iTargetTime;

	while (GetTickCount() < iTimeout)
	{
		switch (m_iTask)
		{
		case MetadataTask_LoadFile:
			UpdateLoad(iChunkSize);
			break;

		case MetadataTask_DecryptData:
			UpdateDecrypt(iChunkSize);
			break;

		case MetadataTask_TokeniseData:
			UpdateTokenise();
			break;

		case MetadataTask_ParseTokens:
			UpdateParse();
			break;

		default:
			return;
		}
	}
}

// =============================================================================
// Nat Ryall                                                         29-Jan-2008
// =============================================================================
xuint CMetadata::GetProgress()
{
	switch (m_iTask)
	{
	case MetadataTask_LoadFile:
		return (m_iBytesRead * 100) / m_iFileSize;

	case MetadataTask_DecryptData:
		return 100;

	case MetadataTask_TokeniseData:
		return (m_iTokenOffset * 100) / m_iFileSize;

	case MetadataTask_ParseTokens:
		return (m_iTokenIndex * 100) / m_lpTokens.size();
	}

	return 0;
}

// =============================================================================
// Nat Ryall                                                         29-Jan-2008
// =============================================================================
void CMetadata::UpdateLoad(xuint iChunkSize)
{
	// Initialise the file.
	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		m_hFile = CreateFileA(m_pFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);

		if (m_hFile == INVALID_HANDLE_VALUE)
		{
			SetError(XFORMAT("Failed to open the file: %s.", m_pFile));
			return;
		}

		m_iFileSize = GetFileSize(m_hFile, NULL);

		if (m_iFileSize == 0)
		{
			SetError("File returned zero bytes.");
			return;
		}

		m_pData = new xchar[m_iFileSize + 1];
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
		if (m_iSourceType == MetadataSourceType_Encrypted)
			m_iTask = MetadataTask_DecryptData;
		else
			m_iTask = MetadataTask_TokeniseData;

		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}
}

// =============================================================================
// Nat Ryall                                                         20-Aug-2008
// =============================================================================
void CMetadata::UpdateDecrypt(xuint iChunkSize)
{
	// The first block of bytes in the file is the IV.
	xchar* pIV = m_pData;
	xchar* pEncryptedData = m_pData + AES::BLOCKSIZE;

	m_iFileSize -= AES::BLOCKSIZE;
	m_iBytesRead -= AES::BLOCKSIZE;

	// Allocate a new buffer for the decrypted file contents.
	xchar* pDecryptedData = new xchar[m_iFileSize + 1];
	pDecryptedData[m_iFileSize] = NULL;

	if (!pDecryptedData)
	{
		SetError("Failed to allocate decryption buffer.");
		return;
	}

	// Decrypt the contents into the new buffer.
	CFB_Mode<AES>::Decryption xAES((byte*)m_pEncryptionKey, String::Length(m_pEncryptionKey) / 2, (byte*)pIV);
	StringSource((byte*)pEncryptedData, m_iFileSize, true, new StreamTransformationFilter(xAES, new ArraySink((byte*)pDecryptedData, m_iFileSize)));

	// Start pointing to the decrypted data.
	delete [] m_pData;
	m_pData = pDecryptedData;

	// Now we have our decrypted data, start tokenising.
	m_iTask = MetadataTask_TokeniseData;
}

// =============================================================================
// Nat Ryall                                                         29-Jan-2008
// =============================================================================
void CMetadata::UpdateTokenise()
{
	// Ignore comments.
	if (m_pData[m_iTokenOffset] == '/' && m_pData[m_iTokenOffset + 1] == '/')
	{
		m_iTokenOffset += 2;

		while (m_pData[m_iTokenOffset] != NULL && m_pData[m_iTokenOffset] != '\n')
			m_iTokenOffset++;

		return;
	}

	// Find the next token.
	xuint iTokenStart = m_iTokenOffset;
	xbool bQuotes = false;

	while (m_pData[m_iTokenOffset] != NULL && (bQuotes || !iswspace(m_pData[m_iTokenOffset])))
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

	xuint iTokenLen = m_iTokenOffset - iTokenStart;

	if (iTokenLen)
	{
		// Terminate the string and add it to the token list.
		m_pData[m_iTokenOffset] = NULL;
		m_lpTokens.push_back(&m_pData[iTokenStart]);
	}
	else
		m_iTokenOffset++;

	if (m_iTokenOffset == m_iFileSize)
		m_iTask = MetadataTask_ParseTokens;
}

// =============================================================================
// Nat Ryall                                                         29-Jan-2008
// =============================================================================
void CMetadata::UpdateParse()
{
	xchar* pToken = m_lpTokens[m_iTokenIndex];

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
		m_iTask = MetadataTask_Completed;
}

// =============================================================================
// Nat Ryall                                                         29-Jan-2008
// =============================================================================
void CMetadata::SetError(const xchar* pError)
{
	Cleanup();

	m_iTask = MetadataTask_Error;
	m_pError = (xchar*)pError;

	XEXCEPTION(pError);
}

// =============================================================================
// Nat Ryall                                                         29-Jan-2008
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