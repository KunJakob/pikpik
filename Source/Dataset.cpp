#pragma region Include
//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Global.
#include <Global.h>

// Local.
#include <Dataset.h>

//##############################################################################
#pragma endregion

#pragma region Definition
//##############################################################################
//
//                                 DEFINITION
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         14-Apr-2008
// =============================================================================
CDataset::~CDataset()
{
	XEN_LIST_ERASE_ALL(m_lpDatasets);
	XEN_LIST_ERASE_ALL(m_lpProperties);
}

// =============================================================================
// Author: Nat Ryall                                            Date: 1-Feb-2008
// =============================================================================
xuint CDataset::GetDatasetCount(const xchar* pType)
{
	if (!pType)
		return (xuint)m_lpDatasets.size();
	else
	{
		xuint iCount = 0;

		XEN_LIST_FOREACH(t_DatasetList, ppDataset, m_lpDatasets)
		{
			if (strcmp((*ppDataset)->GetType(), pType) == 0)
				++iCount;
		}

		return iCount;
	}
}

// =============================================================================
// Author: Nat Ryall                                            Date: 1-Feb-2008
// =============================================================================
CDataset* CDataset::GetDataset(CDataset* pPrev, const xchar* pType, const xchar* pName)
{
	t_DatasetList::iterator ppDataset = m_lpDatasets.begin();

	if (pPrev)
	{
		for (; ppDataset < m_lpDatasets.end(); ppDataset++)
		{
			if (*ppDataset == pPrev)
			{
				ppDataset++;
				break;
			}
		}
	}

	for (; ppDataset < m_lpDatasets.end(); ppDataset++)
	{
		if (strcmp((*ppDataset)->GetType(), pType) == 0)
		{
			if (!pName || ((*ppDataset)->GetName() && strcmp((*ppDataset)->GetName(), pName) == 0))
				return *ppDataset;
		}
	}

	return NULL;
}

// =============================================================================
// Author: Nat Ryall                                            Date: 1-Feb-2008
// =============================================================================
xuint CDataset::GetPropertyCount(const xchar* pType)
{
	if (!pType)
		return (xuint)m_lpProperties.size();
	else
	{
		xuint iCount = 0;

		XEN_LIST_FOREACH(t_PropertyList, ppProperty, m_lpProperties)
		{
			if (strcmp((*ppProperty)->GetType(), pType) == 0)
				++iCount;
		}

		return iCount;
	}
}

// =============================================================================
// Author: Nat Ryall                                            Date: 1-Feb-2008
// =============================================================================
CProperty* CDataset::GetProperty(CProperty* pPrev, const xchar* pType)
{
	t_PropertyList::iterator ppProperty = m_lpProperties.begin();

	if (pPrev)
	{
		for (; ppProperty < m_lpProperties.end(); ppProperty++)
		{
			if (*ppProperty == pPrev)
			{
				ppProperty++;
				break;
			}
		}
	}

	for (; ppProperty < m_lpProperties.end(); ppProperty++)
	{
		if (strcmp((*ppProperty)->GetType(), pType) == 0)
			return *ppProperty;
	}

	return NULL;
}

//##############################################################################
#pragma endregion