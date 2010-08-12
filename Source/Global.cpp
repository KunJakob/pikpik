//##############################################################################

// Global.
#include <Global.h>

// Other.
#include <Player.h>
#include <Map.h>

//##############################################################################

// =============================================================================
const xchar* CGlobal::GetLocale(const xchar* pName)
{
	if (m_pLocale)
	{
		if (XEN_METADATA_PROPERTY_EXISTS(m_pLocale, pName))
			return XEN_METADATA_PROPERTY->GetString();
	}

	return pName;
}

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

//##############################################################################