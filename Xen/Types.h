//#########################################################################
// 
//	*******************************************************************
//	File: Xen Types
//	*******************************************************************
//	Part of the Xen Engine.
// 
//	Author: Nat Ryall
//	Creation Date: Thursday, 8 February 2007 (10:12 PM [GMT +10])
//	-------------------------------------------------------------------
//	Copyright © 1998 - 2007, SAPIAN
// 
//#########################################################################

#ifndef __XEN__Types_h__
#define __XEN__Types_h__

//#########################################################################

///////////////////////////////////////////////////////////////////////////
// 
//	INCLUDE
// 
///////////////////////////////////////////////////////////////////////////
#include <Xen/Macros.h>

#include <string>
#include <list>
#include <vector>
#include <map>

#include <Xen/External/FastDelegate.h>

///////////////////////////////////////////////////////////////////////////
// 
//	BASIC TYPES
// 
///////////////////////////////////////////////////////////////////////////
namespace Xen
{
	typedef char  						xchar;
	typedef wchar_t						xwchar;
	typedef unsigned char				xuchar;
	typedef unsigned char				xbyte;
	
	typedef int							xint;
	typedef unsigned int				xuint;
	typedef long						xlong;
	typedef unsigned long				xulong;

	typedef __int8						xint8;
	typedef __int16						xint16;
	typedef __int32						xint32;
	typedef __int64						xint64;

	typedef unsigned __int8				xuint8;
	typedef unsigned __int16			xuint16;
	typedef unsigned __int32			xuint32;
	typedef unsigned __int64			xuint64;

	typedef float						xfloat;
	typedef double						xdouble;

	typedef bool						xbool;

	typedef std::string					xstring;
	typedef std::wstring				xwstring;
}

///////////////////////////////////////////////////////////////////////////
// 
//	COMPLEX TYPES
// 
///////////////////////////////////////////////////////////////////////////
#define xlist							std::list
#define xarray							std::vector

#define xhash							std::map
#define xmhash							std::multimap				

#define xfunction(PARAMS)				fastdelegate::FastDelegate ## PARAMS
#define xbind							fastdelegate::MakeDelegate

//#########################################################################

#endif // __XEN__Types_h__
