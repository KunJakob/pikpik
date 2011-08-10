//#########################################################################
// 
//	*******************************************************************
//	File: String
//	*******************************************************************
//	Part of the Xen Engine.
// 
//	Author: Nat Ryall
//	Creation Date: Friday, 9 February 2007 (08:16 PM [GMT +10])
//	-------------------------------------------------------------------
//	Copyright © 1998 - 2007, SAPIAN
// 
//#########################################################################

#ifndef __String_h__
#define __String_h__

//#########################################################################

///////////////////////////////////////////////////////////////////////////
// 
//	COMMON
// 
///////////////////////////////////////////////////////////////////////////
#include <Xen/Common.h>

///////////////////////////////////////////////////////////////////////////
// 
//	MACROS
// 
///////////////////////////////////////////////////////////////////////////
#define XEN_FORMAT \
		Xen::String::Format

#define XEN_WIDE_FORMAT \
		Xen::String::Format

///////////////////////////////////////////////////////////////////////////
// 
//	DECLARATION
// 
///////////////////////////////////////////////////////////////////////////
namespace Xen
{
	namespace String
	{
		// Format a string by including the specified params.
		const xchar* Format(const xchar* pString, ...);

		// Format a wide string by including the specified params.
		const xwchar* Format(const xwchar* pString, ...);

		// Format a string and place it in a specified buffer.
		const xchar* Format(xchar* pBuffer, xint iBufferSize, const xchar* pString, ...);

		// Format a wide string and place it in a specified buffer.
		const xwchar* Format(xwchar* pBuffer, xint iBufferSize, const xwchar* pString, ...);

		// Determine if a character is whitespace.
		inline xbool IsWhitespace(xchar cChar)
		{
			return (cChar == ' ' || cChar == '\n' || cChar == '\r' || cChar == '\t' || cChar == '\f');
		}

		// Determine if a character is a lowercase letter.
		inline xbool IsLowerLetter(xchar cChar)
		{
			return (cChar >= 'a' && cChar <= 'z');
		}

		// Determine if a character is an uppercase letter.
		inline xbool IsUpperLetter(xchar cChar)
		{
			return (cChar >= 'A' && cChar <= 'Z');
		}

		// Determine if a character is an upper or lowercase letter.
		inline xbool IsLetter(xchar cChar)
		{
			return (IsLowerLetter(cChar) || IsUpperLetter(cChar));
		}

		// Determine if a character is a number.
		inline xbool IsNumber(xchar cChar)
		{
			return (cChar >= '0' && cChar <= '9');
		}

		// Determine if a character is alpha-numeric (letter or a number).
		inline xbool IsAlphaNumeric(xchar cChar)
		{
			return (IsLetter(cChar) || IsNumber(cChar));
		}

		// Compare two strings against one another (case sensitive).
		// ~iLen The number of characters to compare; if this is -1, all characters in the string are compared.
		// ~return 0 if the strings match, < 0 if pA is lower than pB, > 0 if pB is lower than pA.
		inline xint Compare(const xchar* pA, const xchar* pB, xint iLen = -1)
		{
			return (iLen == -1) ? strcmp(pA, pB) : strncmp(pA, pB, iLen);
		}

		// Compare two wide strings against one another (case sensitive).
		// ~iLen The number of characters to compare; if this is -1, all characters in the string are compared.
		// ~return 0 if the strings match, < 0 if pA is lower than pB, > 0 if pB is lower than pA.
		inline xint Compare(const xwchar* pA, const xwchar* pB, xint iLen = -1)
		{
			return (iLen == -1) ? wcscmp(pA, pB) : wcsncmp(pA, pB, iLen);
		}

		// Compare two strings against one another (case insensitive).
		// ~iLen The number of characters to compare; if this is -1, all characters in the string are compared.
		// ~return 0 if the strings match, < 0 if pA is lower than pB, > 0 if pB is lower than pA.
		inline xint CaselessCompare(const xchar* pA, const xchar* pB, xint iLen = -1)
		{
			return (iLen == -1) ? _stricmp(pA, pB) : _strnicmp(pA, pB, iLen);
		}

		// Compare two wide strings against one another (case insensitive).
		// ~iLen The number of characters to compare; if this is -1, all characters in the string are compared.
		// ~return 0 if the strings match, < 0 if pA is lower than pB, > 0 if pB is lower than pA.
		inline xint CaselessCompare(const xwchar* pA, const xwchar* pB, xint iLen = -1)
		{
			return (iLen == -1) ? _wcsicmp(pA, pB) : _wcsnicmp(pA, pB, iLen);
		}

		// Check if two strings are identical (case sensitive).
		// ~iLen The number of characters to compare; if this is -1, all characters in the string are compared.
		inline xbool IsMatch(const xchar* pA, const xchar* pB, xint iLen = -1)
		{
			return Compare(pA, pB, iLen) == 0;
		}

		// Check if two wide strings are identical (case sensitive).
		// ~iLen The number of characters to compare; if this is -1, all characters in the string are compared.
		inline xbool IsMatch(const xwchar* pA, const xwchar* pB, xint iLen = -1)
		{
			return Compare(pA, pB, iLen) == 0;
		}

		// Check if two strings are the same (case insensitive).
		// ~iLen The number of characters to compare; if this is -1, all characters in the string are compared.
		inline xbool IsCaselessMatch(const xchar* pA, const xchar* pB, xint iLen = -1)
		{
			return CaselessCompare(pA, pB, iLen) == 0;
		}

		// Check if two wide strings are the same (case insensitive).
		// ~iLen The number of characters to compare; if this is -1, all characters in the string are compared.
		inline xbool IsCaselessMatch(const xwchar* pA, const xwchar* pB, xint iLen = -1)
		{
			return CaselessCompare(pA, pB, iLen) == 0;
		}

		// Get the length of a string not including the NULL terminator.
		inline xint Length(const xchar* pString)
		{
			return (xint)strlen(pString);
		}

		// Get the length of a wide string not including the NULL terminator.
		inline xint Length(const xwchar* pString)
		{
			return (xint)wcslen(pString);
		}

		// Copy a string into a buffer including the NULL terminator.
		inline void Copy(const xchar* pFrom, xchar* pTo, xint iToLen)
		{
			strcpy_s(pTo, iToLen, pFrom);
		}

		// Copy a wide string into a buffer including the NULL terminator.
		inline void Copy(const xwchar* pFrom, xwchar* pTo, xint iToLen)
		{
			wcscpy_s(pTo, iToLen, pFrom);
		}

		// Convert a lower case char to an upper case char.
		inline xchar ToUpperLetter(const xchar cChar)
		{
			return (!IsLowerLetter(cChar)) ? cChar : cChar + 32;
		}

		// Convert an upper case char to a lower case char.
		inline xchar ToLowerLetter(const xchar cChar)
		{
			return (!IsUpperLetter(cChar)) ? cChar : cChar - 32;
		}

		// Convert a string to an integer.
		inline xint ToInt(const xchar* pString)
		{
			return atoi(pString);
		}

		// Convert a wide string to an integer.
		inline xint ToInt(const xwchar* pString)
		{
			return _wtoi(pString);
		}

		// Convert a string to a floating point value.
		inline xfloat ToFloat(const xchar* pString)
		{
			return (xfloat)atof(pString);
		}

		// Convert a wide string to a floating point value.
		inline xfloat ToFloat(const xwchar* pString)
		{
			return (xfloat)_wtof(pString);
		}

		// Find a specific character in a string.
		// ~iLen The number of characters to search; if this is -1, all characters in the string are compared.
		// ~return The character memory location or NULL if not found.
		inline const xchar* Find(xchar cFind, const xchar* pIn, xint iLen = -1)
		{
			return (iLen == -1) ? strchr(pIn, cFind) : (xchar*)memchr(pIn, cFind, iLen);
		}

		// Find a specific character in a wide string.
		// ~iLen The number of characters to search; if this is -1, all characters in the string are compared.
		// ~return The character memory location or NULL if not found.
		inline const xwchar* Find(xwchar cFind, const xwchar* pIn, xint iLen = -1)
		{
			return (iLen == -1) ? wcschr(pIn, cFind) : wmemchr(pIn, cFind, iLen);
		}

		// Find a specific substring in a string.
		// ~return The string memory location or NULL if not found.
		inline const xchar* Find(const xchar* pFind, const xchar* pIn)
		{
			return strstr(pIn, pFind);
		}

		// Find a specific substring in a wide string.
		// ~return The string memory location or NULL if not found.
		inline const xwchar* Find(const xwchar* pFind, const xwchar* pIn)
		{
			return wcsstr(pIn, pFind);
		}
	}
}

//#########################################################################

#endif // __String_h__
