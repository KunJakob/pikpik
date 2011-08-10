#pragma once

//##############################################################################
// 
//	Rect.h
//	--------------------------------------------------------------------------
//  Brief:		Data type representing four points of an aligned rect.
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

// Other.
#include <Xen/Point.h>

//##############################################################################
namespace Xen
{
	namespace Math
	{
		template<typename t_Type>
		class CRectT
		{
		public:
			// Constructor. (Default)
			CRectT() : m_tLeft(0), m_tTop(0), m_tRight(0), m_tBottom(0) {}

			// Constructor. (Left, Top)
			CRectT(t_Type tLeft, t_Type tTop) : m_tLeft(tLeft), m_tTop(tTop), m_tRight(0), m_tBottom(0) {}

			// Constructor. (Left, Top, Right, Bottom)
			CRectT(t_Type tLeft, t_Type tTop, t_Type tRight, t_Type tBottom) : m_tLeft(tLeft), m_tTop(tTop), m_tRight(tRight), m_tBottom(tBottom) {}

			// Constructor. (Top-Left, Right, Bottom)
			CRectT(CPointT<t_Type> tTopLeft, t_Type tRight, t_Type tBottom) : m_tLeft(tTopLeft.m_tX), m_tTop(tTopLeft.m_tY), m_tRight(tRight), m_tBottom(tBottom) {}

			// Constructor. (Left, Top, Bottom-Right)
			CRectT(t_Type tLeft, t_Type tTop, CPointT<t_Type> tBottomRight) : m_tLeft(tLeft), m_tTop(tTop), m_tRight(tBottomRight.m_tX), m_tBottom(tBottomRight.m_tY) {}

			// Constructor. (Top-Left, Bottom-Right)
			CRectT(CPointT<t_Type> tTopLeft, CPointT<t_Type> tBottomRight) : m_tLeft(tTopLeft.m_tX), m_tTop(tTopLeft.m_tY), m_tRight(tBottomRight.m_tX), m_tBottom(tBottomRight.m_tY) {}

			// Reset to zero.
			inline void Reset()
			{
				m_tLeft = m_tTop = m_tRight = m_tBottom = 0;
			}

			// Check if all values are set to zero.
			inline xbool IsZero()
			{
				return m_tLeft == 0 && m_tTop == 0 && m_tRight == 0 && m_tBottom == 0;
			}

			// Set the rect values in one call.
			inline void Set(t_Type tLeft, t_Type tTop, t_Type tRight, t_Type tBottom)
			{
				m_tLeft = tLeft;
				m_tTop = tTop;
				m_tRight = tRight;
				m_tBottom = tBottom;
			}

			// Get the width of the rect.
			inline t_Type GetWidth()
			{
				return (m_tLeft <= m_tRight) ? (m_tRight - m_tLeft) : (m_tLeft - m_tRight);
			}

			// Get the height of the rect.
			inline t_Type GetHeight()
			{
				return (m_tTop <= m_tBottom) ? (m_tBottom - m_tTop) : (m_tTop - m_tBottom);
			}

			// Get the top-left point of the rect.
			inline CPointT<t_Type> GetTopLeft()
			{
				return CPointT<t_Type>(m_tLeft, m_tTop);
			}

			// Get the top-right point of the rect.
			inline CPointT<t_Type> GetTopRight()
			{
				return CPointT<t_Type>(m_tRight, m_tTop);
			}

			// Get the bottom-left point of the rect.
			inline CPointT<t_Type> GetBottomLeft()
			{
				return CPointT<t_Type>(m_tLeft, m_tBottom);
			}

			// Get the bottom-right point of the rect.
			inline CPointT<t_Type> GetBottomRight()
			{
				return CPointT<t_Type>(m_tRight, m_tBottom);
			}

			// Add another rect to this rect.
			inline CRectT<t_Type>& operator+=(CRectT<t_Type>& tRect)
			{
				m_tLeft += tRect.m_tLeft;
				m_tTop += tRect.m_tTop;
				m_tRight += tRect.m_tRight;
				m_tBottom += tRect.m_tBottom;

				return *this;
			}

			// Subtract another rect from this rect.
			inline CRectT<t_Type>& operator-=(CRectT<t_Type>& tRect)
			{
				m_tLeft -= tRect.m_tLeft;
				m_tTop -= tRect.m_tTop;
				m_tRight -= tRect.m_tRight;
				m_tBottom -= tRect.m_tBottom;

				return *this;
			}

			// Multiply this rect with another rect.
			inline CRectT<t_Type>& operator*=(CRectT<t_Type>& tRect)
			{
				m_tLeft *= tRect.m_tLeft;
				m_tTop *= tRect.m_tTop;
				m_tRight *= tRect.m_tRight;
				m_tBottom *= tRect.m_tBottom;

				return *this;
			}

			// Divide this rect by another rect.
			inline CRectT<t_Type>& operator/=(CRectT<t_Type>& tRect)
			{
				m_tLeft /= tRect.m_tLeft;
				m_tTop /= tRect.m_tTop;
				m_tRight /= tRect.m_tRight;
				m_tBottom /= tRect.m_tBottom;

				return *this;
			}

			// Add a value to all points of this rect.
			inline CRectT<t_Type>& operator+=(t_Type tValue)
			{
				m_tLeft += tValue;
				m_tTop += tValue;
				m_tRight += tValue;
				m_tBottom += tValue;

				return *this;
			}

			// Subtract a value from all points of this rect.
			inline CRectT<t_Type>& operator-=(t_Type tValue)
			{
				m_tLeft -= tValue;
				m_tTop -= tValue;
				m_tRight -= tValue;
				m_tBottom -= tValue;

				return *this;
			}

			// Multiply all points of this rect with a value.
			inline CRectT<t_Type>& operator*=(t_Type tValue)
			{
				m_tLeft *= tValue;
				m_tTop *= tValue;
				m_tRight *= tValue;
				m_tBottom *= tValue;

				return *this;
			}

			// Divide all points of this rect by a value.
			inline CRectT<t_Type>& operator/=(t_Type tValue)
			{
				m_tLeft /= tValue;
				m_tTop /= tValue;
				m_tRight /= tValue;
				m_tBottom /= tValue;

				return *this;
			}

			// Add a point to this rect to change the offset.
			inline CRectT<t_Type>& operator+=(CPointT<t_Type>& tPoint)
			{
				m_tLeft += tPoint.m_tX;
				m_tTop += tPoint.m_tY;
				m_tRight += tPoint.m_tX;
				m_tBottom += tPoint.m_tY;

				return *this;
			}

			// Subtract a point from this rect to change the offset.
			inline CRectT<t_Type>& operator-=(CPointT<t_Type>& tPoint)
			{
				m_tLeft -= tPoint.m_tX;
				m_tTop -= tPoint.m_tY;
				m_tRight -= tPoint.m_tX;
				m_tBottom -= tPoint.m_tY;

				return *this;
			}

			// The rect coordinates.
			t_Type m_tLeft;
			t_Type m_tTop;
			t_Type m_tRight;
			t_Type m_tBottom;
		};
	}
}

//##############################################################################