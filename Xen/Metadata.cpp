//##############################################################################
// 
//	Metadata.cpp
//	--------------------------------------------------------------------------
//  Brief:		Describing data using an external, script-like file.
//	Project:	Xen
//	Author:		Nat Ryall
//	Date:		29/01/2008
//	License:	Private
//	--------------------------------------------------------------------------
//	Copyright (c) SAPIAN, 2008
// 
//##############################################################################

// Common.
#include <Xen/Common.h>

// Local.
#include <Xen/Metadata.h>

// Crypto.
#if defined(CRYPTOPP_CRYPTLIB_H)
	#include <Crypto/filters.h>
	#include <Crypto/aes.h>
	#include <Crypto/modes.h>
#endif

// Other.
#include <Xen/String.h>
#include <Xen/Exception.h>

//##############################################################################

// Namespaces.
#if defined(CRYPTOPP_CRYPTLIB_H)
	using namespace CryptoPP;
#endif

//##############################################################################

// =============================================================================
Xen::xchar Xen::CProperty::GetChar(xint iIndex)
{
	return m_lpValues[iIndex][0];
}

// =============================================================================
Xen::xint Xen::CProperty::GetInt(xint iIndex)
{
	return String::ToInt(m_lpValues[iIndex]);
}

// =============================================================================
Xen::xfloat Xen::CProperty::GetFloat(xint iIndex)
{
	return String::ToFloat(m_lpValues[iIndex]);
}

// =============================================================================
Xen::xbool Xen::CProperty::GetBool(xint iIndex)
{
	return (String::IsMatch(GetString(iIndex), "false") || String::IsMatch(GetString(iIndex), "0")) ? false : true;
}

// =============================================================================
const Xen::xchar* Xen::CProperty::GetString(xint iIndex)
{
	return (const xchar*)m_lpValues[iIndex];
}

//##############################################################################

// =============================================================================
Xen::CDataset::~CDataset()
{
	XEN_LIST_ERASE_ALL(m_lpDatasets);
	XEN_LIST_ERASE_ALL(m_lpProperties);
}

// =============================================================================
Xen::xint Xen::CDataset::GetDatasetCount(const xchar* pType)
{
	if (!pType)
		return (xint)m_lpDatasets.size();
	else
	{
		xint iCount = 0;

		XEN_LIST_FOREACH(t_DatasetList, ppDataset, m_lpDatasets)
		{
			if (String::IsMatch((*ppDataset)->GetType(), pType))
				++iCount;
		}

		return iCount;
	}
}

// =============================================================================
Xen::CDataset* Xen::CDataset::GetDataset(CDataset* pPrev, const xchar* pType, const xchar* pName)
{
	t_DatasetList::iterator ppDataset = m_lpDatasets.begin();

	if (pPrev)
	{
		for (; ppDataset < m_lpDatasets.end(); ppDataset++)
		{
			if (*ppDataset == pPrev)
			{
				ppDataset++;
				break;
			}
		}
	}

	for (; ppDataset < m_lpDatasets.end(); ppDataset++)
	{
		if (String::IsMatch((*ppDataset)->GetType(), pType))
		{
			if (!pName || ((*ppDataset)->GetName() && String::IsMatch((*ppDataset)->GetName(), pName)))
				return *ppDataset;
		}
	}

	return NULL;
}

// =============================================================================
Xen::xint Xen::CDataset::GetPropertyCount(const xchar* pType)
{
	if (!pType)
		return (xint)m_lpProperties.size();
	else
	{
		xint iCount = 0;

		XEN_LIST_FOREACH(t_PropertyList, ppProperty, m_lpProperties)
		{
			if (String::IsMatch((*ppProperty)->GetType(), pType))
				++iCount;
		}

		return iCount;
	}
}

// =============================================================================
Xen::CProperty* Xen::CDataset::GetProperty(CProperty* pPrev, const xchar* pType)
{
	t_PropertyList::iterator ppProperty = m_lpProperties.begin();

	if (pPrev)
	{
		for (; ppProperty < m_lpProperties.end(); ppProperty++)
		{
			if (*ppProperty == pPrev)
			{
				ppProperty++;
				break;
			}
		}
	}

	for (; ppProperty < m_lpProperties.end(); ppProperty++)
	{
		if (String::IsMatch((*ppProperty)->GetType(), pType))
			return *ppProperty;
	}

	return NULL;
}

//##############################################################################

// =============================================================================
Xen::CMetadata::CMetadata(const xchar* pMetadataFile, const xchar* pEncryptionKey, xbool bProcessImmediately) : CDataset(NULL, "Metadata", NULL),
	m_iSourceType(MetadataSourceType_Raw),
	m_iTask(MetadataTask_LoadFile), 
	m_pError(NULL),
	m_pFileName(NULL),
	m_pFile(NULL),
	m_iFileSize(0),
	m_pData(NULL),
	m_pEncryptionKey(NULL)
{
	m_pFileName = (xchar*)pMetadataFile;

#if defined(CRYPTOPP_CRYPTLIB_H)
	if (pEncryptionKey)
	{
		m_iSourceType = MetadataSourceType_Encrypted;
		m_pEncryptionKey = (xchar*)pEncryptionKey;
	}
#endif

	if (bProcessImmediately) 
	{
		while (!ProcessNextTask());
	}
}

// =============================================================================
Xen::xbool Xen::CMetadata::ProcessNextTask()
{
	switch (m_iTask)
	{
	case MetadataTask_LoadFile:
		Load();
		break;

#if defined(CRYPTOPP_CRYPTLIB_H)
	case MetadataTask_DecryptData:
		Decrypt();
		break;
#endif

	case MetadataTask_TokeniseData:
		Tokenise();
		break;

	case MetadataTask_ParseTokens:
		Parse();
		break;

	default:
		return true;
	}

	return IsCompleted();
}

// =============================================================================
void Xen::CMetadata::Load()
{
	m_pFile = FileManager.Open(m_pFileName, FileFlag_ReadOnly);

	if (!m_pFile)
	{
		SetError(XFORMAT("Failed to open the file: %s.", m_pFileName));
		return;
	}

	m_iFileSize = m_pFile->GetSize();

	if (m_iFileSize == -1)
	{
		SetError(XFORMAT("Could not access the file: %s.", m_pFileName));
		return;
	}

	if (m_iFileSize == 0)
	{
		SetError(XFORMAT("The file '%s' is empty.", m_pFileName));
		return;
	}

	m_pData = new xchar[m_iFileSize + 1];
	m_pData[m_iFileSize] = NULL;

	if (!m_pData)
	{
		SetError("Failed to allocate file buffer.");
		return;
	}

	if (m_pFile->Read(m_pData) != FileError_Success)
	{
		SetError(XFORMAT("Failed to read the file: %s.", m_pFileName));
	}

	FileManager.Close(m_pFile);
	m_pFile = NULL;

	// Now that we're finished.
#if defined(CRYPTOPP_CRYPTLIB_H)
	if (m_iSourceType == MetadataSourceType_Encrypted)
		m_iTask = MetadataTask_DecryptData;
	else
		m_iTask = MetadataTask_TokeniseData;
#else
	m_iTask = MetadataTask_TokeniseData;
#endif
}

// =============================================================================
void Xen::CMetadata::Decrypt()
{
#if defined(CRYPTOPP_CRYPTLIB_H)
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
#endif
}

// =============================================================================
void Xen::CMetadata::Tokenise()
{
	for (xint iTokenOffset = 0; iTokenOffset < m_iFileSize;)
	{
		// Ignore comments.
		if (m_pData[iTokenOffset] == '/' && m_pData[iTokenOffset + 1] == '/')
		{
			iTokenOffset += 2;

			while (m_pData[iTokenOffset] != NULL && m_pData[iTokenOffset] != '\n')
				iTokenOffset++;

			continue;
		}

		// Find the next token.
		xuint iTokenStart = iTokenOffset;
		xbool bQuotes = false;

		while (m_pData[iTokenOffset] != NULL && (bQuotes || !iswspace(m_pData[iTokenOffset])))
		{
			if (m_pData[iTokenOffset] == '"')
			{
				if (bQuotes)
					break;
				else
				{
					iTokenStart++;
					bQuotes = true;
				}
			}

			iTokenOffset++;
		}

		xuint iTokenLen = iTokenOffset - iTokenStart;

		if (iTokenLen)
		{
			// Terminate the string and add it to the token list.
			m_pData[iTokenOffset] = NULL;
			m_lpTokens.push_back(&m_pData[iTokenStart]);
		}
		else
			iTokenOffset++;
	}

	m_iTask = MetadataTask_ParseTokens;
}

// =============================================================================
void Xen::CMetadata::Parse()
{
	CDataset* pDataset = this;
	CProperty* pProperty = NULL;

	for (xint iTokenIndex = 0; iTokenIndex < (xint)m_lpTokens.size(); ++iTokenIndex)
	{
		xchar* pToken = m_lpTokens[iTokenIndex];

		if (!pToken)
			continue;

		switch (pToken[0])
		{
		case '.':
			{
				pProperty = new CProperty(pDataset, &pToken[1]);
				pDataset->m_lpProperties.push_back(pProperty);
			}
			break;

		case '{':
			{
				pDataset->m_lpProperties.pop_back();

				pDataset = new CDataset(pDataset, pProperty->GetType(), pProperty->GetValueCount() ? pProperty->GetString() : NULL);
				pDataset->m_pParent->m_lpDatasets.push_back(pDataset);

				delete pProperty;
				pProperty = NULL;
			}
			break;

		case '}':
			{
				pProperty = NULL;
				pDataset = pDataset->m_pParent;
			}
			break;

		default:
			{
				if (pProperty)
					pProperty->m_lpValues.push_back(pToken);
			}
		}
	}

	m_iTask = MetadataTask_Completed;
}

// =============================================================================
void Xen::CMetadata::Cleanup()
{
	if (m_pFile)
	{
		FileManager.Close(m_pFile);
		m_pFile = NULL;
	}

	if (m_pData)
	{
		delete [] m_pData;
		m_pData = NULL;
	}
}

// =============================================================================
void Xen::CMetadata::SetError(const xchar* pError)
{
	Cleanup();

	m_iTask = MetadataTask_Error;
	m_pError = (xchar*)pError;

	XEXCEPTION(pError);
}

//##############################################################################