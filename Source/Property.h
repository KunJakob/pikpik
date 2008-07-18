#pragma once

/**
* @file Property.h
* @author Nat Ryall
* @date 29/01/2008
*
* Copyright © Creative Disorder
*/

#pragma region Include
//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Global.
#include <Global.h>

//##############################################################################
#pragma endregion

#pragma region Types
//##############################################################################
//
//                                   TYPES
//
//##############################################################################

// Predeclare.
class CMetadata;
class CDataset;

//##############################################################################
#pragma endregion

#pragma region Declaration
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

	/**
	* Constructor. 
	*/
	CProperty(CDataset* pParent, xchar* pType) : m_pParent(pParent), m_pType(pType) {}

	/**
	* Get the dataset this property belongs to.
	*/
	CDataset* GetParent()
	{
		return m_pParent;
	}

	/**
	* Get the type of the property.
	*/
	const xchar* GetType()
	{
		return (const xchar*)m_pType;
	}

	/**
	* Get the number of values in the property. 
	*/
	xuint GetValueCount()
	{
		return (xuint)m_lpValues.size();
	}

	/**
	* Get the property value as a char.
	* @param iIndex Specify the index offset of the value to retrieve.
	*/
	xchar GetChar(xuint iIndex = 0);

	/**
	* Get the property value as an integer.
	* @param iIndex Specify the index offset of the value to retrieve.
	*/
	xint GetInt(xuint iIndex = 0);

	/**
	* Get the property value as a float.
	* @param iIndex Specify the index offset of the value to retrieve.
	*/
	xfloat GetFloat(xuint iIndex = 0);

	/**
	* Get the property value as a boolean.
	* @param iIndex Specify the index offset of the value to retrieve.
	*/
	xbool GetBool(xuint iIndex = 0);

	/**
	* Get the property value as a string.
	* @param iIndex Specify the index offset of the value to retrieve.
	*/
	const xchar* GetString(xuint iIndex = 0);

	/**
	* Get the property value as a point.
	*/
	xpoint GetPoint(xuint iIndex = 0)
	{
		xuint iOffset = iIndex * 2;
		return xpoint(GetInt(iOffset + 0), GetInt(iOffset + 1));
	}

	/**
	* Get the property value as a rect.
	*/
	xrect GetRect(xuint iIndex = 0)
	{
		xuint iOffset = iIndex * 4;
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
#pragma endregion