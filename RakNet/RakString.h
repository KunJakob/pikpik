#ifndef __RAK_STRING_H
#define __RAK_STRING_H 

#include "Export.h"
#include "DS_List.h"
#include <stdio.h>
#include <stdarg.h>

#if defined(__GNUC__)
#define _vsnprintf vsnprintf
#endif

namespace RakNet
{
/// \brief String class
/// Has the following improvements over std::string
/// Reference counting: Suitable to store in lists
/// Varidic assignment operator
/// Doesn't cause linker errors
class RAK_DLL_EXPORT RakString
{
public:
	/// Constructors
	RakString();
	RakString(char input);
	RakString(unsigned char input);
	RakString(const unsigned char *format, ...){
		char text[8096];
		va_list ap;
		va_start(ap, format);
		_vsnprintf(text, 8096, (const char*) format, ap);
		va_end(ap);
		text[8096-1]=0;
		Assign(text);
	}
	RakString(const char *format, ...){
		char text[8096];
		va_list ap;
		va_start(ap, format);
		_vsnprintf(text, 8096, format, ap);
		va_end(ap);
		text[8096-1]=0;
		Assign(text);
	}
	~RakString();
	RakString( const RakString & rhs);

	/// Implicit return of const char*
	operator const char* () const {return sharedString->c_str;}

	/// Same as std::string::c_str
	const char *C_String(void) const {return sharedString->c_str;}

	/// Assigment operators
	RakString& operator = ( const RakString& rhs );
	RakString& operator = ( const char *str );
	RakString& operator = ( char *str );

	/// Concatenation
	RakString& operator +=( const RakString& rhs);
	RakString& operator += ( const char *str );
	RakString& operator += ( char *str );

	/// Character index. Do not use to change the string however.
	unsigned char operator[] ( const unsigned int position ) const;

	/// Equality
	bool operator==(const RakString &rhs) const;
	bool operator==(const char *str) const;
	bool operator==(char *str) const;

	/// Inequality
	bool operator!=(const RakString &rhs) const;

	/// Change all characters to lowercase
	void ToLower(void);

	/// Change all characters to uppercase
	void ToUpper(void);

	/// Set the value of the string
	void Set(const char *format, ...);

	/// Returns if the string is empty. Also, C_String() would return ""
	bool IsEmpty(void) const;

	/// Returns the length of the string
	size_t GetLength(void) const;

	/// Replace character(s) in starting at index, for count, with c
	void Replace(unsigned index, unsigned count, unsigned char c);

	/// Erase characters out of the string at index for count
	void Erase(unsigned index, unsigned count);

	/// Compare strings (case sensitive)
	int StrCmp(const RakString &rhs) const;

	/// Compare strings (not case sensitive)
	int StrICmp(const RakString &rhs) const;

	/// Clear the string
	void Clear(void);

	/// Print the string to the screen
	void Printf(void);

	/// Print the string to a file
	void FPrintf(FILE *fp);

	/// Does the given IP address match the IP address encoded into this string, accounting for wildcards?
	bool IPAddressMatch(const char *IP);
	
	/// \internal
	static size_t GetSizeToAllocate(size_t bytes)
	{
		const size_t smallStringSize = 128-sizeof(unsigned int)-sizeof(size_t)-sizeof(char*)*2;
		if (bytes<=smallStringSize)
			return smallStringSize;
		else
			return bytes*2;
	}

	/// \internal
	struct SharedString
	{
		unsigned int refCount;
		size_t bytesUsed;
		char *bigString;
		char *c_str;
		char smallString[128-sizeof(unsigned int)-sizeof(size_t)-sizeof(char*)*2];		
	};

	/// \internal
	RakString( SharedString *_sharedString );

	/// \internal
	SharedString *sharedString;

//	static SimpleMutex poolMutex;
//	static DataStructures::MemoryPool<SharedString> pool;
	/// \internal
	static SharedString emptyString;

	//static SharedString *sharedStringFreeList;
	//static unsigned int sharedStringFreeListAllocationCount;
	/// \internal
	/// List of free objects to reduce memory reallocations
	static DataStructures::List<SharedString*> freeList;

	/// Means undefined position
	static unsigned int nPos;


	static int RakStringComp( RakString const &key, RakString const &data );

protected:
	void Assign(const char *str);
	void Clone(void);
	void Free(void);
	unsigned char ToLower(unsigned char c);
	unsigned char ToUpper(unsigned char c);
	void Realloc(SharedString *sharedString, size_t bytes);
};

}

const RakNet::RakString operator+(const RakNet::RakString &lhs, const RakNet::RakString &rhs);


#endif
