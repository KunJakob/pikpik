#pragma once

//##############################################################################
// 
//	Point.h
//	--------------------------------------------------------------------------
//  Brief:		Data type representing the coordinates of a point.
//	Project:	Xen
//	Author:		Nat Ryall
//	Date:		07/12/2007
//	License:	Private
//	--------------------------------------------------------------------------
//	Copyright (c) SAPIAN, 2007
// 
//##############################################################################

// Common.
#include <Xen/Common.h>

//##############################################################################
namespace Xen
{
	namespace Math
	{
		template<typename t_Type>
		class CPointT
		{
		public:
			// Constructor. (Default)
			CPointT() : m_tX(0), m_tY(0) {}

			// Constructor. (X, Y)
			CPointT(t_Type tX, t_Type tY) : m_tX(tX), m_tY(tY) {}

			// Reset to zero.
			inline void Reset()
			{
				m_tX = m_tY = 0;
			}

			// Check if all values are set to zero.
			inline xbool IsZero()
			{
				return m_tX == 0 && m_tY == 0;
			}

			// Set the point values in one call.
			inline void Set(t_Type tX, t_Type tY)
			{
				m_tX = tX;
				m_tY = tY;
			}

			// Add another point to this point.
			inline CPointT<t_Type>& operator+=(CPointT<t_Type>& tPoint)
			{
				m_tX += tPoint.m_tX;
				m_tY += tPoint.m_tY;

				return *this;
			}

			// Subtract another point from this point.
			inline CPointT<t_Type>& operator-=(CPointT<t_Type>& tPoint)
			{
				m_tX -= tPoint.m_tX;
				m_tY -= tPoint.m_tY;

				return *this;
			}

			// Multiply this point with another point.
			inline CPointT<t_Type>& operator*=(CPointT<t_Type>& tPoint)
			{
				m_tX *= tPoint.m_tX;
				m_tY *= tPoint.m_tY;

				return *this;
			}

			// Divide this point by another point.
			inline CPointT<t_Type>& operator/=(CPointT<t_Type>& tPoint)
			{
				m_tX /= tPoint.m_tX;
				m_tY /= tPoint.m_tY;

				return *this;
			}

			// Get the modulus for this point using another point.
			inline CPointT<t_Type>& operator%=(CPointT<t_Type>& tPoint)
			{
				m_tX %= tPoint.m_tX;
				m_tY %= tPoint.m_tY;

				return *this;
			}

			// Add a value to both axis of this point.
			inline CPointT<t_Type>& operator+=(t_Type tValue)
			{
				m_tX += tValue;
				m_tY += tValue;

				return *this;
			}

			// Subtract a value from both axis of this point.
			inline CPointT<t_Type>& operator-=(t_Type tValue)
			{
				m_tX -= tValue;
				m_tY -= tValue;

				return *this;
			}

			// Multiply both axis of this point with a value.
			inline CPointT<t_Type>& operator*=(t_Type tValue)
			{
				m_tX *= tValue;
				m_tY *= tValue;

				return *this;
			}

			// Divide both axis of this point by a value.
			inline CPointT<t_Type>& operator/=(t_Type tValue)
			{
				m_tX /= tValue;
				m_tY /= tValue;

				return *this;
			}

			// Get the modulus of both axis of this point using a value.
			inline CPointT<t_Type>& operator%=(t_Type tValue)
			{
				m_tX %= tValue;
				m_tY %= tValue;

				return *this;
			}

			// The point values.
			t_Type m_tX;
			t_Type m_tY;
		};
	}
}

//##############################################################################