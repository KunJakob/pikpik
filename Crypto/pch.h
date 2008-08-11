#ifndef CRYPTOPP_PCH_H
#define CRYPTOPP_PCH_H

#ifdef CRYPTOPP_GENERATE_X64_MASM

	#include "Crypto/cpu.h"

#else

	#include "Crypto/config.h"

	#ifdef USE_PRECOMPILED_HEADERS
		#include "Crypto/simple.h"
		#include "Crypto/secblock.h"
		#include "Crypto/misc.h"
		#include "Crypto/smartptr.h"
	#endif

#endif

#endif
