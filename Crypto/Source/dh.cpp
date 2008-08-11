// dh.cpp - written and placed in the public domain by Wei Dai

#include "Crypto/pch.h"

#ifndef CRYPTOPP_IMPORTS

#include "Crypto/dh.h"

NAMESPACE_BEGIN(CryptoPP)

void DH_TestInstantiations()
{
	DH dh1;
	DH dh2(NullRNG(), 10);
}

NAMESPACE_END

#endif
