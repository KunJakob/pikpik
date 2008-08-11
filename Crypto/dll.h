#ifndef CRYPTOPP_DLL_H
#define CRYPTOPP_DLL_H

#if !defined(CRYPTOPP_IMPORTS) && !defined(CRYPTOPP_EXPORTS) && !defined(CRYPTOPP_DEFAULT_NO_DLL)
#ifdef CRYPTOPP_CONFIG_H
#error To use the DLL version of Crypto++, this file must be included before any other Crypto++ header files.
#endif
#define CRYPTOPP_IMPORTS
#endif

#include "Crypto/aes.h"
#include "Crypto/cbcmac.h"
#include "Crypto/channels.h"
#include "Crypto/des.h"
#include "Crypto/dh.h"
#include "Crypto/dsa.h"
#include "Crypto/ec2n.h"
#include "Crypto/eccrypto.h"
#include "Crypto/ecp.h"
#include "Crypto/files.h"
#include "Crypto/fips140.h"
#include "Crypto/hex.h"
#include "Crypto/hmac.h"
#include "Crypto/modes.h"
#include "Crypto/mqueue.h"
#include "Crypto/nbtheory.h"
#include "Crypto/osrng.h"
#include "Crypto/pkcspad.h"
#include "Crypto/pssr.h"
#include "Crypto/randpool.h"
#include "Crypto/rsa.h"
#include "Crypto/rw.h"
#include "Crypto/sha.h"
#include "Crypto/skipjack.h"
#include "Crypto/trdlocal.h"

#ifdef CRYPTOPP_IMPORTS

#ifdef _DLL
// cause CRT DLL to be initialized before Crypto++ so that we can use malloc and free during DllMain()
#ifdef NDEBUG
#pragma comment(lib, "msvcrt")
#else
#pragma comment(lib, "msvcrtd")
#endif
#endif

#pragma comment(lib, "cryptopp")

#endif		// #ifdef CRYPTOPP_IMPORTS

#include <new>	// for new_handler

NAMESPACE_BEGIN(CryptoPP)

#if !(defined(_MSC_VER) && (_MSC_VER < 1300))
using std::new_handler;
#endif

typedef void * (CRYPTOPP_API * PNew)(size_t);
typedef void (CRYPTOPP_API * PDelete)(void *);
typedef void (CRYPTOPP_API * PGetNewAndDelete)(PNew &, PDelete &);
typedef new_handler (CRYPTOPP_API * PSetNewHandler)(new_handler);
typedef void (CRYPTOPP_API * PSetNewAndDelete)(PNew, PDelete, PSetNewHandler);

NAMESPACE_END

#endif
