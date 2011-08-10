//##############################################################################
// 
//	Copyright © 1998 - 2007, SAPIAN
//	-------------------------------------------------------------------
//	Please see the corresponding header for further details.
// 
//##############################################################################

#pragma warning(push)
#pragma warning(disable: 4996)

//##############################################################################

// Common.
#include <Xen/Common.h>

// Local.
#include <Xen/String.h>

// Other.
#include <stdio.h>
#include <stdarg.h>

//##############################################################################

// The number of temporary buffers allocated for format operations.
#define XEN_FORMAT_BUFFER_COUNT \
		32

// The max string length for format operations.
#define XEN_FORMAT_BUFFER_LENGTH \
		1024

//##############################################################################

// Format.
static Xen::xint s_iBufferIndex = XEN_FORMAT_BUFFER_COUNT;
static Xen::xchar s_cFormatBuffer[XEN_FORMAT_BUFFER_COUNT][XEN_FORMAT_BUFFER_LENGTH];

//##############################################################################

// =============================================================================
const Xen::xchar* Xen::String::Format(const xchar* pString, ...)
{
	if (++s_iBufferIndex >= XEN_FORMAT_BUFFER_COUNT)
		s_iBufferIndex = 0;

	va_list pArgumentList;

	va_start(pArgumentList, pString);
	xint iLength = _vsnprintf(s_cFormatBuffer[s_iBufferIndex], XEN_FORMAT_BUFFER_LENGTH / sizeof(Xen::xchar), pString, pArgumentList);
	va_end(pArgumentList);

	s_cFormatBuffer[s_iBufferIndex][iLength] = '\0';

	return (const xchar*)s_cFormatBuffer[s_iBufferIndex];
}

// =============================================================================
const Xen::xwchar* Xen::String::Format(const xwchar* pString, ...)
{
	if (++s_iBufferIndex >= XEN_FORMAT_BUFFER_COUNT)
		s_iBufferIndex = 0;

	xwchar* pBuffer = (xwchar*)s_cFormatBuffer[s_iBufferIndex];

	va_list pArgumentList;

	va_start(pArgumentList, pString);
	xint iLength = _vsnwprintf(pBuffer, XEN_FORMAT_BUFFER_LENGTH / sizeof(Xen::xwchar), pString, pArgumentList);
	va_end(pArgumentList);

	pBuffer[iLength] = L'\0';

	return (const xwchar*)s_cFormatBuffer[s_iBufferIndex];
}

// =============================================================================
const Xen::xchar* Xen::String::Format(xchar* pBuffer, xint iBufferSize, const xchar* pString, ...)
{
	va_list pArgumentList;

	va_start(pArgumentList, pString);
	int iLength = _vsnprintf(pBuffer, iBufferSize / sizeof(Xen::xchar), pString, pArgumentList);
	va_end(pArgumentList);

	pBuffer[iLength] = '\0';

	return (const xchar*)pBuffer;
}

// =============================================================================
const Xen::xwchar* Xen::String::Format(xwchar* pBuffer, xint iBufferSize, const xwchar* pString, ...)
{
	va_list pArgumentList;

	va_start(pArgumentList, pString);
	int iLength = _vsnwprintf(pBuffer, iBufferSize / sizeof(Xen::xwchar), pString, pArgumentList);
	va_end(pArgumentList);

	pBuffer[iLength] = L'\0';

	return (const xwchar*)pBuffer;
}

//##############################################################################

#pragma warning(pop)