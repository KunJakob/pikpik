#pragma once

//##############################################################################
// 
//	Memory.h
//	--------------------------------------------------------------------------
//  Brief:		Memory management and manipulation routines.
//	Project:	Xen
//	Author:		Nat Ryall
//	Date:		19/11/2008
//	License:	Private
//	--------------------------------------------------------------------------
//	Copyright (c) SAPIAN, 2008
// 
//##############################################################################

// Common.
#include <Xen/Common.h>

//##############################################################################
namespace Xen
{
	namespace Memory
	{
		// Set a block of memory to a specified value.
		inline void Set(void* pMemory, xint iMemorySize, xint iValue)
		{
			memset(pMemory, iValue, iMemorySize);
		}

		// Set a block of memory to a specified value.
		template <typename t_Object>
		inline void Set(t_Object& xObject, xint iValue)
		{
			Set(&xObject, sizeof(xObject), iValue);
		}

		// Copy memory from one location to another.
		inline void Copy(const void* pFrom, void* pTo, xint iBytes, xint iToSize)
		{
			memcpy_s(pTo, iToSize, pFrom, iBytes);
		}

		// Copy memory from one location to another.
		inline void Copy(const void* pFrom, void* pTo, xint iBytes)
		{
			memcpy_s(pTo, iBytes, pFrom, iBytes);
		}

		// Copy structure from one location to another.
		template <typename t_Object>
		inline void Copy(const void* pFrom, const t_Object& xTo)
		{
			Copy(pFrom, (void*)&xTo, sizeof(xTo), sizeof(xTo));
		}

		// Set all bytes in memory to zero.
		inline void Zero(void* pMemory, xint iMemorySize)
		{
			Set(pMemory, iMemorySize, 0);
		}

		// Set all bytes in a structure to zero.
		template <typename t_Object>
		inline void Zero(t_Object& xObject)
		{
			Zero(&xObject, sizeof(xObject));
		}

		// Compare memory from two locations.
		// ~return 0 if the memory matches, < 0 if pA is lower than pB, > 0 if pB is lower than pA.
		inline xint Compare(const void* pA, const void* pB, xint iBytes)
		{
			return memcmp(pA, pB, iBytes);
		}

		// Determine if two memory blocks match.
		inline xbool IsMatch(const void* pA, const void* pB, xint iBytes)
		{
			return Compare(pA, pB, iBytes) == 0;
		}
	}
}

//##############################################################################