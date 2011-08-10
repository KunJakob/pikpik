//##############################################################################
// 
//	File.cpp
//	--------------------------------------------------------------------------
//  Brief:		Transparent file management on various platforms.
//	Project:	Xen
//	Author:		Nat Ryall
//	Date:		17/11/2008
//	License:	Private
//	--------------------------------------------------------------------------
//	Copyright (c) SAPIAN, 2008
// 
//##############################################################################

// Common.
#include <Xen/Common.h>

// Local.
#include <Xen/File.h>

// Other.
#include <Xen/Exception.h>

//##############################################################################

// =============================================================================
Xen::CFile::CFile()
{
	Close();
}

// =============================================================================
Xen::CFile::~CFile()
{
}

// =============================================================================
Xen::t_FileError Xen::CFile::Close()
{
	m_pFile = NULL;
	m_iFlags = 0;

	return FileError_Success;
}

// =============================================================================
void Xen::CFile::SetFlags(xuint iFlags)
{
	XASSERT(!XFLAGISSET(iFlags, FileFlag_ReadOnly) || !XFLAGISSET(iFlags, FileFlag_WriteOnly));
	m_iFlags = iFlags;
}

//##############################################################################

#if XWINDOWS
//##############################################################################

// =============================================================================
Xen::CWinFile::CWinFile() :
	m_hFile(INVALID_HANDLE_VALUE)
{
}

// =============================================================================
Xen::CWinFile::~CWinFile()
{
	Close();
}

// =============================================================================
Xen::t_FileError Xen::CWinFile::Create(const xchar* pFile, xuint iFlags)
{
	if (!pFile)
		return FileError_InvalidParam;

	if (IsOpen())
		return FileError_AlreadyOpen;

	DWORD iFileAccess = GENERIC_READ | GENERIC_WRITE;
	DWORD iShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
	DWORD iOpenMode = CREATE_NEW;

	if (XFLAGISSET(iFlags, FileFlag_ReadOnly))
	{
		iFileAccess = GENERIC_READ;
		iShareMode = FILE_SHARE_READ;
	}
	else if (XFLAGISSET(iFlags, FileFlag_WriteOnly))
	{
		iFileAccess = GENERIC_WRITE;
		iShareMode = FILE_SHARE_WRITE;
	}

	if (!XFLAGISSET(iFlags, FileFlag_ShareContents))
		iShareMode = 0;

	if (XFLAGISSET(iFlags, FileFlag_OverwriteExisting))
		iOpenMode = CREATE_ALWAYS;

	m_hFile = CreateFile(pFile, iFileAccess, iShareMode, NULL, iOpenMode, FILE_ATTRIBUTE_NORMAL, NULL);

	if (!IsOpen())
	{
		switch (GetLastError())
		{
		case ERROR_ALREADY_EXISTS:
			return FileError_AlreadyExists;

		case ERROR_ACCESS_DENIED:
			return FileError_AccessDenied;

		default:
			return FileError_OperationFailed;
		}
	}

	m_pFile = pFile;
	SetFlags(iFlags);

	return FileError_Success;
}

// =============================================================================
Xen::t_FileError Xen::CWinFile::Open(const xchar* pFile, xuint iFlags)
{
	if (!pFile)
		return FileError_InvalidParam;

	if (IsOpen())
		return FileError_AlreadyOpen;

	DWORD iFileAccess = GENERIC_READ | GENERIC_WRITE;
	DWORD iShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
	DWORD iOpenMode = OPEN_EXISTING;

	if (XFLAGISSET(iFlags, FileFlag_ReadOnly))
	{
		iFileAccess = GENERIC_READ;
		iShareMode = FILE_SHARE_READ;
	}
	else if (XFLAGISSET(iFlags, FileFlag_WriteOnly))
	{
		iFileAccess = GENERIC_WRITE;
		iShareMode = FILE_SHARE_WRITE;
	}

	if (!XFLAGISSET(iFlags, FileFlag_ShareContents))
		iShareMode = 0;

	if (XFLAGISSET(iFlags, FileFlag_ClearContents))
		iOpenMode = TRUNCATE_EXISTING;

	m_hFile = CreateFile(pFile, iFileAccess, iShareMode, NULL, iOpenMode, FILE_ATTRIBUTE_NORMAL, NULL);

	if (!IsOpen())
	{
		switch (GetLastError())
		{
		case ERROR_NOT_FOUND:
			return FileError_NotFound;

		case ERROR_ACCESS_DENIED:
			return FileError_AccessDenied;

		default:
			return FileError_OperationFailed;
		}
	}

	m_pFile = pFile;
	SetFlags(iFlags);

	return FileError_Success;
}

// =============================================================================
Xen::t_FileError Xen::CWinFile::Close()
{
	if (!IsOpen())
		return FileError_NotOpen;

	Flush();

	CFile::Close();

	CloseHandle(m_hFile);
	m_hFile = INVALID_HANDLE_VALUE;

	return FileError_Success;
}

// =============================================================================
Xen::xint Xen::CWinFile::GetSize()
{
	if (!IsOpen())
		return -1;

	DWORD iFileSize = GetFileSize(m_hFile, NULL);
	
	if (iFileSize == UINT_MAX)
		return -1;
	else
		return (xint)iFileSize;
}

// =============================================================================
Xen::t_FileError Xen::CWinFile::Read(void* pBuffer, xint iBytes, xint* pBytesRead)
{
	if (iBytes == -1)
		iBytes = GetSize();

	if (!pBuffer || iBytes <= 0)
		return FileError_InvalidParam;

	if (!IsOpen())
		return FileError_NotOpen;

	if (IsFlagSet(FileFlag_WriteOnly))
		return FileError_AccessDenied;

	DWORD iBytesRead = 0;
	BOOL bResult = ReadFile(m_hFile, (LPVOID)pBuffer, iBytes, &iBytesRead, NULL);

	if (bResult == FALSE || iBytesRead == 0)
		return FileError_OperationFailed;

	if (pBytesRead)
		*pBytesRead = iBytesRead;

	return FileError_Success;
}

// =============================================================================
Xen::t_FileError Xen::CWinFile::Write(const void* pBuffer, xint iBytes, xint* pBytesWritten)
{
	if (!pBuffer || iBytes <= 0)
		return FileError_InvalidParam;

	if (!IsOpen())
		return FileError_NotOpen;

	if (IsFlagSet(FileFlag_ReadOnly))
		return FileError_AccessDenied;

	DWORD iBytesWritten = 0;
	BOOL bResult = WriteFile(m_hFile, (LPVOID)pBuffer, iBytes, &iBytesWritten, NULL);

	if (bResult == FALSE || iBytesWritten == 0)
		return FileError_OperationFailed;

	if (pBytesWritten)
		*pBytesWritten = iBytesWritten;

	return FileError_Success;
}

// =============================================================================
Xen::t_FileError Xen::CWinFile::Flush()
{
	if (!IsOpen())
		return FileError_NotOpen;

	if (FlushFileBuffers(m_hFile) == TRUE)
		return FileError_Success;
	else
		return FileError_OperationFailed;
}

// =============================================================================
Xen::t_FileError Xen::CWinFile::SetOffset(xint iOffset)
{
	if (iOffset < 0)
		return FileError_InvalidParam;

	if (!IsOpen())
		return FileError_NotOpen;

	if (SetFilePointer(m_hFile, (DWORD)iOffset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		return FileError_OperationFailed;
	else
		return FileError_Success;
}

// =============================================================================
Xen::t_FileError Xen::CWinFile::GetOffset(xint* pOffset)
{
	if (!pOffset)
		return FileError_InvalidParam;

	*pOffset = 0;

	if (!IsOpen())
		return FileError_NotOpen;

	*pOffset = SetFilePointer(m_hFile, 0, NULL, FILE_CURRENT);

	if (*pOffset == INVALID_SET_FILE_POINTER)
	{
		*pOffset = 0;
		return FileError_OperationFailed;
	}
	
	return FileError_Success;
}

//##############################################################################
#endif

//##############################################################################

// =============================================================================
void Xen::CFileManager::OnDeinitialise()
{
	XLISTFOREACH(t_FileList, ppFile, m_lpFiles)
	{
		if ((*ppFile)->IsOpen())
			(*ppFile)->Close();
	}

	m_lpFiles.clear();
}

#if XWINDOWS
// =============================================================================
Xen::CWinFile* Xen::CFileManager::Create(const xchar* pFileName, xuint iFlags)
{
	CWinFile* pFile = new CWinFile();
	
	m_iLastError = pFile->Create(pFileName, iFlags);

	if (m_iLastError == FileError_Success)
	{
		m_lpFiles.push_back(pFile);
		return pFile;
	}
	else
	{
		delete pFile;
		return NULL;
	}
}

// =============================================================================
Xen::CWinFile* Xen::CFileManager::Open(const xchar* pFileName, xuint iFlags)
{
	CWinFile* pFile = new CWinFile();

	m_iLastError = pFile->Open(pFileName, iFlags);

	if (m_iLastError == FileError_Success)
	{
		m_lpFiles.push_back(pFile);
		return pFile;
	}
	else
	{
		delete pFile;
		return NULL;
	}
}


// =============================================================================
Xen::t_FileScanResult Xen::CFileManager::Scan(const xchar* pPath)
{
	WIN32_FIND_DATA xFindData;
	t_FileScanResult lsResult;

	HANDLE pHandle = FindFirstFile(pPath, &xFindData);
	BOOL bFoundFile = pHandle != INVALID_HANDLE_VALUE;

	while (bFoundFile)
	{
		if (!(xFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			lsResult.push_back(xFindData.cFileName);

		bFoundFile = FindNextFile(pHandle, &xFindData);
	}

	if (pHandle != INVALID_HANDLE_VALUE)
		FindClose(pHandle);

	return lsResult;
}
#endif

// =============================================================================
void Xen::CFileManager::Close(CFile* pFile)
{
	XASSERT(pFile);

	if (pFile->IsOpen())
		pFile->Close();

	XLISTREMOVE(t_FileList, m_lpFiles, pFile);

	delete pFile;
}

//##############################################################################