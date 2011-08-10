//##############################################################################
// 
//	**************************************************************************
//	File: Timer
//	**************************************************************************
//	Part of the Xen Engine.
// 
//	Author: Nat Ryall
//	--------------------------------------------------------------------------
//	Copyright © 1998 - 2008, SAPIAN
// 
//##############################################################################

#ifndef __XEN__Timer_h__
#define __XEN__Timer_h__

//##############################################################################

// Common
#include <Xen/Common.h>

//##############################################################################
namespace Xen
{
	class CTimer
	{
	public:
		/**
		* Constructor: Initialise the timer.
		*/
		CTimer() : m_iTime(0) {}

		/**
		* Check if the timer has expired.
		*/
		inline xbool IsExpired()
		{
			return (m_iTime == 0 || GetTickCount() >= m_iTime);
		}

		/**
		* Set the timer to expire after a certain number of milliseconds.
		*/
		inline void ExpireAfter(xuint iMillisecs)
		{
			m_iTime = GetTickCount() + iMillisecs;
		}

		/**
		* Get the expiration time.
		*/
		inline xuint ExpirationTime()
		{
			return m_iTime;
		}

		/**
		* Get the time in milliseconds left before the timer expires.
		*/
		inline xuint TimeToExpiration()
		{
			return m_iTime ? m_iTime - GetTickCount() : 0;
		}

		/**
		* Reset the timer.
		*/
		inline void Reset()
		{
			m_iTime = 0;
		}

	private:
		xuint m_iTime;
	};
}

//##############################################################################

#endif // __XEN__Timer_h__
