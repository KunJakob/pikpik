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
//
//                                  INCLUDE
//
//##############################################################################

// Global.
#include <Global.h>

//##############################################################################

//##############################################################################
//
//                                   MACROS
//
//##############################################################################

// Shortcuts.
#define SaveManager CSaveManager::Get()

//##############################################################################

//##############################################################################
//
//                                   TYPES
//
//##############################################################################

// Lists.

//##############################################################################

//##############################################################################
//
//                                SAVE MANAGER
//
//##############################################################################
class CSaveManager
{
public:
	// Singleton instance.
	static inline CSaveManager& Get() 
	{
		static CSaveManager s_Instance;
		return s_Instance;
	}

	// Register a boolean key with the save manager.
	void RegisterKey(const xchar* pKey, xbool bDefault);

	// Register a integer key with the save manager.
	void RegisterKey(const xchar* pKey, xint iDefault);

	// Register a floating-point key with the save manager.
	void RegisterKey(const xchar* pKey, xfloat fDefault);

	// Register a string key with the save manager.
	void RegisterKey(const xchar* pKey, xstring xDefault);

	// Get a boolean value from the save data.
	xbool GetBool(const xchar* pKey);

	// Get an integer value from the save data.
	// ~iMinValue The minimum value to allow for the integer.
	// ~iMaxValue The maximum value to allow for the integer.
	xint GetInt(const xchar* pKey, xint iMinValue = INT_MIN, xint iMaxValue = INT_MAX);

	// Get a floating-point value from the save data.
	// ~fMinValue The minimum value to allow for the float.
	// ~fMaxValue The maximum value to allow for the float.
	xfloat GetFloat(const xchar* pKey, xfloat fMinValue = FLT_MAX, xfloat fMaxValue = FLT_MAX);

	// Get a string value from the save data.
	// ~iMaxLength The maximum string length to read in (includes NULL terminator).
	xstring GetString(const xchar* pKey, xint iMaxLength = 64);

	// Save all registered values to a file.
	void Save(const xchar* pFileName);

	// Load all registered values from a file.
	void Load(const xchar* pFileName);

protected:
	// The list of key-value pairs.
};

//##############################################################################