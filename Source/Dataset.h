#pragma once

/**
* @file Dataset.h
* @author Nat Ryall
* @date 29/01/2008
*
* Copyright © Creative Disorder
*/

//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Global.
#include <Global.h>

// Other.
#include <Property.h>

//##############################################################################

//##############################################################################
//
//                                   TYPES
//
//##############################################################################

// Predeclare.
class CMetadata;

//##############################################################################

//##############################################################################
//
//                                 DECLARATION
//
//##############################################################################
class CDataset
{
public:
	// Friends.
	friend CMetadata;

	/**
	* Constructor. 
	*/
	CDataset(CDataset* pParent, const xchar* pType, const xchar* pName) : m_pParent(pParent), m_pType(pType), m_pName(pName) {}

	/**
	* Destructor.
	*/
	virtual ~CDataset();

	/**
	* Get the parent dataset. Returns NULL if called on the metadata object.
	*/
	CDataset* GetParent()
	{
		return m_pParent;
	}

	/**
	* Get the type of the dataset.
	*/
	const xchar* GetType()
	{
		return m_pType;
	}

	/**
	* Get the name of the dataset. Returns NULL if no name was specified.
	*/
	const xchar* GetName()
	{
		return m_pName;
	}

	/**
	* Get the total number of subsets available with the specified type.
	* @param pType The type of the dataset to count instances of. Specify NULL to count all subsets in the current scope.
	*/
	xuint GetDatasetCount(const xchar* pType = NULL);

	/**
	* Get a child dataset by type. Type must be specified but name can be set to NULL if no name is specified in metadata.
	* @return A valid dataset object or NULL if no dataset is found.
	*/
	CDataset* GetDataset(const xchar* pType, const xchar* pName = NULL)
	{
		return GetDataset(NULL, pType, pName);
	}

	/**
	* Get the next child dataset by type after the specified dataset. Type must be specified but name can be set to NULL if no name is specified in metadata.
	* @param pPrev The dataset to start searching from. This dataset is used for locational purposes only and is not included in the search.
	* @return A valid dataset object or NULL if no dataset is found.
	*/
	CDataset* GetDataset(CDataset* pPrev, const xchar* pType, const xchar* pName = NULL);

	/**
	* Get a child dataset by index.
	* @return A valid dataset object or NULL if the index is out of bounds.
	*/
	CDataset* GetDataset(xuint iIndex)
	{
		return (iIndex < m_lpDatasets.size()) ? m_lpDatasets[iIndex] : NULL;
	}

	/**
	* Get the total number of properties available with the specified name.
	* @param pType The type name of the property to count instances of. Specify NULL to count all properties in the current scope.
	*/
	xuint GetPropertyCount(const xchar* pType = NULL);

	/**
	* Get a property by type name.
	* @return A valid property object or NULL if no property is found.
	*/
	CProperty* GetProperty(const xchar* pType)
	{
		return GetProperty(NULL, pType);
	}

	/**
	* Get a property by type name after the specified property.
	* @param pPrev The property to start searching from. This property is used for locational purposes only and is not included in the search.
	* @return A valid property object or NULL if no property is found.
	*/
	CProperty* GetProperty(CProperty* pPrev, const xchar* pType);

	/**
	* Get a child property by index.
	* @return A valid property object or NULL if the index is out of bounds.
	*/
	CProperty* GetProperty(xuint iIndex)
	{
		return (iIndex < m_lpProperties.size()) ? m_lpProperties[iIndex] : NULL;
	}

protected:
	// Lists.
	typedef xarray<CDataset*> t_DatasetList;
	typedef xarray<CProperty*> t_PropertyList;

	// The parent dataset.
	CDataset* m_pParent;

	// The local dataset type.
	const xchar* m_pType;

	// The local dataset name.
	const xchar* m_pName;

	// A list of all child subsets.
	t_DatasetList m_lpDatasets;

	// A list of all properties associated with the dataset.
	t_PropertyList m_lpProperties;
};

//##############################################################################