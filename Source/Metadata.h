#pragma once

/**
* @file Metadata.h
* @author Nat Ryall
* @date 29/01/2008
*
* Copyright © Creative Disorder
*/

#pragma region Include
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
#pragma endregion

#pragma region Predeclare
//##############################################################################
//
//                                 PREDECLARE
//
//##############################################################################
class CMetadata;

//##############################################################################
#pragma endregion

#pragma region Macros
//##############################################################################
//
//                                   MACROS
//
//##############################################################################
#define _PROPVAR \
				_pProperty

#define _DATAVAR \
				_pDataset

#define _PROPERTY_FOREACH(ITER, DATASET, NAME) \
        for (CProperty* ITER = NULL; ITER = DATASET->GetProperty(ITER, NAME);)

#define _DATASET_FOREACH(ITER, DATASET, TYPE, NAME) \
				for (CDataset* ITER = NULL; ITER = DATASET->GetDataset(ITER, TYPE, NAME);)

#define _PROPERTY_EXISTS(DATASET, PROPERTYNAME) \
				CProperty* _PROPVAR = DATASET->GetProperty(PROPERTYNAME)

#define _DATASET_EXISTS(DATASET, DATASETNAME) \
				CDataset* _DATAVAR = DATASET->GetDataset(DATASETNAME)

//##############################################################################
#pragma endregion

#pragma region Types
//##############################################################################
//
//                                   TYPES
//
//##############################################################################

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
#pragma endregion

#pragma region Declaration
//##############################################################################
//
//                                 DECLARATION
//
//##############################################################################
class CMetadata : public CDataset
{
public:
  /**
  * Constructor.
  * @param bExecute Specify true to execute the metadata synchronously in the constructor.
  */
  CMetadata(const XCHAR* pFilePath, XBOOL bExecute = true);

  /**
  * Update the metadata processing. This should be called every frame and will run up to iMaxTime milliseconds. Check IsCompleted to see if the metadata is ready to be used.
  * @param iMaxTime The maximum time, in milliseconds, allowed for the update. This keeps the metadata asychronous and fast.
  * @param iChunkSize The number of bytes to read each cycle. This number of bytes will be read in chunks until iMaxTime.
  */
  void Update(XUINT iMaxTime = 20, XUINT iChunkSize = 64);

  /**
  * Get the current task being executed by the metadata.
  */
  t_MetadataTask GetCurrentTask()
  {
    return m_iTask;
  }

  /**
  * Get the progress of the current task as a percentage (0 to 100).
  */
  XUINT GetProgress();

  /**
  * Get the global/averaged progress of the metadata processing.
  */
  XUINT GetGlobalProgress()
  {
    return (((XUINT)GetCurrentTask() * 100) + GetProgress()) / 3;
  }

  /**
  * Check if the metadata has finished processing all data and is ready to be used.
  */
  XBOOL IsCompleted()
  {
    return m_iTask == ST_Completed || m_iTask == ST_Error;
  }

  /**
  * Get a demetadataion of the last error that occured. Valid when GetCurrentTask() == ST_Error.
  */
  const XCHAR* GetError()
  {
    return m_pError;
  }

protected:
	// Lists.
	typedef XVLIST<XCHAR*> t_lpString;

  /**
  * Internal update for the metadata tasks.
  */
  void UpdateLoad(XUINT iChunkSize);
  void UpdateTokenise();
  void UpdateParse();

  /**
  * Clean up and free resources associated with the metadata.
  */
  void Cleanup();

  /**
  * Set the error internally to report back to the user.
  */
  void SetError(const XCHAR* pError);

  /**
  * System.
  */

  // The current metadata task.
  t_MetadataTask m_iTask;

  // The completion status of the current metadata task (0-100).
  XUINT m_iPercent;

  // The error string. Valid when m_iTask == ST_Error.
  XCHAR* m_pError;

  /**
  * File.
  */

  // The file name of the metadata.
  XCHAR* m_pFile;

  // The handle to the file being loaded.
  HANDLE m_hFile;

  // The size of the file being loaded.
  XUINT m_iFileSize;

  // The number of bytes read in so far.
  XUINT m_iBytesRead;

  // The metadata data.
  XCHAR* m_pData;

  /**
  * Tokenise.
  */

  // The current offset.
  XUINT m_iTokenOffset;

  // A list of metadata tokens.
  t_lpString m_lpTokens;

  /**
  * Parse. 
  */

  // The current token offset.
  XUINT m_iTokenIndex;
  
  // The current dataset that we're working with.
  CDataset* m_pDataset;

  // The current property that we're working with.
  CProperty* m_pProperty;
};

//##############################################################################
#pragma endregion