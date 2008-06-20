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
	CProperty(CDataset* pParent, XCHAR* pType) : m_pParent(pParent), m_pType(pType) {}

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
	const XCHAR* GetType()
	{
		return (const XCHAR*)m_pType;
	}

	/**
	* Get the number of values in the property. 
	*/
	XUINT GetValueCount()
	{
		return (XUINT)m_lpValues.size();
	}

	/**
	* Get the property value as a char.
	* @param iIndex Specify the index offset of the value to retrieve.
	*/
	XCHAR GetChar(XUINT iIndex = 0);

	/**
	* Get the property value as an integer.
	* @param iIndex Specify the index offset of the value to retrieve.
	*/
	XINT GetInt(XUINT iIndex = 0);

	/**
	* Get the property value as a float.
	* @param iIndex Specify the index offset of the value to retrieve.
	*/
	XFLOAT GetFloat(XUINT iIndex = 0);

	/**
	* Get the property value as a boolean.
	* @param iIndex Specify the index offset of the value to retrieve.
	*/
	XBOOL GetBool(XUINT iIndex = 0);

	/**
	* Get the property value as a string.
	* @param iIndex Specify the index offset of the value to retrieve.
	*/
	const XCHAR* GetString(XUINT iIndex = 0);

	/**
	* Get the property value as a point.
	*/
	XPOINT GetPoint(XUINT iIndex = 0)
	{
		XUINT iOffset = iIndex * 2;
		return XPOINT(GetInt(iOffset + 0), GetInt(iOffset + 1));
	}

	/**
	* Get the property value as a rect.
	*/
	XRECT GetRect(XUINT iIndex = 0)
	{
		XUINT iOffset = iIndex * 4;
		return XRECT(GetInt(iOffset + 0), GetInt(iOffset + 1), GetInt(iOffset + 2), GetInt(iOffset + 3));
	}

protected:
	// Lists.
	typedef std::vector<XCHAR*> t_ValueList;

	// The parent dataset.
	CDataset* m_pParent;

	// The type of the property.
	XCHAR* m_pType;

	// List of values held within the property.
	t_ValueList m_lpValues;
};

//##############################################################################
#pragma endregion