/**
* @file Crypt.cpp
* @author Nat Ryall
* @date 11/08/2008
* @brief Cryptographic data management.
*
* Copyright © SAPIAN
*/

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

// Namespaces.
using namespace CryptoPP;

//##############################################################################

// =============================================================================
const xchar* CCryptManager::GetPublicKey()
{
	// The external key.
	static xstring s_xHexKey =
		#include <Crypto/Keys/RSA-Hex-2048-Public.txt>
	;

	// Hex decode the key and return it.
	static xstring s_xKey;

	if (s_xKey.length() == 0)
		StringSource(s_xHexKey, true, new HexDecoder(new StringSink(s_xKey)));

	return s_xKey.c_str();
}

// =============================================================================
const xchar* CCryptManager::GetEncryptionKey()
{
	// The external key.
	static xstring s_xHexKey =
		#include <Crypto/Keys/AES-Hex-256-Key.txt>
	;

	// Hex decode the key and return it.
	static xstring s_xKey;

	if (s_xKey.length() == 0)
		StringSource(s_xHexKey, true, new HexDecoder(new StringSink(s_xKey)));

	return s_xKey.c_str();
}

//##############################################################################