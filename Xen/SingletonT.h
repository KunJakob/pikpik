//#########################################################################
//
//	*******************************************************************
//	File: Singleton Template
//	*******************************************************************
//	Part of the Xen Engine.
//
//	Author: Nat Ryall
//	Creation Date: Saturday, 27 August 2005 (11:32 AM [GMT +10])
//	Refactor Date: Thursday, 8 February 2007 (10:18 PM [GMT +10])
//	-------------------------------------------------------------------
//	Copyright © 1998 - 2007, SAPIAN
//
//#########################################################################

#ifndef __Singleton_T_h__
#define __Singleton_T_h__

//#########################################################################

///////////////////////////////////////////////////////////////////////////
//
//	CLASS DECLARATION
//
///////////////////////////////////////////////////////////////////////////
namespace Xen
{
	namespace Templates
	{
		template <typename t_Class>
		class CSingletonT 
		{
		public:
			// Returns a globally accessible, static instance of the class.
			static inline t_Class& Get() 
			{
				static t_Class s_Instance;
				return s_Instance;
			}
		};
	}
}

//#########################################################################

#endif // __Singleton_T_h__
