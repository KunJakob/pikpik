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
	srand((unsigned int)time(NULL));

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
			cIV[iA] = 0xF0;

		CFB_Mode<AES>::Encryption xAES((byte*)xKey.c_str(), xKey.length() / 2, cIV);
		StringSource(xFormattedMetadata, true, new StreamTransformationFilter(xAES, new FileSink(pOutputFile)));
	}

	cout << "  Completed! " << "\n" << "\n";

	// Exit.
	return 0;
}

//##############################################################################