#pragma once

//##############################################################################
// 
//	Circle.h
//	--------------------------------------------------------------------------
//  Brief:		Data type representing a point with a radius.
//	Project:	Xen
//	Author:		Nat Ryall
//	Date:		28/07/2008
//	License:	Private
//	--------------------------------------------------------------------------
//	Copyright (c) SAPIAN, 2008
// 
//##############################################################################

// Common.
#include <Xen/Common.h>
#include <Xen/Point.h>

//##############################################################################
namespace Xen
{
	namespace Math
	{
		template<typename t_Type>
		class CCircleT : public CPointT<t_Type>
		{
		public:
			// Constructor. (Default)
			CCircleT() : m_tRadius(0) {}

			// Constructor. (Radius)
			CCircleT(t_Type tRadius) : m_tRadius(tRadius) {}

			// Constructor. (X, Y, Radius)
			CCircleT(t_Type tX, t_Type tY, t_Type tRadius) : CPointT<t_Type>(tX, tY), m_tRadius(tRadius) {}

			// Constructor. (CPoint<t_Type>, Radius)
			CCircleT(CPointT<t_Type> tPoint, t_Type tRadius) : CPointT<t_Type>(tPoint), m_tRadius(tRadius) {}

			// Reset to zero.
			inline void Reset()
			{
				m_tX = m_tY = m_tRadius = 0;
			}

			// Check if all values are set to zero.
			inline xbool IsZero()
			{
				return m_tX == 0 && m_tY == 0 && m_tRadius == 0;
			}

			// Set the point values in one call.
			inline void Set(t_Type tX, t_Type tY, t_Type tRadius)
			{
				m_tX = tX;
				m_tY = tY;
				m_tRadius = tRadius;
			}

			// The point values.
			t_Type m_tRadius;
		};
	}
}

//##############################################################################