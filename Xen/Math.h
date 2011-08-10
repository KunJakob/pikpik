#pragma once

//##############################################################################
// 
//	Math.h
//	--------------------------------------------------------------------------
//  Brief:		Set of math related utility functions and types.
//	Project:	Xen
//	Author:		Nat Ryall
//	Date:		07/12/2007
//	License:	Private
//	--------------------------------------------------------------------------
//	Copyright (c) SAPIAN, 2007
// 
//##############################################################################

// Math types.
#include <Xen/Point.h>
#include <Xen/Rect.h>
#include <Xen/Circle.h>

// Standard lib.
#include <math.h>

//##############################################################################

// PI.
#define XEN_PI							3.1415926535897932384626433832795f
#define XEN_INVERSE_PI					0.31830988618379067153776752674503f

// Radians & Degrees.
#define XEN_RADIANS_PER_DEGREE			0.017453292519943295769236907684886f
#define XEN_DEGREES_PER_RADIAN			57.295779513082320876798154814105f

// Floating-point epsilon.
#define XEN_EPSILON						0.001f

//##############################################################################
namespace Xen
{
	typedef Math::CPointT<xint>			xpoint;
	typedef Math::CRectT<xint>			xrect;
	typedef Math::CCircleT<xint>		xcircle;

	typedef Math::CPointT<xfloat>		xpointf;
	typedef Math::CRectT<xfloat>		xrectf;
	typedef Math::CCircleT<xfloat>		xcirclef;
}

//##############################################################################
namespace Xen
{
	namespace Math
	{
		// Get the square of a value.
		template<typename t_Type>
		inline t_Type Square(t_Type tValue)
		{
			return tValue * tValue;
		}

		// Get the power of a value.
		template<typename t_Type>
		inline t_Type Power(t_Type tValue, t_Type tPower)
		{
			return pow(tValue, tPower);
		}

		// Get the square root of a value.
		template<typename t_Type>
		inline t_Type SquareRoot(t_Type tValue)
		{
			return sqrt(tValue);
		}

		// Get the squared magnitude (length) of a point vector.
		template<typename t_Type>
		inline t_Type SquaredMagnitude(CPointT<t_Type> tValue)
		{
			return Dot(tValue, tValue);
		}

		// Get the magnitude (length) of a point vector.
		template<typename t_Type>
		inline t_Type Magnitude(CPointT<t_Type> tValue)
		{
			return SquareRoot(SquaredMagnitude(tValue));
		}

		// Get the dot product between two points.
		template<typename t_Type>
		inline t_Type Dot(CPointT<t_Type> tA, CPointT<t_Type> tB)
		{
			return Square(tA.m_tX) + Square(tA.m_tY);
		}

		// Get the minimum of two values.
		template<typename t_Type>
		inline t_Type Min(t_Type tA, t_Type tB)
		{
			return (tA < tB) ? tA : tB;
		}

		// Get the maximum of two values.
		template<typename t_Type>
		inline t_Type Max(t_Type tA, t_Type tB)
		{
			return (tA > tB) ? tA : tB;
		}

		// Get degrees from a radian value.
		inline xfloat Degrees(xfloat fRadians)
		{
			return fRadians * XEN_DEGREES_PER_RADIAN;
		}

		// Get radians from a degree value.
		inline xfloat Radians(xfloat fDegrees)
		{
			return fDegrees * XEN_RADIANS_PER_DEGREE;
		}

		// Clamps a value between two bounds.
		template<typename t_Type>
		inline t_Type Clamp(t_Type tVar, t_Type tLowest, t_Type tHighest)
		{
			return (tVar < tLowest) ? tLowest : ((tVar > tHighest) ? tHighest : tVar);
		}

		// Clamp a point to a coordinate within a rect area.
		template<typename t_Type>
		inline CPointT<t_Type> Confine(CPointT<t_Type> tPoint, CRectT<t_Type> tRect)
		{
			return CPointT<t_Type>
			(
				Clamp(tPoint.m_tX, tRect.m_tLeft, tRect.m_tRight),
				Clamp(tPoint.m_tY, tRect.m_tTop, tRect.m_tBottom)
			);
		}

		// Get the clipped bounds of one rect within another.
		template<typename t_Type>
		inline CRectT<t_Type> Clip(CRectT<t_Type> tRect, CRectT<t_Type> tClip)
		{
			return CRectT<t_Type>
			(
				(tRect.m_tLeft >= tClip.m_tLeft) ? tRect.m_tLeft : tClip.m_tLeft,
				(tRect.m_tTop >= tClip.m_tTop) ? tRect.m_tTop : tClip.m_tTop,
				(tRect.m_tRight <= tClip.m_tRight) ? tRect.m_tRight : tClip.m_tRight,
				(tRect.m_tBottom <= tClip.m_tBottom) ? tRect.m_tBottom : tClip.m_tBottom
			);
		}

		// Check if a point is intersecting with another point.
		template<typename t_Type>
		inline xbool IsIntersecting(CPointT<t_Type> tA, CPointT<t_Type> tB)
		{
			return tA.m_tX == tB.m_tX && tA.m_tY == tB.m_tY;
		}

		// Check if a rect is intersecting with another rect.
		template<typename t_Type>
		inline xbool IsIntersecting(CRectT<t_Type> tA, CRectT<t_Type> tB)
		{
			return tA.m_tRight >= tB.m_tLeft && tA.m_tLeft <= tB.m_tRight && tA.m_tBottom >= tB.m_tTop && tA.m_tTop <= tB.m_tBottom;
		}

		// Check if a circle is intersecting with another circle.
		template<typename t_Type>
		inline xbool IsIntersecting(CCircleT<t_Type> tA, CCircleT<t_Type> tB)
		{
			return ((tB.m_tX - tA.m_tX) * (tB.m_tX - tA.m_tX)) + ((tB.m_tY - tA.m_tY) * (tB.m_tY - tA.m_tY)) <= (tA.m_tRadius + tB.m_tRadius) * (tA.m_tRadius + tB.m_tRadius);
		}

		// Check if a point is intersecting with a rect.
		template<typename t_Type>
		inline xbool IsIntersecting(CPointT<t_Type> tA, CRectT<t_Type> tB)
		{
			return tA.m_tX >= tB.m_tLeft && tA.m_tX <= tB.m_tRight && tA.m_tY >= tB.m_tTop && tA.m_tY <= tB.m_tBottom;
		}

		// Check if a point is intersecting with a circle.
		template<typename t_Type>
		inline xbool IsIntersecting(CPointT<t_Type> tA, CCircleT<t_Type> tB)
		{
			return ((tB.m_tX - tA.m_tX) * (tB.m_tX - tA.m_tX)) + ((tB.m_tY - tA.m_tY) * (tB.m_tY - tA.m_tY)) > tB.m_tRadius * tB.m_tRadius;
		}

		// Check if a rect is intersecting with a circle.
		template<typename t_Type>
		inline xbool IsIntersecting(CRectT<t_Type> tA, CCircleT<t_Type> tB)
		{
			return IsIntersecting(Confine(tB, tA), tB);
		}
	}
}

//##############################################################################

// Determine if two points are equal.
template<typename t_Type>
inline bool operator==(Xen::Math::CPointT<t_Type>& tA, Xen::Math::CPointT<t_Type>& tB)
{
	return tA.m_tX == tB.m_tX && tA.m_tY == tB.m_tY;
}

// Determine if two points are not equal.
template<typename t_Type>
inline bool operator!=(Xen::Math::CPointT<t_Type>& tA, Xen::Math::CPointT<t_Type>& tB)
{
	return tA.m_tX != tB.m_tX || tA.m_tY != tB.m_tY;
}

// Add two points together.
template<typename t_Type>
inline Xen::Math::CPointT<t_Type> operator+(Xen::Math::CPointT<t_Type>& tA, Xen::Math::CPointT<t_Type>& tB)
{
	return Xen::Math::CPointT<t_Type>(tA.m_tX + tB.m_tX, tA.m_tY + tB.m_tY);
}

// Subtract one point from another.
template<typename t_Type>
inline Xen::Math::CPointT<t_Type> operator-(Xen::Math::CPointT<t_Type>& tA, Xen::Math::CPointT<t_Type>& tB)
{
	return Xen::Math::CPointT<t_Type>(tA.m_tX - tB.m_tX, tA.m_tY - tB.m_tY);
}

// Multiply two points together.
template<typename t_Type>
inline Xen::Math::CPointT<t_Type> operator*(Xen::Math::CPointT<t_Type>& tA, Xen::Math::CPointT<t_Type>& tB)
{
	return Xen::Math::CPointT<t_Type>(tA.m_tX * tB.m_tX, tA.m_tY * tB.m_tY);
}

// Divide one point by another.
template<typename t_Type>
inline Xen::Math::CPointT<t_Type> operator/(Xen::Math::CPointT<t_Type>& tA, Xen::Math::CPointT<t_Type>& tB)
{
	return Xen::Math::CPointT<t_Type>(tA.m_tX / tB.m_tX, tA.m_tY / tB.m_tY);
}

// Get the modulus for one point using another point.
template<typename t_Type>
inline Xen::Math::CPointT<t_Type> operator%(Xen::Math::CPointT<t_Type>& tA, Xen::Math::CPointT<t_Type>& tB)
{
	return Xen::Math::CPointT<t_Type>(tA.m_tX % tB.m_tX, tA.m_tY % tB.m_tY);
}

// Add a value to both axis of a point.
template<typename t_Type>
inline Xen::Math::CPointT<t_Type> operator+(Xen::Math::CPointT<t_Type>& tA, t_Type tB)
{
	return Xen::Math::CPointT<t_Type>(tA.m_tX + tB, tA.m_tY + tB);
}

// Add a value to both axis of a point.
template<typename t_Type>
inline Xen::Math::CPointT<t_Type> operator+(t_Type tA, Xen::Math::CPointT<t_Type>& tB)
{
	return tB + tA;
}

// Subtract a value from both axis of a point.
template<typename t_Type>
inline Xen::Math::CPointT<t_Type> operator-(Xen::Math::CPointT<t_Type>& tA, t_Type tB)
{
	return Xen::Math::CPointT<t_Type>(tA.m_tX - tB, tA.m_tY - tB);
}

// Multiply both axis of a point with a value.
template<typename t_Type>
inline Xen::Math::CPointT<t_Type> operator*(Xen::Math::CPointT<t_Type>& tA, t_Type tB)
{
	return Xen::Math::CPointT<t_Type>(tA.m_tX * tB, tA.m_tY * tB);
}

// Multiply both axis of a point with a value.
template<typename t_Type>
inline Xen::Math::CPointT<t_Type> operator*(t_Type tA, Xen::Math::CPointT<t_Type>& tB)
{
	return tB * tA;
}

// Divide both axis of a point by a value.
template<typename t_Type>
inline Xen::Math::CPointT<t_Type> operator/(Xen::Math::CPointT<t_Type>& tA, t_Type tB)
{
	return Xen::Math::CPointT<t_Type>(tA.m_tX / tB, tA.m_tY / tB);
}

// Get the modulus for both axis of a point using a value.
template<typename t_Type>
inline Xen::Math::CPointT<t_Type> operator%(Xen::Math::CPointT<t_Type>& tA, t_Type tB)
{
	return Xen::Math::CPointT<t_Type>(tA.m_tX % tB, tA.m_tY % tB);
}

// Determine if two rects are equal.
template<typename t_Type>
inline bool operator==(Xen::Math::CRectT<t_Type>& tA, Xen::Math::CRectT<t_Type>& tB)
{
	return tA.m_tLeft == tB.m_tLeft && tA.m_tTop == tB.m_tTop && tA.m_tRight == tB.m_tRight && tA.m_tBottom == tB.m_tBottom;
}

// Determine if two rects are not equal.
template<typename t_Type>
inline bool operator!=(Xen::Math::CRectT<t_Type>& tA, Xen::Math::CRectT<t_Type>& tB)
{
	return tA.m_tLeft != tB.m_tLeft || tA.m_tTop != tB.m_tTop || tA.m_tRight != tB.m_tRight || tA.m_tBottom != tB.m_tBottom;
}

// Add two rects together.
template<typename t_Type>
inline Xen::Math::CRectT<t_Type> operator+(Xen::Math::CRectT<t_Type>& tA, Xen::Math::CRectT<t_Type>& tB)
{
	return Xen::Math::CRectT<t_Type>(tA.m_tLeft + tB.m_tLeft, tA.m_tTop + tB.m_tTop, tA.m_tRight + tB.m_tRight, tA.m_tBottom + tB.m_tBottom);
}

// Subtract one rect from another.
template<typename t_Type>
inline Xen::Math::CRectT<t_Type> operator-(Xen::Math::CRectT<t_Type>& tA, Xen::Math::CRectT<t_Type>& tB)
{
	return Xen::Math::CRectT<t_Type>(tA.m_tLeft - tB.m_tLeft, tA.m_tTop - tB.m_tTop, tA.m_tRight - tB.m_tRight, tA.m_tBottom - tB.m_tBottom);
}

// Multiply two rects together.
template<typename t_Type>
inline Xen::Math::CRectT<t_Type> operator*(Xen::Math::CRectT<t_Type>& tA, Xen::Math::CRectT<t_Type>& tB)
{
	return Xen::Math::CRectT<t_Type>(tA.m_tLeft * tB.m_tLeft, tA.m_tTop * tB.m_tTop, tA.m_tRight * tB.m_tRight, tA.m_tBottom * tB.m_tBottom);
}

// Divide one rect by another.
template<typename t_Type>
inline Xen::Math::CRectT<t_Type> operator/(Xen::Math::CRectT<t_Type>& tA, Xen::Math::CRectT<t_Type>& tB)
{
	return Xen::Math::CRectT<t_Type>(tA.m_tLeft / tB.m_tLeft, tA.m_tTop / tB.m_tTop, tA.m_tRight / tB.m_tRight, tA.m_tBottom / tB.m_tBottom);
}

// Add a point to the top-left and bottom-right of a rect.
template<typename t_Type>
inline Xen::Math::CRectT<t_Type> operator+(Xen::Math::CRectT<t_Type>& tA, Xen::Math::CPointT<t_Type>& tB)
{
	return Xen::Math::CRectT<t_Type>(tA.m_tLeft + tB.m_tX, tA.m_tTop + tB.m_tY, tA.m_tRight + tB.m_tX, tA.m_tBottom + tB.m_tY);
}

// Add a point to the top-left and bottom-right of a rect.
template<typename t_Type>
inline Xen::Math::CRectT<t_Type> operator+(Xen::Math::CPointT<t_Type>& tA, Xen::Math::CRectT<t_Type>& tB)
{
	return tB + tA;
}

// Subtract a point from the top-left and bottom-right of a rect.
template<typename t_Type>
inline Xen::Math::CRectT<t_Type> operator-(Xen::Math::CRectT<t_Type>& tA, Xen::Math::CPointT<t_Type>& tB)
{
	return Xen::Math::CRectT<t_Type>(tA.m_tLeft - tB.m_tX, tA.m_tTop - tB.m_tY, tA.m_tRight - tB.m_tX, tA.m_tBottom - tB.m_tY);
}

// Multiply the top-left and bottom-right of a rect with a point.
template<typename t_Type>
inline Xen::Math::CRectT<t_Type> operator*(Xen::Math::CRectT<t_Type>& tA, Xen::Math::CPointT<t_Type>& tB)
{
	return Xen::Math::CRectT<t_Type>(tA.m_tLeft * tB.m_tX, tA.m_tTop * tB.m_tY, tA.m_tRight * tB.m_tX, tA.m_tBottom * tB.m_tY);
}

// Multiply the top-left and bottom-right of a rect with a point.
template<typename t_Type>
inline Xen::Math::CRectT<t_Type> operator*(Xen::Math::CPointT<t_Type>& tA, Xen::Math::CRectT<t_Type>& tB)
{
	return tB * tA;
}

// Divide the top-left and bottom-right of a rect by a point.
template<typename t_Type>
inline Xen::Math::CRectT<t_Type> operator/(Xen::Math::CRectT<t_Type>& tA, Xen::Math::CPointT<t_Type>& tB)
{
	return Xen::Math::CRectT<t_Type>(tA.m_tLeft / tB.m_tX, tA.m_tTop / tB.m_tY, tA.m_tRight / tB.m_tX, tA.m_tBottom / tB.m_tY);
}

// Add a value to all points of a rect.
template<typename t_Type>
inline Xen::Math::CRectT<t_Type> operator+(Xen::Math::CRectT<t_Type>& tA, t_Type tB)
{
	return Xen::Math::CRectT<t_Type>(tA.m_tLeft + tB, tA.m_tTop + tB, tA.m_tRight + tB, tA.m_tBottom + tB);
}

// Add a value to all points of a rect.
template<typename t_Type>
inline Xen::Math::CRectT<t_Type> operator+(t_Type tA, Xen::Math::CRectT<t_Type>& tB)
{
	return tB + tA;
}

// Subtract a value from all points of a rect.
template<typename t_Type>
inline Xen::Math::CRectT<t_Type> operator-(Xen::Math::CRectT<t_Type>& tA, t_Type tB)
{
	return Xen::Math::CRectT<t_Type>(tA.m_tLeft - tB, tA.m_tTop - tB, tA.m_tRight - tB, tA.m_tBottom - tB);
}

// Multiply all points of a rect with a value.
template<typename t_Type>
inline Xen::Math::CRectT<t_Type> operator*(Xen::Math::CRectT<t_Type>& tA, t_Type tB)
{
	return Xen::Math::CRectT<t_Type>(tA.m_tLeft * tB, tA.m_tTop * tB, tA.m_tRight * tB, tA.m_tBottom * tB);
}

// Multiply all points of a rect with a value.
template<typename t_Type>
inline Xen::Math::CRectT<t_Type> operator*(t_Type tA, Xen::Math::CRectT<t_Type>& tB)
{
	return tB * tA;
}

// Divide all points of a rect by a value.
template<typename t_Type>
inline Xen::Math::CRectT<t_Type> operator/(Xen::Math::CRectT<t_Type>& tA, t_Type tB)
{
	return Xen::Math::CRectT<t_Type>(tA.m_tLeft / tB, tA.m_tTop / tB, tA.m_tRight / tB, tA.m_tBottom / tB);
}

//##############################################################################