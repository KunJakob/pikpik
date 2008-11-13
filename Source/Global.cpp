//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Global.
#include <Global.h>

// Other.
#include <Metadata.h>

//##############################################################################

//##############################################################################
//
//                                   GLOBAL
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         13-Nov-2008
// =============================================================================
const xchar* CGlobal::GetLocale(const xchar* pName)
{
	if (m_pLocale)
	{
		if (METADATA_PROPERTY_EXISTS(m_pLocale, pName))
			return METADATA_PROPERTY_VALUE->GetString();
	}

	return pName;
}

// =============================================================================
// Nat Ryall                                                         23-Oct-2008
// =============================================================================
const xchar* CGlobal::GetLocaleFromVar(const xchar* pInput)
{
	// Check if we have a locale string: "&NAME". 
	if (pInput && String::Length(pInput) > 1 && pInput[0] == '&')
	{
		// If we see "&&" it means we want to use '&'.
		if (pInput[1] == '&')
			return &pInput[1];
		
		return GetLocale(&pInput[1]);
	}

	// We should just return the string otherwise.
	return pInput;
}

//##############################################################################