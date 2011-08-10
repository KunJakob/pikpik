#pragma once

//##############################################################################
// 
//	Metadata.h
//	--------------------------------------------------------------------------
//  Brief:		Describes data using an external, script-like file.
//	Project:	Xen
//	Author:		Nat Ryall
//	Date:		29/01/2008
//	License:	Private
//	--------------------------------------------------------------------------
//	Copyright (c) SAPIAN, 2008
// 
//##############################################################################

// Common.
#include <Xen/Common.h>

// Other.
#include <Xen/Math.h>
#include <Xen/File.h>

//##############################################################################
#define XEN_METADATA_DATASET \
		_pDataset

#define XEN_METADATA_PROPERTY \
		_pProperty

#define XEN_METADATA_DATASET_FOREACH(ITER, DATASET, TYPE, NAME) \
		for (CDataset* ITER = NULL; ITER = DATASET->GetDataset(ITER, TYPE, NAME);)

#define XEN_METADATA_PROPERTY_FOREACH(ITER, DATASET, NAME) \
		for (CProperty* ITER = NULL; ITER = DATASET->GetProperty(ITER, NAME);)

#define XEN_METADATA_DATASET_EXISTS(DATASET, DATASETNAME) \
		CDataset* XEN_METADATA_DATASET = DATASET->GetDataset(DATASETNAME)

#define XEN_METADATA_PROPERTY_EXISTS(DATASET, PROPERTYNAME) \
		CProperty* XEN_METADATA_PROPERTY = DATASET->GetProperty(PROPERTYNAME)

//##############################################################################
namespace Xen
{
	// Predeclare.
	class CMetadata;
	class CDataset;

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
		MetadataTask_DecryptData,			// Decrypting the metadata (only available when Crypto++ is compiled in).
		MetadataTask_TokeniseData,			// Tokenising the metadata.
		MetadataTask_ParseTokens,			// Building structures from the tokenised data.
		MetadataTask_Completed,				// All tasks have completed.
		MetadataTask_Error,					// Tasks have ceased due to an error.
	};
}

//##############################################################################
namespace Xen
{
	class CProperty
	{
		// Friends.
		friend class CMetadata;

	public:
		// Constructor. 
		CProperty(CDataset* pParent, xchar* pType) : m_pParent(pParent), m_pType(pType) {}

		// Get the dataset this property belongs to.
		inline CDataset* GetParent()
		{
			return m_pParent;
		}

		// Get the type of the property.
		inline const xchar* GetType()
		{
			return (const xchar*)m_pType;
		}

		// Get the number of values in the property. 
		inline xint GetValueCount()
		{
			return (xint)m_lpValues.size();
		}

		// Get the property value as a char.
		// ~iIndex Specify the index offset of the value to retrieve.
		xchar GetChar(xint iIndex = 0);

		// Get the property value as an integer.
		// ~iIndex Specify the index offset of the value to retrieve.
		xint GetInt(xint iIndex = 0);

		// Get the property value as a float.
		// ~iIndex Specify the index offset of the value to retrieve.
		xfloat GetFloat(xint iIndex = 0);

		// Get the property value as a boolean.
		// ~iIndex Specify the index offset of the value to retrieve.
		xbool GetBool(xint iIndex = 0);

		// Get the property value as a string.
		// ~iIndex Specify the index offset of the value to retrieve.
		const xchar* GetString(xint iIndex = 0);

		// Get the property value as a point.
		inline xpoint GetPoint(xint iIndex = 0)
		{
			xint iOffset = iIndex * 2;
			return xpoint(GetInt(iOffset + 0), GetInt(iOffset + 1));
		}

		// Get the property value as a rect.
		inline xrect GetRect(xint iIndex = 0)
		{
			xint iOffset = iIndex * 4;
			return xrect(GetInt(iOffset + 0), GetInt(iOffset + 1), GetInt(iOffset + 2), GetInt(iOffset + 3));
		}

	protected:
		// Lists.
		typedef std::vector<xchar*> t_ValueList;

		// The parent dataset.
		CDataset* m_pParent;

		// The type of the property.
		xchar* m_pType;

		// List of values held within the property.
		t_ValueList m_lpValues;
	};
}

//##############################################################################
namespace Xen
{
	class CDataset
	{
		// Friends.
		friend class CMetadata;

	public:
		// Constructor. 
		CDataset(CDataset* pParent, const xchar* pType, const xchar* pName) : m_pParent(pParent), m_pType(pType), m_pName(pName) {}

		// Destructor.
		virtual ~CDataset();

		// Get the parent dataset. Returns NULL if called on the metadata object.
		inline CDataset* GetParent()
		{
			return m_pParent;
		}

		// Get the type of the dataset.
		inline const xchar* GetType()
		{
			return m_pType;
		}

		// Get the name of the dataset. Returns NULL if no name was specified.
		inline const xchar* GetName()
		{
			return m_pName;
		}

		// Get the total number of subsets available with the specified type.
		// ~pType The type of the dataset to count instances of. Specify NULL to count all subsets in the current scope.
		xint GetDatasetCount(const xchar* pType = NULL);

		// Get a child dataset by type. Type must be specified but name can be set to NULL if no name is specified in metadata.
		// ~return A valid dataset object or NULL if no dataset is found.
		inline CDataset* GetDataset(const xchar* pType, const xchar* pName = NULL)
		{
			return GetDataset(NULL, pType, pName);
		}

		// Get the next child dataset by type after the specified dataset. Type must be specified but name can be set to NULL if no name is specified in metadata.
		// ~pPrev The dataset to start searching from. This dataset is used for locational purposes only and is not included in the search.
		// ~return A valid dataset object or NULL if no dataset is found.
		CDataset* GetDataset(CDataset* pPrev, const xchar* pType, const xchar* pName = NULL);

		// Get a child dataset by index.
		// ~return A valid dataset object or NULL if the index is out of bounds.
		inline CDataset* GetDataset(xuint iIndex)
		{
			return (iIndex < m_lpDatasets.size()) ? m_lpDatasets[iIndex] : NULL;
		}

		// Get the total number of properties available with the specified name.
		// ~pType The type name of the property to count instances of. Specify NULL to count all properties in the current scope.
		xint GetPropertyCount(const xchar* pType = NULL);

		// Get a property by type name.
		// ~return A valid property object or NULL if no property is found.
		inline CProperty* GetProperty(const xchar* pType)
		{
			return GetProperty(NULL, pType);
		}

		// Get a property by type name after the specified property.
		// ~pPrev The property to start searching from. This property is used for locational purposes only and is not included in the search.
		// ~return A valid property object or NULL if no property is found.
		CProperty* GetProperty(CProperty* pPrev, const xchar* pType);

		// Get a child property by index.
		// ~return A valid property object or NULL if the index is out of bounds.
		inline CProperty* GetProperty(xint iIndex)
		{
			return (iIndex < (xint)m_lpProperties.size()) ? m_lpProperties[iIndex] : NULL;
		}

	protected:
		// Lists.
		typedef xarray<CDataset*> t_DatasetList;
		typedef xarray<CProperty*> t_PropertyList;

		// The parent dataset.
		CDataset* m_pParent;

		// The local dataset type.
		const xchar* m_pType;

		// The local dataset name.
		const xchar* m_pName;

		// A list of all child subsets.
		t_DatasetList m_lpDatasets;

		// A list of all properties associated with the dataset.
		t_PropertyList m_lpProperties;
	};
}

//##############################################################################
namespace Xen
{
	class CMetadata : public CDataset
	{
	public:
		// Constructor.
		// ~pMetadataFile The file path to the signed-encrypted metadata file.
		// ~pEncryptionKey The AES key used to encrypt the metadata. Specify NULL if the metadata is not encrypted.
		// ~bExecute Specify true to process the metadata synchronously in the constructor.
		// ~note 'cryptlib.h' must be included before this file to enable encryption.
		CMetadata(const xchar* pMetadataFile, const xchar* pEncryptionKey, xbool bProcessImmediately);

		// Process the next task in the metadata task chain.
		// ~return Specifies if the 'process' operation has completed.
		xbool ProcessNextTask();

		// Get the current task being executed by the metadata.
		inline t_MetadataTask GetCurrentTask()
		{
			return m_iTask;
		}

		// Check if the metadata has finished processing all data and is ready to be used.
		inline xbool IsCompleted()
		{
			return m_iTask == MetadataTask_Completed || m_iTask == MetadataTask_Error;
		}

		// Check if there was an error in the operation.
		inline xbool Failed()
		{
			return m_iTask == MetadataTask_Error;
		}

		// Get a description of the last error that occured. Valid when GetCurrentTask() == ST_Error.
		inline const xchar* GetError()
		{
			return m_pError;
		}

	protected:
		// Lists.
		typedef xarray<xchar*> t_lpString;

		// Load in the file contents.
		void Load();

		// Decrypt the file contents.
		void Decrypt();

		// Tokenise the metadata.
		void Tokenise();

		// Process the metadata into structures.
		void Parse();

		// Clean up and free resources associated with the metadata.
		void Cleanup();

		// Set the error internally to report back to the user.
		void SetError(const xchar* pError);

		// The metadata source type.
		t_MetadataSourceType m_iSourceType;

		// The current metadata task.
		t_MetadataTask m_iTask;

		// The error string. Valid when m_iTask == MetadataTask_Error.
		xchar* m_pError;

		// The file name of the metadata.
		xchar* m_pFileName;

		// The file being loaded.
		CFile* m_pFile;

		// The size of the file being loaded.
		xint m_iFileSize;

		// The metadata data.
		xchar* m_pData;

		// The encryption key to use to decrypt the metadata.
		xchar* m_pEncryptionKey;

		// A list of metadata tokens.
		t_lpString m_lpTokens;
	};
}

//##############################################################################