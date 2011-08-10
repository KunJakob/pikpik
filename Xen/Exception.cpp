//#########################################################################
// 
//	Copyright © 1998 - 2007, SAPIAN
//	-------------------------------------------------------------------
//	Please see the corresponding header for further details.
// 
//#########################################################################

//##############################################################################

// Global.
#include <Global.h>

// Common.
#include <Xen/Common.h>

// Local.
#include <Xen/Exception.h>

// Other.
#include <Xen/String.h>

//##############################################################################

// =============================================================================
xbool Xen::CException::HandleError(xstring sMessage, xstring sCondition, xstring sFunction, xstring sFile, xint iLine)
{
	xstring sExceptionMessage;

#if !XRETAIL
	sExceptionMessage = XFORMAT
	(
		"Failed Condition: %s" XENDL "Function: %s" XENDL "Line: %d" XENDL XENDL "%s" XENDL XENDL "Would you like to debug?", 
		sCondition.c_str(), sFunction.c_str(), iLine, sMessage.c_str()
	);
#else
	sExceptionMessage = sMessage;
#endif

XLOG(sExceptionMessage.c_str());

#if XWINDOWS
	#if !XRETAIL
		xint iDialogResult = MessageBox(NULL, sExceptionMessage.c_str(), "Application Error", MB_YESNO | MB_ICONERROR | MB_APPLMODAL | MB_SETFOREGROUND | MB_DEFBUTTON1);

		if (iDialogResult == IDYES)
			return true;
	#endif
#endif
	
	throw CException(sExceptionMessage);
	return false;
}

//##############################################################################