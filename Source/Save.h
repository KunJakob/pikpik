#pragma once

/**
* @file Save.h
* @author Nat Ryall
* @date 07/08/2008
* @brief Persistent data saving and loading.
*
* Copyright © SAPIAN
*/

//##############################################################################

// Global.
#include <Global.h>

//##############################################################################

// Shortcuts.
//#define SaveManager CSaveManager::Get()

//##############################################################################
/*class CSaveManager
{
public:
	// Singleton instance.
	static inline CSaveManager& Get() 
	{
		static CSaveManager s_Instance;
		return s_Instance;
	}

	// Register a boolean key with the save manager.
	// ~bDefault The default value to use if the key is unset.
	void RegisterKey(const xchar* pKey, xbool bDefault);

	// Register a integer key with the save manager.
	// ~iDefault The default value to use if the key is unset.
	// ~iMinValue The minimum value to allow for the integer.
	// ~iMaxValue The maximum value to allow for the integer.
	void RegisterKey(const xchar* pKey, xint iDefault, xint iMinValue = INT_MIN, xint iMaxValue = INT_MAX);

	// Register a floating-point key with the save manager.
	// ~fDefault The default value to use if the key is unset.
	// ~fMinValue The minimum value to allow for the float.
	// ~fMaxValue The maximum value to allow for the float.
	void RegisterKey(const xchar* pKey, xfloat fDefault, xfloat fMinValue = FLT_MAX, xfloat fMaxValue = FLT_MAX);

	// Register a string key with the save manager.
	// ~xDefault The default value to use if the key is unset.
	// ~iMaxLength The maximum string length to read in (includes NULL terminator).
	void RegisterKey(const xchar* pKey, xstring xDefault, xint iMaxLength = 64);

	// Get a boolean value from the save data.
	xbool GetBool(const xchar* pKey);

	// Get an integer value from the save data.
	xint GetInt(const xchar* pKey);

	// Get a floating-point value from the save data.
	xfloat GetFloat(const xchar* pKey);

	// Get a string value from the save data.
	xstring GetString(const xchar* pKey);

	// Save all registered values to a file.
	void Save(const xchar* pFileName);

	// Load all registered values from a file.
	void Load(const xchar* pFileName);

protected:
};*/

//##############################################################################