/**
* @file Crypt.cpp
* @author Nat Ryall
* @date 11/08/2008
* @brief Cryptographic data management.
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

// Local.
#include <Crypt.h>

// Other.
#include <Crypto/cryptlib.h>
#include <Crypto/hex.h>
#include <Crypto/filters.h>

//##############################################################################

//##############################################################################
//
//                                   TYPES
//
//##############################################################################

// Namespaces.
using namespace CryptoPP;

//##############################################################################

//##############################################################################
//
//                               CRYPT MANAGER
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         20-Aug-2008
// =============================================================================
xstring& CCryptManager::GetPublicKey()
{
	// The external key.
	static xstring s_xHexKey =
		#include <Crypto/Keys/RSA-Hex-2048-Public.txt>
	;

	// Hex decode the key and return it.
	static xstring s_xKey;

	StringSource(s_xHexKey, true, new HexDecoder(new StringSink(s_xKey)));

	return s_xKey;
}

// =============================================================================
// Nat Ryall                                                         20-Aug-2008
// =============================================================================
xstring& CCryptManager::GetEncryptionKey()
{
	// The external key.
	static xstring s_xHexKey =
		#include <Crypto/Keys/AES-Hex-256-Key.txt>
	;

	// Hex decode the key and return it.
	static xstring s_xKey;

	StringSource(s_xHexKey, true, new HexDecoder(new StringSink(s_xKey)));

	return s_xKey;
}

//##############################################################################