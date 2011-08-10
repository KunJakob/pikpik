#pragma once

//##############################################################################
// 
//	File.h
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

// System.
#if XWINDOWS
	// Windows.
	#include <Windows.h>
#endif

// Other.
#include <Xen/Module.h>

//##############################################################################

// Shortcuts.
#define FileManager \
		CFileManager::Get()

//##############################################################################
namespace Xen
{
	// Predeclare.
	class CFile;

	// The possible file operation errors that can occur.
	enum t_FileError
	{
		FileError_Success,				// The operation was successful.
		FileError_InvalidParam,			// An invalid parameter was specified for the operation.
		FileError_NotOpen,				// There is no valid file handle because the file is not open.
		FileError_AlreadyOpen,			// The file handle is already open and must be closed first.
		FileError_OperationFailed,		// The operation failed with no specific error.
		FileError_NotFound,				// The file specified does not exist or is invalid.
		FileError_AlreadyExists,		// The file specified already exists.
		FileError_AccessDenied,			// The specified file cannot be accessed due to insufficient permissions.
	};

	// The file operation flags.
	enum t_FileFlag
	{
		FileFlag_ShareContents			= XBIT(0),		// (CO) The file should not aquire locked access allowing multiple access.
		FileFlag_BufferContents			= XBIT(1),		// (--) The file should be read into an internal buffer for faster access (not currently supported).
		FileFlag_ReadOnly				= XBIT(2),		// (CO) The file should only be opened for reading.
		FileFlag_WriteOnly				= XBIT(3),		// (CO) The file should only be opened for writing.
		FileFlag_ClearContents			= XBIT(4),		// (-O) The file should have all its contents erased when successfully opened.
		FileFlag_OverwriteExisting		= XBIT(5),		// (C-) The file should overwrite any existing file with the same path.
	};

	// The file listing.
	typedef xarray<xstring> t_FileScanResult;
}

//##############################################################################
namespace Xen
{
	class CFile
	{
		// Friends.
		friend class CFileManager;

	public:
		// Get the file name.
		inline const xchar* GetName()
		{
			return m_pFile;
		}

		// Get the current flags.
		inline xbool IsFlagSet(xuint iFlag)
		{
			return XFLAGISSET(m_iFlags, iFlag);
		}

		// Read the file contents into the specified buffer.
		// ~iBytes The number of bytes to read in or -1 to read in the entire buffer contents.
		// ~iBytesRead The number of bytes that were read during this operation. Specify NULL to ignore.
		virtual t_FileError Read(void* pBuffer, xint iBytes = -1, xint* pBytesRead = NULL) = 0;

		// Write the specified number of bytes to the file.
		// ~iBytesWritten The number of bytes that were written during this operation. Specify NULL to ignore.
		virtual t_FileError Write(const void* pBuffer, xint iBytes, xint* pBytesWritten = NULL) = 0;

		// Flush the contents of the buffer to the file immediately.
		virtual t_FileError Flush() = 0;

		// Set the current read/write pointer offset.
		virtual t_FileError SetOffset(xint iOffset) = 0;

		// Get the current read/write pointer offset.
		virtual t_FileError GetOffset(xint* pOffset) = 0;

		// Get the current file size on disk.
		// ~return The size of the opened file or -1 otherwise.
		virtual xint GetSize() = 0;

		// Determine if the file is open/valid.
		virtual xbool IsOpen() = 0;

	protected:
		// Constructor.
		// Protected to prevent 'new' on this class.
		CFile();

		// Destructor.
		// Protected to prevent 'delete' on this class.
		virtual ~CFile();

		// Create a new, blank file and open it.
		// ~pFile The path to the file to create. Directories will *not* be created.
		// ~iFlags The flags to use with this operation.
		virtual t_FileError Create(const xchar* pFile, xuint iFlags = 0) = 0;

		// Open an existing file.
		// ~pFile The path to the file to open.
		// ~iFlags The flags to use with this operation.
		virtual t_FileError Open(const xchar* pFile, xuint iFlags = 0) = 0;

		// Close the file handle.
		virtual t_FileError Close();

		// Set the flags safely.
		void SetFlags(xuint iFlags);

		// The file path.
		const xchar* m_pFile;

	private:
		// The file access flags.
		xuint m_iFlags;
	};
}

//##############################################################################

#if XWINDOWS
//##############################################################################
namespace Xen
{
	class CWinFile : public CFile
	{
		// Friends.
		friend class CFileManager;

	public:
		// Read the file contents into the specified buffer.
		// ~iBytes The number of bytes to read in or -1 to read in the entire buffer contents.
		// ~iBytesRead The number of bytes that were read during this operation. Specify NULL to ignore.
		virtual t_FileError Read(void* pBuffer, xint iBytes = -1, xint* pBytesRead = NULL);

		// Write the specified number of bytes to the file.
		// ~iBytesWritten The number of bytes that were written during this operation. Specify NULL to ignore.
		virtual t_FileError Write(const void* pBuffer, xint iBytes, xint* pBytesWritten = NULL);

		// Flush the contents of the buffer to the file immediately.
		virtual t_FileError Flush();

		// Set the current read/write pointer offset.
		virtual t_FileError SetOffset(xint iOffset);

		// Get the current read/write pointer offset.
		virtual t_FileError GetOffset(xint* pOffset);

		// Get the current file size on disk.
		// ~return The size of the opened file or -1 otherwise.
		virtual xint GetSize();

		// Determine if the file is open/valid.
		virtual xbool IsOpen()
		{
			return (m_hFile != INVALID_HANDLE_VALUE);
		}

		// Get the handle to the file directly.
		// ~note This is for advanced use only.
		inline HANDLE GetHandle()
		{
			return m_hFile;
		}

	protected:
		// Constructor.
		// Protected to prevent 'new' on this class.
		CWinFile();

		// Destructor.
		// Protected to prevent 'delete' on this class.
		virtual ~CWinFile();

		// Create a new, blank file and open it.
		// ~pFile The path to the file to create. Directories will *not* be created.
		// ~iFlags The flags to use with this operation.
		virtual t_FileError Create(const xchar* pFile, xuint iFlags = 0);

		// Open an existing file.
		// ~pFile The path to the file to open.
		// ~iFlags The flags to use with this operation.
		virtual t_FileError Open(const xchar* pFile, xuint iFlags = 0);

		// Close the file handle.
		virtual t_FileError Close();

		// The windows file handle.
		HANDLE m_hFile;
	};
}

//##############################################################################
#endif

//##############################################################################
namespace Xen
{
	class CFileManager : public CModule
	{
	public:
		// Singleton instance.
		static inline CFileManager& Get()
		{
			static CFileManager s_xInstance;
			return s_xInstance;
		}

#if XWINDOWS
		// Create a new, blank file and open it.
		// ~pFileName The path to the file to create. Directories will *not* be created.
		// ~iFlags The flags to use with this operation.
		CWinFile* Create(const xchar* pFileName, xuint iFlags = 0);

		// Open an existing file.
		// ~pFileName The path to the file to open.
		// ~iFlags The flags to use with this operation.
		CWinFile* Open(const xchar* pFileName, xuint iFlags = 0);

		// Scan a directory for files.
		// ~pPath The path to scan for files. This can also include a file mask.
		t_FileScanResult Scan(const xchar* pPath);
#endif

		// Close an open file and destroy it.
		void Close(CFile* pFile);

		// Get the last error code returned.
		inline t_FileError GetLastError()
		{
			return m_iLastError;
		}

	protected:
		// Types.
		typedef xlist<CFile*> t_FileList;

		// Deinitialise the file manager.
		virtual void OnDeinitialise();

		// The managed list of files.
		t_FileList m_lpFiles;
		
		// The error code from the last executed operation.
		t_FileError m_iLastError;
	};
}

//##############################################################################