//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Local.
#include <Main.h>

// Standard Lib.
#include <iostream>
#include <string>
#include <time.h>

// Crypto.
#include <Crypto/cryptlib.h>
#include <Crypto/filters.h>
#include <Crypto/modes.h>
#include <Crypto/files.h>
#include <Crypto/aes.h>
#include <Crypto/hex.h>

// Other.
#include <Windows.h>

//##############################################################################

//##############################################################################
//
//                                   MACROS
//
//##############################################################################

// The instruction sheet.
#define TEXT_HEADER \
		"\n" \
		"  Metadata Compiler" "\n" \
		"  Revision 1" "\n" \
		"  ---------------------------------------------------------------------------  " "\n" \
		"  Copyright © SAPIAN, 2008, All Rights Reserved" "\n" \
		"\n" \

//##############################################################################

//##############################################################################
//
//                                   TYPES
//
//##############################################################################

// Namespace.
using namespace std;
using namespace CryptoPP;

//##############################################################################

//##############################################################################
//
//                                    MAIN
//
//##############################################################################

// =============================================================================
// Nat Ryall                                                         19-Aug-2008
// =============================================================================
int main(int iNumArgs, const char* pArgs[])
{
	srand(GetTickCount());

	// Output the header.
	cout << TEXT_HEADER;

	// If we have enough arguments to process the command.
	if (iNumArgs == 4 || iNumArgs == 5)
	{
		const char* pInputFile = pArgs[1];
		const char* pOutputFile = pArgs[2];
		const char* pInputKeyFile = pArgs[3];

		// Read the key in, hex encode it and write it out.
		cout << "  Reading Key: " << pInputKeyFile << "\n";

		string xKey;
		FileSource(pInputKeyFile, true, new StringSink(xKey));

		if (iNumArgs == 5)
		{
			const char* pOutputKeyFile = pArgs[4];

			cout << "  Writing Hex Encoded Key: " << pOutputKeyFile << "\n";
			
			StringSource(xKey, true, new HexEncoder(new FileSink(pOutputKeyFile)));
		}
		
		// Read in the specified file.
		cout << "  Reading Metadata: " << pInputFile << "\n";

		string xMetadata;
		FileSource(pInputFile, true, new StringSink(xMetadata));

		// Format the metadata to single "space" separation.
		cout << "  Formatting Metadata" << "\n";

		string xFormattedMetadata;

		bool bSeenWhiteSpace = false;
		bool bIsQuotes = false;

		for (int iA = 0; iA < (int)xMetadata.length(); ++iA)
		{
			// Ignore comments.
			if (xMetadata[iA] == '/' && xMetadata[iA + 1] == '/')
			{
				while (iA < (int)xMetadata.length() && xMetadata[iA] != '\n')
					++iA;
			}

			if (!bIsQuotes && iswspace(xMetadata[iA]))
			{
				if (!bSeenWhiteSpace)
				{
					xFormattedMetadata += ' ';
					bSeenWhiteSpace = true;
				}
			}
			else
			{
				if (xMetadata[iA] == '"')
					bIsQuotes = !bIsQuotes;

				bSeenWhiteSpace = false;
				xFormattedMetadata += xMetadata[iA];
			}
		}

		// Encrypt the metadata and write the output file.
		cout << "  Writing Encrypted Metadata: " << pOutputFile << "\n";

		byte cIV[AES::BLOCKSIZE];

		for (int iA = 0; iA < AES::BLOCKSIZE; ++iA)
			cIV[iA] = rand() % 256;

		CFB_Mode<AES>::Encryption xAES((byte*)xKey.c_str(), xKey.length() / 2, cIV);

		string xEncryptedMetadata;
		StringSource(xFormattedMetadata, true, new StreamTransformationFilter(xAES, new StringSink(xEncryptedMetadata)));

		xEncryptedMetadata.insert(0, (const char*)cIV, AES::BLOCKSIZE);

		StringSource(xEncryptedMetadata, true, new FileSink(pOutputFile));
	}

	// We're finished.
	cout << "  Completed! " << "\n" << "\n";

	// Exit.
	return 0;
}

//##############################################################################