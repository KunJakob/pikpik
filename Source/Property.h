#pragma once

/**
* @file Property.h
* @author Nat Ryall
* @date 29/01/2008
*
* Copyright © SAPIAN
*/

//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Global.
#include <Global.h>

//##############################################################################

//##############################################################################
//
//                                   TYPES
//
//##############################################################################

// Predeclare.
class CMetadata;
class CDataset;

//##############################################################################

//##############################################################################
//
//                                 DECLARATION
//
//##############################################################################
class CProperty
{
public:
	// Friends.
	friend CMetadata;

	// Constructor. 
	CProperty(CDataset* pParent, xchar* pType) : m_pParent(pParent), m_pType(pType) {}

	// Get the dataset this property belongs to.
	inline CDataset* GetParent()
	{
		return m_pParent;
	}

	// Get the type of the property.
	inline const xchar* GetType()
	{
		return (const xchar*)m_pType;
	}

	// Get the number of values in the property. 
	inline xint GetValueCount()
	{
		return (xint)m_lpValues.size();
	}

	// Get the property value as a char.
	// ~iIndex Specify the index offset of the value to retrieve.
	xchar GetChar(xint iIndex = 0);

	// Get the property value as an integer.
	// ~iIndex Specify the index offset of the value to retrieve.
	xint GetInt(xint iIndex = 0);

	// Get the property value as a float.
	// ~iIndex Specify the index offset of the value to retrieve.
	xfloat GetFloat(xint iIndex = 0);

	// Get the property value as a boolean.
	// ~iIndex Specify the index offset of the value to retrieve.
	xbool GetBool(xint iIndex = 0);

	// Get the property value as a string.
	// ~iIndex Specify the index offset of the value to retrieve.
	const xchar* GetString(xint iIndex = 0);

	// Get the property value as a point.
	inline xpoint GetPoint(xint iIndex = 0)
	{
		xint iOffset = iIndex * 2;
		return xpoint(GetInt(iOffset + 0), GetInt(iOffset + 1));
	}

	// Get the property value as a rect.
	inline xrect GetRect(xint iIndex = 0)
	{
		xint iOffset = iIndex * 4;
		return xrect(GetInt(iOffset + 0), GetInt(iOffset + 1), GetInt(iOffset + 2), GetInt(iOffset + 3));
	}

protected:
	// Lists.
	typedef std::vector<xchar*> t_ValueList;

	// The parent dataset.
	CDataset* m_pParent;

	// The type of the property.
	xchar* m_pType;

	// List of values held within the property.
	t_ValueList m_lpValues;
};

//##############################################################################