#pragma once

/**
* @file Metadata.h
* @author Nat Ryall
* @date 29/01/2008
*
* Copyright © Creative Disorder
*/

//##############################################################################
//
//                                   INCLUDE
//
//##############################################################################

// Global.
#include <Global.h>

// Other.
#include <Dataset.h>
#include <Property.h>

//##############################################################################

//##############################################################################
//
//                                   MACROS
//
//##############################################################################
#define _PROPERTY_VALUE \
		_pProperty

#define _DATASET_VALUE \
		_pDataset

#define _PROPERTY_FOREACH(ITER, DATASET, NAME) \
		for (CProperty* ITER = NULL; ITER = DATASET->GetProperty(ITER, NAME);)

#define _DATASET_FOREACH(ITER, DATASET, TYPE, NAME) \
		for (CDataset* ITER = NULL; ITER = DATASET->GetDataset(ITER, TYPE, NAME);)

#define _PROPERTY_EXISTS(DATASET, PROPERTYNAME) \
		CProperty* _PROPERTY_VALUE = DATASET->GetProperty(PROPERTYNAME)

#define _DATASET_EXISTS(DATASET, DATASETNAME) \
		CDataset* _DATASET_VALUE = DATASET->GetDataset(DATASETNAME)

//##############################################################################

//##############################################################################
//
//                                   TYPES
//
//##############################################################################

// Predeclare.
class CMetadata;

// The metadata source type.
enum t_MetadataSourceType
{
	MetadataSourceType_Raw,
	MetadataSourceType_Encrypted,
};

// The metadata processing tasks.
enum t_MetadataTask
{
	MetadataTask_LoadFile,				// Loading the contents of the metadata file into memory.
	MetadataTask_DecryptData,			// Decrypting the metadata.
	MetadataTask_TokeniseData,			// Tokenising the metadata.
	MetadataTask_ParseTokens,			// Building structures from the tokenised data.
	MetadataTask_Completed,				// All tasks have completed.
	MetadataTask_Error,					// Tasks have ceased due to an error.
};

//##############################################################################

//##############################################################################
//
//                                 DECLARATION
//
//##############################################################################
class CMetadata : public CDataset
{
public:
	// Constructor.
	// ~pMetadataFile The file path to the signed-encrypted metadata file.
	// ~pEncryptionKey The AES key used to encrypt the metadata. Specify NULL if the metadata is not encrypted.
	// ~bExecute Specify true to process the metadata synchronously in the constructor.
	CMetadata(const xchar* pMetadataFile, const xchar* pEncryptionKey, xbool bProcessImmediately);

	// Update the metadata processing. This should be called every frame and will run up to iMaxTime milliseconds. Check IsCompleted to see if the metadata is ready to be used.
	// ~iTargetTime The target time, in milliseconds, allowed for the update. This keeps the metadata asychronous and fast.
	// ~iChunkSize The number of bytes to read each cycle. This number of bytes will be read in chunks until iMaxTime.
	void Update(xuint iTargetTime = 20, xuint iChunkSize = 64);

	// Get the current task being executed by the metadata.
	t_MetadataTask GetCurrentTask()
	{
		return m_iTask;
	}

	// Get the progress of the current task as a percentage (0 to 100).
	xuint GetProgress();

	// Get the global/averaged progress of the metadata processing.
	xuint GetGlobalProgress()
	{
		return (((xuint)GetCurrentTask() * 100) + GetProgress()) / 3;
	}

	// Check if the metadata has finished processing all data and is ready to be used.
	xbool IsCompleted()
	{
		return m_iTask == MetadataTask_Completed || m_iTask == MetadataTask_Error;
	}

	// Get a demetadataion of the last error that occured. Valid when GetCurrentTask() == ST_Error.
	const xchar* GetError()
	{
		return m_pError;
	}

protected:
	// Lists.
	typedef xarray<xchar*> t_lpString;

	// Load in the file contents.
	void UpdateLoad(xuint iChunkSize);

	// Decrypt the file contents.
	void UpdateDecrypt(xuint iChunkSize);

	// Tokenise the metadata.
	void UpdateTokenise();
	
	// Process the metadata into structures.
	void UpdateParse();

	// Clean up and free resources associated with the metadata.
	void Cleanup();

	// Set the error internally to report back to the user.
	void SetError(const xchar* pError);

	/**
	* System.
	*/

	// The metadata source type.
	t_MetadataSourceType m_iSourceType;

	// The current metadata task.
	t_MetadataTask m_iTask;

	// The completion status of the current metadata task (0-100).
	xuint m_iPercent;

	// The error string. Valid when m_iTask == ST_Error.
	xchar* m_pError;

	/**
	* File.
	*/

	// The file name of the metadata.
	xchar* m_pFile;

	// The handle to the file being loaded.
	HANDLE m_hFile;

	// The size of the file being loaded.
	xuint m_iFileSize;

	// The number of bytes read in so far.
	xuint m_iBytesRead;

	// The metadata data.
	xchar* m_pData;

	/**
	* Decrypt.
	*/

	// The encryption key to use to decrypt the metadata.
	xchar* m_pEncryptionKey;

	/**
	* Tokenise.
	*/

	// The current offset.
	xuint m_iTokenOffset;

	// A list of metadata tokens.
	t_lpString m_lpTokens;

	/**
	* Parse. 
	*/

	// The current token offset.
	xuint m_iTokenIndex;

	// The current dataset that we're working with.
	CDataset* m_pDataset;

	// The current property that we're working with.
	CProperty* m_pProperty;
};

//##############################################################################