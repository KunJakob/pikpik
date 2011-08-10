#pragma once

//#########################################################################
// 
//	*******************************************************************
//	File: Exception
//	*******************************************************************
//	Part of the Xen Engine.
// 
//	Author: Nat Ryall
//	Creation Date: Sunday, 9 July 2006 (03:11 PM [GMT +1])
//	Refactor Date: Friday, 9 February 2007 (05:47 PM [GMT +10])
//	-------------------------------------------------------------------
//	Copyright © 1998 - 2007, SAPIAN
// 
//#########################################################################

//##############################################################################

// Common.
#include <Xen/Common.h>

//##############################################################################
#if !XRETAIL
	#define XEN_EXCEPTION(MESSAGE) \
			XSAFEMACRO(if (Xen::CException::HandleError(MESSAGE, "Exception", __FUNCTION__, __FILE__, __LINE__)) XBREAK)

	#define XEN_ASSERT(CONDITION) \
			XSAFEMACRO(if (!(CONDITION)) if (Xen::CException::HandleError("", #CONDITION, __FUNCTION__, __FILE__, __LINE__)) XBREAK)

	#define XEN_MESSAGE_ASSERT(CONDITION, MESSAGE) \
			XSAFEMACRO(if (!(CONDITION)) if (Xen::CException::HandleError(MESSAGE, #CONDITION, __FUNCTION__, __FILE__, __LINE__)) XBREAK)
#else
	#define XEN_EXCEPTION(MESSAGE) \
			XSAFEMACRO(Xen::CException::HandleError(MESSAGE, "Exception", "", "", 0))

	#define XEN_ASSERT(CONDITION) \
			XSAFEMACRO(if (!(CONDITION)) Xen::CException::HandleError("The application has encountered an error.", #CONDITION, "", "", 0))

	#define XEN_MESSAGE_ASSERT(CONDITION, MESSAGE) \
			XSAFEMACRO(if (!(CONDITION)) Xen::CException::HandleError(MESSAGE, #CONDITION, "", "", 0))
#endif

//##############################################################################
namespace Xen
{
	class CException
	{
	public:
		// Handle an exception.
		static xbool HandleError(xstring sMessage, xstring sCondition, xstring sFunction, xstring sFile, xint iLine);

		// Get the exception message.
		inline const xchar* GetMessage()
		{
			return m_sMessage.c_str();
		}

	private:
		// Constructor: Formats and saves the exception message.
		CException(xstring& sMessage) : m_sMessage(sMessage) {}

		// The saved exception messages.
		xstring m_sMessage;
	};
}

//##############################################################################