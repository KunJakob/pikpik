//#########################################################################
// 
//	*******************************************************************
//	File: Log
//	*******************************************************************
//	Part of the Xen Engine.
// 
//	Author: Nat Ryall
//	Creation Date: Sunday, 9 July 2006 (03:09 PM [GMT +1])
//	Refactor Date: Thursday, 8 February 2007 (11:03 PM [GMT +10])
//	-------------------------------------------------------------------
//	Copyright © 1998 - 2007, SAPIAN
// 
//#########################################################################

#ifndef __Log_h__
#define __Log_h__

//#########################################################################

///////////////////////////////////////////////////////////////////////////
// 
//	COMMON
// 
///////////////////////////////////////////////////////////////////////////
#include <Xen/Common.h>

///////////////////////////////////////////////////////////////////////////
// 
//	INCLUDE
// 
///////////////////////////////////////////////////////////////////////////
#include <Windows.h>
#include <Xen/Exception.h>

///////////////////////////////////////////////////////////////////////////
//
//	MACROS
//
///////////////////////////////////////////////////////////////////////////
#define XEN_EVENT(MESSAGE, ...) \
		Xen::Log::Event(Xen::Log::XLT_Event, MESSAGE, __VA_ARGS__)

#define XEN_WARNING(MESSAGE, ...) \
		Xen::Log::Event(Xen::Log::XLT_Warning, MESSAGE, __VA_ARGS__)

#define XEN_ERROR(MESSAGE, ...) \
		Xen::Log::Event(Xen::Log::XLT_Error, MESSAGE, __VA_ARGS__)

#define XEN_FATAL(MESSAGE, ...) \
		Xen::Log::Event(Xen::Log::XLT_Fatal, MESSAGE, __VA_ARGS__)

#define XEN_LOG(MESSAGE, ...) \
		XEN_EVENT(MESSAGE, __VA_ARGS__)

#define XEN_LOG_CHAR(VALUE) \
		XEN_EVENT(XFORMAT(L"%s = %c", XUNICODE(#VALUE), VALUE))

#define XEN_LOG_INT(VALUE) \
		XEN_EVENT(XFORMAT(L"%s = %d", XUNICODE(#VALUE), VALUE))

#define XEN_LOG_FLOAT(VALUE) \
		XEN_EVENT(XFORMAT(L"%s = %f", XUNICODE(#VALUE), VALUE))

#define XEN_LOG_STRING(VALUE) \
		XEN_EVENT(XFORMAT(L"%s = %s", XUNICODE(#VALUE), VALUE))

///////////////////////////////////////////////////////////////////////////
// 
//	TYPES
// 
///////////////////////////////////////////////////////////////////////////
namespace Xen
{
	namespace Log
	{
		// Error types.
		enum Error
		{
			XE_Success,
			XE_AlreadyCreated,
			XE_CreateFailed,
			XE_NotOpen,
			XE_WriteFailed,
		};

		// Log types.
		enum LogType
		{
			XLT_Event,
			XLT_Warning,
			XLT_Error,
			XLT_Fatal,
		};
	}
}

///////////////////////////////////////////////////////////////////////////
// 
//	DECLARATION
// 
///////////////////////////////////////////////////////////////////////////
namespace Xen
{
	namespace Log
	{
		// Create a file to output the log to.
		Error CreateFile(xwstring sFileName);

		// Close the file being used to output the log.
		Error CloseFile();

		// Log an event. (Wide)
		Error Event(LogType eType, const xchar* psEvent, ...);

		// Log an event. (Wide)
		Error Event(LogType eType, const xwchar* psEvent, ...);
	}
}

//#########################################################################

#endif // __Log_h__
