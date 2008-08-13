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
#define _PROPERTY \
		_pProperty

#define _DATASET \
		_pDataset

#define _PROPERTY_FOREACH(ITER, DATASET, NAME) \
		for (CProperty* ITER = NULL; ITER = DATASET->GetProperty(ITER, NAME);)

#define _DATASET_FOREACH(ITER, DATASET, TYPE, NAME) \
		for (CDataset* ITER = NULL; ITER = DATASET->GetDataset(ITER, TYPE, NAME);)

#define _PROPERTY_EXISTS(DATASET, PROPERTYNAME) \
		CProperty* _PROPERTY = DATASET->GetProperty(PROPERTYNAME)

#define _DATASET_EXISTS(DATASET, DATASETNAME) \
		CDataset* _DATASET = DATASET->GetDataset(DATASETNAME)

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
	MetadataSourceType_SignedEncrypted,
};

// The metadata processing tasks.
enum t_MetadataTask
{
	ST_LoadFile,            // Loading the contents of the metadata file into memory.
	ST_TokeniseData,        // Tokenising the metadata data.
	ST_ParseTokens,         // Building structures from the tokenised data.
	ST_Completed,           // All tasks have completed.
	ST_Error,               // Tasks have ceased due to an error.
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
	// ~pMetadataFile The file path to the raw metadata file.
	// ~bExecute Specify true to execute the metadata synchronously in the constructor.
	CMetadata(const xchar* pMetadataFile, xbool bExecute = true);

	// Constructor.
	// ~pMetadataFile The file path to the signed-encrypted metadata file.
	// ~pPassword The password used to encrypt the metadata.
	// ~pSignature The signature of the metadata to compare against.
	// ~pKey The public key to verify the metadata signature against.
	// ~bExecute Specify true to execute the metadata synchronously in the constructor.
	//CMetadata(const xchar* pFilePath, const xchar* pPassword, const xchar* pSignature, const xchar* pKey, xbool bExecute = true);

	// Update the metadata processing. This should be called every frame and will run up to iMaxTime milliseconds. Check IsCompleted to see if the metadata is ready to be used.
	// ~iMaxTime The maximum time, in milliseconds, allowed for the update. This keeps the metadata asychronous and fast.
	// ~iChunkSize The number of bytes to read each cycle. This number of bytes will be read in chunks until iMaxTime.
	void Update(xuint iMaxTime = 20, xuint iChunkSize = 64);

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
		return m_iTask == ST_Completed || m_iTask == ST_Error;
	}

	// Get a demetadataion of the last error that occured. Valid when GetCurrentTask() == ST_Error.
	const xchar* GetError()
	{
		return m_pError;
	}

protected:
	// Lists.
	typedef xvlist<xchar*> t_lpString;

	// Internal update for the metadata tasks.
	void UpdateLoad(xuint iChunkSize);
	void UpdateTokenise();
	void UpdateParse();

	// Clean up and free resources associated with the metadata.
	void Cleanup();

	// Set the error internally to report back to the user.
	void SetError(const xchar* pError);

	/**
	* System.
	*/

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