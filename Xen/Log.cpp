#include <Global.h>

//#########################################################################
// 
//	Copyright © 1998 - 2007, SAPIAN
//	-------------------------------------------------------------------
//	Please see the corresponding header for further details.
// 
//#########################################################################

#pragma warning(push)
#pragma warning(disable: 4996)

//#########################################################################

///////////////////////////////////////////////////////////////////////////
// 
//	COMMON
// 
///////////////////////////////////////////////////////////////////////////
#include <Xen/Common.h>

///////////////////////////////////////////////////////////////////////////
// 
//	LOCAL
// 
///////////////////////////////////////////////////////////////////////////
#include <Xen/Log.h>

///////////////////////////////////////////////////////////////////////////
// 
//	HEADER
// 
///////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdarg.h>
#include <String.h>

///////////////////////////////////////////////////////////////////////////
// 
//	MACROS
// 
///////////////////////////////////////////////////////////////////////////
#define XEN_FORMAT_BUFFER_LENGTH \
		512

///////////////////////////////////////////////////////////////////////////
// 
//	LOCALS
// 
///////////////////////////////////////////////////////////////////////////
static HANDLE s_hFile = INVALID_HANDLE_VALUE;
static Xen::xchar s_cFormatBuffer[XEN_FORMAT_BUFFER_LENGTH];
static Xen::xwchar s_wcFormatBuffer[XEN_FORMAT_BUFFER_LENGTH];

///////////////////////////////////////////////////////////////////////////
// 
//	INITIALISE
// 
///////////////////////////////////////////////////////////////////////////
Xen::Log::Error Xen::Log::CreateFile(xwstring sFile)
{
	if (s_hFile != INVALID_HANDLE_VALUE)
		return XE_AlreadyCreated;

	s_hFile = ::CreateFileW(sFile.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (s_hFile == INVALID_HANDLE_VALUE)
		return XE_CreateFailed;

	return XE_Success;
}

///////////////////////////////////////////////////////////////////////////
// 
//	SHUTDOWN
// 
///////////////////////////////////////////////////////////////////////////
Xen::Log::Error Xen::Log::CloseFile()
{
	if (s_hFile == INVALID_HANDLE_VALUE)
		return XE_NotOpen;

	CloseHandle(s_hFile);
	s_hFile = INVALID_HANDLE_VALUE;

	return XE_Success;
}

///////////////////////////////////////////////////////////////////////////
// 
//	EVENT
// 
///////////////////////////////////////////////////////////////////////////
Xen::Log::Error Xen::Log::Event(LogType eType, const xchar* pFormat, ...)
{
	va_list pArgumentList;

	va_start(pArgumentList, pFormat);
	xint iLength = _vsnprintf(s_cFormatBuffer, XEN_FORMAT_BUFFER_LENGTH / sizeof(Xen::xchar), pFormat, pArgumentList);
	va_end(pArgumentList);

	s_cFormatBuffer[iLength] = '\0';

	OutputDebugStringA(s_cFormatBuffer);
	OutputDebugStringA(XENDL);

	if (s_hFile != INVALID_HANDLE_VALUE)
	{
		DWORD iBytesWritten = 0;

		if (!WriteFile(s_hFile, s_cFormatBuffer, iLength * sizeof(xchar), &iBytesWritten, NULL))
			return XE_WriteFailed;

		if (!WriteFile(s_hFile, XWENDL, (DWORD)String::Length(XENDL) * sizeof(xchar), &iBytesWritten, NULL))
			return XE_WriteFailed;
	}

	return XE_Success;
}

///////////////////////////////////////////////////////////////////////////
// 
//	EVENT
// 
///////////////////////////////////////////////////////////////////////////
Xen::Log::Error Xen::Log::Event(LogType eType, const xwchar* pFormat, ...)
{
	va_list pArgumentList;

	va_start(pArgumentList, pFormat);
	xint iLength = _vsnwprintf(s_wcFormatBuffer, XEN_FORMAT_BUFFER_LENGTH / sizeof(Xen::xwchar), pFormat, pArgumentList);
	va_end(pArgumentList);

	s_wcFormatBuffer[iLength] = L'\0';

	OutputDebugStringW(s_wcFormatBuffer);
	OutputDebugStringW(XWENDL);

	if (s_hFile != INVALID_HANDLE_VALUE)
	{
		DWORD iBytesWritten = 0;

		if (!WriteFile(s_hFile, s_wcFormatBuffer, iLength * sizeof(xwchar), &iBytesWritten, NULL))
			return XE_WriteFailed;

		if (!WriteFile(s_hFile, XWENDL, (DWORD)String::Length(XWENDL) * sizeof(xwchar), &iBytesWritten, NULL))
			return XE_WriteFailed;
	}

	return XE_Success;
}

//#########################################################################

#pragma warning(pop)
