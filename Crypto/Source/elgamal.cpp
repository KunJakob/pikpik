// elgamal.cpp - written and placed in the public domain by Wei Dai

#include "Crypto/pch.h"
#include "Crypto/elgamal.h"
#include "Crypto/asn.h"
#include "Crypto/nbtheory.h"

NAMESPACE_BEGIN(CryptoPP)

void ElGamal_TestInstantiations()
{
	ElGamalEncryptor test1(1, 1, 1);
	ElGamalDecryptor test2(NullRNG(), 123);
	ElGamalEncryptor test3(test2);
}

NAMESPACE_END
