#pragma once

//#########################################################################
// 
//	*******************************************************************
//	File: Global Macros
//	*******************************************************************
//	Part of the Xen Engine.
// 
//	Author: Nat Ryall
//	Creation Date: Thursday, 8 February 2007 (10:39 PM [GMT +10])
//	-------------------------------------------------------------------
//	Copyright © 1998 - 2007, SAPIAN
// 
//#########################################################################

//##############################################################################

// Build.
#if defined(_DEBUG)
	#define XEN_DEBUG		1
	#define XEN_RELEASE		0
	#define XEN_RETAIL		0
#elif defined (_RELEASE)
	#define XEN_DEBUG		0
	#define XEN_RELEASE		1
	#define XEN_RETAIL		0
#else
	#define XEN_DEBUG		0
	#define XEN_RELEASE		0
	#define XEN_RETAIL		1
#endif

// Platform.
#if defined(WIN32)
	#define XEN_WINDOWS		1
#else
	#define XEN_WINDOWS		0
#endif

// Debugging.
#define XEN_BREAKPOINT \
		{ __asm int 3 }

// Helpers.
#define XEN_SAFE_MACRO(MACRO) \
		do { MACRO; } while (0)

#define XEN_IN
#define XEN_OUT
#define XEN_IN_OUT

// Template.
#define XEN_SINGLETON(CLASSNAME) \
		CLASSNAME : Xen::Templates::CSingletonT<CLASSNAME>

// Pointer.
#define XEN_NULL \
		((void*)0)

// Integer.
#define XEN_INTEGER_PERCENTAGE(PERCENT, PERCENTMAX, VALUEMAX) \
		(((PERCENT) * (VALUEMAX)) / (PERCENTMAX))

#define XEN_KB(VALUE) \
		((VALUE) * 1024)

#define XEN_MB(VALUE) \
		((VALUE) * 1048576)

#define XEN_GB(VALUE) \
		((VALUE) * 1073741824)

#define XEN_SECONDS(SECONDS) \
		((SECONDS) * 1000)

#define XEN_MINUTES(MINS) \
		((MINS) * 60000)

#define XEN_HOURS(HOURS) \
		((HOURS) * 3600000)

// String.
#define _XEN_UNICODE(STR) \
		L##STR

#define XEN_UNICODE(STR) \
		_XEN_UNICODE(STR)

#define XEN_ENDL \
		"\r\n"

#define XEN_WENDL \
		L"\r\n"

// Errors.
#define XEN_SUCCESS(CODE) \
		((CODE) == 0)

#define XEN_FAIL(CODE) \
		((CODE) != 0)

// Bits.
#define XEN_BIT(INDEX) \
		(1 << (INDEX))

#define XEN_LEFT_SHIFT(NUM, SHIFT) \
		((NUM) << (SHIFT))

#define XEN_RIGHT_SHIFT(NUM, SHIFT) \
		((NUM) >> (SHIFT))

// Flags.
#define XEN_FLAG_SET(VAR, FLAG) \
		((VAR) |= (FLAG))

#define XEN_FLAG_CLEAR(VAR, FLAG) \
		((VAR) &= ~(FLAG))

#define XEN_FLAG_ISSET(VAR, FLAG) \
		(((VAR) & (FLAG)) == (FLAG))

// Lists.
#define XEN_LIST_FOREACH(TYPE, ITER, LIST) \
		for (TYPE::iterator ITER = LIST.begin(); ITER != LIST.end(); ITER++)

#define XEN_LIST_FOREACH_R(TYPE, ITER, LIST) \
		for (TYPE::reverse_iterator ITER = LIST.rbegin(); ITER != LIST.rend(); ITER++)

#define XEN_LIST_REMOVE(TYPE, LIST, VALUE) \
		XEN_LIST_FOREACH(TYPE, ppIter, LIST) { if (*ppIter == VALUE) { LIST.erase(ppIter); break; } }

#define XEN_LIST_REMOVE_ALL(LIST) \
		LIST.clear()

#define XEN_LIST_ERASE(TYPE, LIST, VALUE) \
		XEN_LIST_FOREACH(TYPE, ppIter, LIST) { if (*ppIter == VALUE) { delete *ppIter; LIST.erase(ppIter); break; } }

#define XEN_LIST_ERASE_ALL(LIST) \
		while (LIST.size()) { delete *LIST.rbegin(); LIST.pop_back(); }

//##############################################################################