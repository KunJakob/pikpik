#pragma once

/**
* @file Crypt.h
* @author Nat Ryall
* @date 11/08/2008
* @brief Cryptographic data management.
*
* Copyright © SAPIAN
*/

//##############################################################################

// Global.
#include <Global.h>

//##############################################################################

// Shortcuts.
#define CryptManager CCryptManager::Get()

//##############################################################################
class CCryptManager
{
public:
	// Singleton instance.
	static inline CCryptManager& Get() 
	{
		static CCryptManager s_Instance;
		return s_Instance;
	}

	// Get the application public key.
	const xchar* GetPublicKey();

	// Get the application data encryption key.
	const xchar* GetEncryptionKey();

protected:
};

//##############################################################################