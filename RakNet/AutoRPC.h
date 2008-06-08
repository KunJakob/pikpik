/// \file
/// \brief Automatically serializing and deserializing RPC system. More advanced RPC, but possibly not cross-platform
///
/// This file is part of RakNet Copyright 2003 Kevin Jenkins.
///
/// Usage of RakNet is subject to the appropriate license agreement.
/// Creative Commons Licensees are subject to the
/// license found at
/// http://creativecommons.org/licenses/by-nc/2.5/
/// Single application licensees are subject to the license found at
/// http://www.jenkinssoftware.com/SingleApplicationLicense.html
/// Custom license users are subject to the terms therein.
/// GPL license users are subject to the GNU General Public
/// License as published by the Free
/// Software Foundation; either version 2 of the License, or (at your
/// option) any later version.

#ifndef __AUTO_RPC_H
#define __AUTO_RPC_H

class RakPeerInterface;
class NetworkIDManager;
#include "PluginInterface.h"
#include "DS_Map.h"
#include "PacketPriority.h"
#include "RakNetTypes.h"
#include "BitStream.h"
#include "Gen_RPC8.h"
#include "RakString.h"

#ifdef _MSC_VER
#pragma warning( push )
#endif

/// \defgroup AUTO_RPC_GROUP AutoRPC
/// \ingroup PLUGINS_GROUP

namespace RakNet
{

/// Maximum amount of data that can be passed on the stack in a function call
#define ARPC_MAX_STACK_SIZE 65536

/// Easier way to get a pointer to a function member of a C++ class
/// \param[in] autoRPCInstance A pointer to an instance of AutoRPC
/// \param[in] _IDENTIFIER_ C string identifier to use on the remote system to call the function
/// \param[in] _RETURN_ Return value of the function
/// \param[in] _CLASS_ Base-most class of the containing class that contains your function
/// \param[in] _FUNCTION_ Name of the function
/// \param[in] _PARAMS_ Parameter list, include parenthesis
#define ARPC_REGISTER_CPP_FUNCTION(autoRPCInstance, _IDENTIFIER_, _RETURN_, _CLASS_, _FUNCTION_, _PARAMS_) \
{ \
union \
{ \
	_RETURN_ (__cdecl _CLASS_::*__memberFunctionPtr)_PARAMS_; \
	void* __voidFunc; \
}; \
	__memberFunctionPtr=&_CLASS_::_FUNCTION_; \
	(autoRPCInstance)->RegisterFunction(_IDENTIFIER_, __voidFunc, true); \
}

/// Error codes returned by a remote system as to why an RPC function call cannot execute
/// Follows packet ID ID_RPC_REMOTE_ERROR
enum RPCErrorCodes
{
	/// AutoRPC::SetNetworkIDManager() was not called, and it must be called to call a C++ object member
	RPC_ERROR_NETWORK_ID_MANAGER_UNAVAILABLE,

	/// Cannot execute C++ object member call because the object specified by SetRecipientObject() does not exist on this system
	RPC_ERROR_OBJECT_DOES_NOT_EXIST,

	/// Internal error, index optimization for function lookup does not exist
	RPC_ERROR_FUNCTION_INDEX_OUT_OF_RANGE,

	/// Named function was not registered with RegisterFunction(). Check your spelling.
	RPC_ERROR_FUNCTION_NOT_REGISTERED,

	/// Named function was registered, but later unregistered with UnregisterFunction() and can no longer be called.
	RPC_ERROR_FUNCTION_NO_LONGER_REGISTERED,

	/// SetRecipientObject() was not called before Call(), but RegisterFunction() was called with isObjectMember=true
	/// If you intended to call a CPP function, call SetRecipientObject() with a valid object first.
	RPC_ERROR_CALLING_CPP_AS_C,

	/// SetRecipientObject() was called before Call(), but RegisterFunction() was called with isObjectMember=false
	/// If you intended to call a C function, call SetRecipientObject(UNASSIGNED_NETWORK_ID) first.
	RPC_ERROR_CALLING_C_AS_CPP,

	/// Internal error, passed stack is bigger than current stack. Check that the version is the same on both systems.
	RPC_ERROR_STACK_TOO_SMALL,

	/// Internal error, formatting error with how the stack was serialized
	RPC_ERROR_STACK_DESERIALIZATION_FAILED,
};

/// The AutoRPC plugin allows you to call remote functions as if they were local functions, using the standard function call syntax
/// No serialization or deserialization is needed.
/// Advantages are that this is easier to use than regular RPC system.
/// Disadvantages is that all parameters must be passable on the stack using memcpy (shallow copy). For other types of parameters, use SetOutgoingExtraData() and GetIncomingExtraData()
/// Pointers are automatically dereferenced and the contents copied with memcpy
/// Use the old system, or regular message passing, if you need greater flexibility
/// \ingroup AUTO_RPC_GROUP
class AutoRPC : public PluginInterface
{
public:
	/// Constructor
	AutoRPC();

	/// Destructor
	virtual ~AutoRPC();

	/// Sets the network ID manager to use for object lookup
	/// Required to call C++ object member functions via SetRecipientObject()
	/// \param[in] idMan Pointer to the network ID manager to use
	void SetNetworkIDManager(NetworkIDManager *idMan);

	/// Registers a function pointer to be callable given an identifier for the pointer
	/// \param[in] uniqueIdentifier String identifying the function. Recommended that this is the name of the function
	/// \param[in] functionPtr Pointer to the function. For C, just pass the name of the function. For C++, use ARPC_REGISTER_CPP_FUNCTION
	/// \param[in] isObjectMember false if a C function. True if a member function of an object (C++)
	/// \return True on success, false on uniqueIdentifier already used
	bool RegisterFunction(const char *uniqueIdentifier, void *functionPtr, bool isObjectMember);

	/// Unregisters a function pointer to be callable given an identifier for the pointer
	/// \note This is not safe to call while connected
	/// \param[in] uniqueIdentifier String identifying the function.
	/// \param[in] isObjectMember false if a C function. True if a member function of an object (C++)
	/// \return True on success, false on function was not previously or is not currently registered.
	bool UnregisterFunction(const char *uniqueIdentifier, bool isObjectMember);
	
	/// Send or stop sending a timestamp with all following calls to Call()
	/// Use GetLastSenderTimestamp() to read the timestamp.
	/// \param[in] timeStamp Non-zero to pass this timestamp using the ID_TIMESTAMP system. 0 to clear passing a timestamp.
	void SetTimestamp(RakNetTime timeStamp);

	/// Set parameters to pass to RakPeer::Send() for all following calls to Call()
	/// Deafults to HIGH_PRIORITY, RELIABLE_ORDERED, ordering channel 0
	/// \param[in] priority See RakPeer::Send()
	/// \param[in] reliability See RakPeer::Send()
	/// \param[in] orderingChannel See RakPeer::Send()
	void SetSendParams(PacketPriority priority, PacketReliability reliability, char orderingChannel);

	/// Set system to send to for all following calls to Call()
	/// Defaults to UNASSIGNED_SYSTEM_ADDRESS, broadcast=true
	/// \param[in] systemAddress See RakPeer::Send()
	/// \param[in] broadcast See RakPeer::Send()
	void SetRecipientAddress(SystemAddress systemAddress, bool broadcast);

	/// Set the NetworkID to pass for all following calls to Call()
	/// Defaults to UNASSIGNED_NETWORK_ID (none)
	/// If set, the remote function will be considered a C++ function, e.g. an object member function
	/// If set to UNASSIGNED_NETWORK_ID (none), the remote function will be considered a C function
	/// If this is set incorrectly, you will get back either RPC_ERROR_CALLING_C_AS_CPP or RPC_ERROR_CALLING_CPP_AS_C
	/// \sa NetworkIDManager
	/// \param[in] networkID Returned from NetworkIDObject::GetNetworkID()
	void SetRecipientObject(NetworkID networkID);

	/// Write extra data to pass for all following calls to Call()
	/// Use BitStream::Reset to clear extra data. Don't forget to do this or you will waste bandwidth.
	/// \return A bitstream you can write to to send extra data with each following call to Call()
	RakNet::BitStream *SetOutgoingExtraData(void);

	/// If the last received function call has a timestamp included, it is stored and can be retrieved with this function.
	/// \return 0 if the last call did not have a timestamp, else non-zero
	RakNetTime GetLastSenderTimestamp(void) const;

	/// Returns the system address of the last system to send us a received function call
	/// Equivalent to the old system RPCParameters::sender
	/// \return Last system to send an RPC call using this system
	SystemAddress GetLastSenderAddress(void) const;

	/// Returns the instance of RakPeer this plugin was attached to
	RakPeerInterface *GetRakPeer(void) const;

	/// Returns the currently running RPC call identifier, set from RegisterFunction::uniqueIdentifier
	/// Returns an empty string "" if none
	/// \Return which RPC call is currently running
	const char *GetCurrentExecution(void) const;

	/// Gets the bitstream written to via SetOutgoingExtraData().
	/// Data is updated with each incoming function call
	/// \return A bitstream you can read from with extra data that was written with SetOutgoingExtraData();
	RakNet::BitStream *GetIncomingExtraData(void);

	/// Calls a remote function, using whatever was last passed to SetTimestamp(), SetSendParams(), SetRecipientAddress(), and SetRecipientObject()
	/// Passed parameter(s), if any, are passed via memcpy and pushed on the stack for the remote function
	/// \note This ONLY works with variables that are passable via memcpy! If you need more flexibility, use SetOutgoingExtraData() and GetIncomingExtraData()
	/// \note The this pointer, for this instance of AutoRPC, is pushed as the last parameter on the stack. See AutoRPCSample.ccp for an example of this
	bool Call(const char *uniqueIdentifier){
		char stack[ARPC_MAX_STACK_SIZE];
		char *stackPtr = (char*) stack;
		SerializeHeader(stackPtr, 0);
		return SendCall(uniqueIdentifier, stack, (unsigned int)(stackPtr-stack));
	}

	/// Calls a remote function, using whatever was last passed to SetTimestamp(), SetSendParams(), SetRecipientAddress(), and SetRecipientObject()
	/// Passed parameter(s), if any, are passed via memcpy and pushed on the stack for the remote function
	/// \note This ONLY works with variables that are passable via memcpy! If you need more flexibility, use SetOutgoingExtraData() and GetIncomingExtraData()
	/// \note The this pointer, for this instance of AutoRPC, is pushed as the last parameter on the stack. See AutoRPCSample.ccp for an example of this
	/// \param[in] es1 Endian swap parameter 1..x if necessary. Requires __BITSTREAM_NATIVE_END is undefined in RakNetDefines.h
	template <class P1>
	bool Call(const char *uniqueIdentifier, P1 p1,
		bool es1=true)	{
		char stack[ARPC_MAX_STACK_SIZE];
		char *stackPtr = (char*) stack;
		SerializeHeader(stackPtr, 1);
		PushHeader(stackPtr, p1, es1);
		Push( stackPtr, p1 );
		return SendCall(uniqueIdentifier, stack, (unsigned int)(stackPtr-stack));
	}

	
	/// Calls a remote function, using whatever was last passed to SetTimestamp(), SetSendParams(), SetRecipientAddress(), and SetRecipientObject()
	/// Passed parameter(s), if any, are passed via memcpy and pushed on the stack for the remote function
	/// \note This ONLY works with variables that are passable via memcpy! If you need more flexibility, use SetOutgoingExtraData() and GetIncomingExtraData()
	/// \note The this pointer, for this instance of AutoRPC, is pushed as the last parameter on the stack. See AutoRPCSample.ccp for an example of this
	/// \param[in] es1 Endian swap parameter 1..x if necessary. Requires __BITSTREAM_NATIVE_END is undefined in RakNetDefines.h
	template <class P1, class P2>
	bool Call(const char *uniqueIdentifier, P1 p1, P2 p2,
		bool es1=true, bool es2=true )	{
			char stack[ARPC_MAX_STACK_SIZE];
			char *stackPtr = (char*) stack;
			SerializeHeader(stackPtr, 2);
			PushHeader(stackPtr, p1, es1);
			PushHeader(stackPtr, p2, es2);
			Push( stackPtr, p1 );
			Push( stackPtr, p2 );
			return SendCall(uniqueIdentifier, stack, (unsigned int)(stackPtr-stack));
	}

	/// Calls a remote function, using whatever was last passed to SetTimestamp(), SetSendParams(), SetRecipientAddress(), and SetRecipientObject()
	/// Passed parameter(s), if any, are passed via memcpy and pushed on the stack for the remote function
	/// \note This ONLY works with variables that are passable via memcpy! If you need more flexibility, use SetOutgoingExtraData() and GetIncomingExtraData()
	/// \note The this pointer, for this instance of AutoRPC, is pushed as the last parameter on the stack. See AutoRPCSample.ccp for an example of this
	/// \param[in] es1 Endian swap parameter 1..x if necessary. Requires __BITSTREAM_NATIVE_END is undefined in RakNetDefines.h
	template <class P1, class P2, class P3>
	bool Call(const char *uniqueIdentifier, P1 p1, P2 p2, P3 p3,
		bool es1=true, bool es2=true, bool es3=true )	{
			char stack[ARPC_MAX_STACK_SIZE];
			char *stackPtr = (char*) stack;
			SerializeHeader(stackPtr, 3);
			PushHeader(stackPtr, p1, es1);
			PushHeader(stackPtr, p2, es2);
			PushHeader(stackPtr, p3, es3);
			Push( stackPtr, p1 );
			Push( stackPtr, p2 );
			Push( stackPtr, p3 );
			return SendCall(uniqueIdentifier, stack, (unsigned int)(stackPtr-stack));
	}

	/// Calls a remote function, using whatever was last passed to SetTimestamp(), SetSendParams(), SetRecipientAddress(), and SetRecipientObject()
	/// Passed parameter(s), if any, are passed via memcpy and pushed on the stack for the remote function
	/// \note This ONLY works with variables that are passable via memcpy! If you need more flexibility, use SetOutgoingExtraData() and GetIncomingExtraData()
	/// \note The this pointer, for this instance of AutoRPC, is pushed as the last parameter on the stack. See AutoRPCSample.ccp for an example of this
	/// \param[in] es1 Endian swap parameter 1..x if necessary. Requires __BITSTREAM_NATIVE_END is undefined in RakNetDefines.h
	template <class P1, class P2, class P3, class P4>
	bool Call(const char *uniqueIdentifier, P1 p1, P2 p2, P3 p3, P4 p4,
		bool es1=true, bool es2=true, bool es3=true, bool es4=true )	{
			char stack[ARPC_MAX_STACK_SIZE];
			char *stackPtr = (char*) stack;
			SerializeHeader(stackPtr, 4);
			PushHeader(stackPtr, p1, es1);
			PushHeader(stackPtr, p2, es2);
			PushHeader(stackPtr, p3, es3);
			PushHeader(stackPtr, p4, es4);
			Push( stackPtr, p1 );
			Push( stackPtr, p2 );
			Push( stackPtr, p3 );
			Push( stackPtr, p4 );
			return SendCall(uniqueIdentifier, stack, (unsigned int)(stackPtr-stack));
	}

	/// Calls a remote function, using whatever was last passed to SetTimestamp(), SetSendParams(), SetRecipientAddress(), and SetRecipientObject()
	/// Passed parameter(s), if any, are passed via memcpy and pushed on the stack for the remote function
	/// \note This ONLY works with variables that are passable via memcpy! If you need more flexibility, use SetOutgoingExtraData() and GetIncomingExtraData()
	/// \note The this pointer, for this instance of AutoRPC, is pushed as the last parameter on the stack. See AutoRPCSample.ccp for an example of this
	/// \param[in] es1 Endian swap parameter 1..x if necessary. Requires __BITSTREAM_NATIVE_END is undefined in RakNetDefines.h
	template <class P1, class P2, class P3, class P4, class P5>
	bool Call(const char *uniqueIdentifier, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5,
		bool es1=true, bool es2=true, bool es3=true, bool es4=true, bool es5=true )	{
			char stack[ARPC_MAX_STACK_SIZE];
			char *stackPtr = (char*) stack;
			SerializeHeader(stackPtr, 5);
			PushHeader(stackPtr, p1, es1);
			PushHeader(stackPtr, p2, es2);
			PushHeader(stackPtr, p3, es3);
			PushHeader(stackPtr, p4, es4);
			PushHeader(stackPtr, p5, es5);
			Push( stackPtr, p1 );
			Push( stackPtr, p2 );
			Push( stackPtr, p3 );
			Push( stackPtr, p4 );
			Push( stackPtr, p5 );
			return SendCall(uniqueIdentifier, stack, (unsigned int)(stackPtr-stack));
	}

	/// Calls a remote function, using whatever was last passed to SetTimestamp(), SetSendParams(), SetRecipientAddress(), and SetRecipientObject()
	/// Passed parameter(s), if any, are passed via memcpy and pushed on the stack for the remote function
	/// \note This ONLY works with variables that are passable via memcpy! If you need more flexibility, use SetOutgoingExtraData() and GetIncomingExtraData()
	/// \note The this pointer, for this instance of AutoRPC, is pushed as the last parameter on the stack. See AutoRPCSample.ccp for an example of this
	/// \param[in] es1 Endian swap parameter 1..x if necessary. Requires __BITSTREAM_NATIVE_END is undefined in RakNetDefines.h
	template <class P1, class P2, class P3, class P4, class P5, class P6>
	bool Call(const char *uniqueIdentifier, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6,
		bool es1=true, bool es2=true, bool es3=true, bool es4=true, bool es5=true, bool es6=true )	{
			char stack[ARPC_MAX_STACK_SIZE];
			char *stackPtr = (char*) stack;
			SerializeHeader(stackPtr, 6);
			PushHeader(stackPtr, p1, es1);
			PushHeader(stackPtr, p2, es2);
			PushHeader(stackPtr, p3, es3);
			PushHeader(stackPtr, p4, es4);
			PushHeader(stackPtr, p5, es5);
			PushHeader(stackPtr, p6, es6);
			Push( stackPtr, p1 );
			Push( stackPtr, p2 );
			Push( stackPtr, p3 );
			Push( stackPtr, p4 );
			Push( stackPtr, p5 );
			Push( stackPtr, p6 );
			return SendCall(uniqueIdentifier, stack, (unsigned int)(stackPtr-stack));
	}

	/// Calls a remote function, using whatever was last passed to SetTimestamp(), SetSendParams(), SetRecipientAddress(), and SetRecipientObject()
	/// Passed parameter(s), if any, are passed via memcpy and pushed on the stack for the remote function
	/// \note This ONLY works with variables that are passable via memcpy! If you need more flexibility, use SetOutgoingExtraData() and GetIncomingExtraData()
	/// \note The this pointer, for this instance of AutoRPC, is pushed as the last parameter on the stack. See AutoRPCSample.ccp for an example of this
	/// \param[in] es1 Endian swap parameter 1..x if necessary. Requires __BITSTREAM_NATIVE_END is undefined in RakNetDefines.h
	template <class P1, class P2, class P3, class P4, class P5, class P6, class P7>
	bool Call(const char *uniqueIdentifier, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7,
		bool es1=true, bool es2=true, bool es3=true, bool es4=true, bool es5=true, bool es6=true, bool es7=true )	{
			char stack[ARPC_MAX_STACK_SIZE];
			char *stackPtr = (char*) stack;
			SerializeHeader(stackPtr, 7);
			PushHeader(stackPtr, p1, es1);
			PushHeader(stackPtr, p2, es2);
			PushHeader(stackPtr, p3, es3);
			PushHeader(stackPtr, p4, es4);
			PushHeader(stackPtr, p5, es5);
			PushHeader(stackPtr, p6, es6);
			PushHeader(stackPtr, p7, es7);
			Push( stackPtr, p1 );
			Push( stackPtr, p2 );
			Push( stackPtr, p3 );
			Push( stackPtr, p4 );
			Push( stackPtr, p5 );
			Push( stackPtr, p6 );
			Push( stackPtr, p7 );
			return SendCall(uniqueIdentifier, stack, (unsigned int)(stackPtr-stack));
	}

	/// Calls a remote function, using whatever was last passed to SetTimestamp(), SetSendParams(), SetRecipientAddress(), and SetRecipientObject()
	/// Passed parameter(s), if any, are passed via memcpy and pushed on the stack for the remote function
	/// \note This ONLY works with variables that are passable via memcpy! If you need more flexibility, use SetOutgoingExtraData() and GetIncomingExtraData()
	/// \note The this pointer, for this instance of AutoRPC, is pushed as the last parameter on the stack. See AutoRPCSample.ccp for an example of this
	/// \param[in] es1 Endian swap parameter 1..x if necessary. Requires __BITSTREAM_NATIVE_END is undefined in RakNetDefines.h
	template <class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8>
	bool Call(const char *uniqueIdentifier, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8,
		bool es1=true, bool es2=true, bool es3=true, bool es4=true, bool es5=true, bool es6=true, bool es7=true, bool es8=true ) {
			char stack[ARPC_MAX_STACK_SIZE];
			char *stackPtr = (char*) stack;
			SerializeHeader(stackPtr, 8);
			PushHeader(stackPtr, p1, es1);
			PushHeader(stackPtr, p2, es2);
			PushHeader(stackPtr, p3, es3);
			PushHeader(stackPtr, p4, es4);
			PushHeader(stackPtr, p5, es5);
			PushHeader(stackPtr, p6, es6);
			PushHeader(stackPtr, p7, es7);
			PushHeader(stackPtr, p8, es8);
			Push( stackPtr, p1 );
			Push( stackPtr, p2 );
			Push( stackPtr, p3 );
			Push( stackPtr, p4 );
			Push( stackPtr, p5 );
			Push( stackPtr, p6 );
			Push( stackPtr, p7 );
			Push( stackPtr, p8 );
			return SendCall(uniqueIdentifier, stack, (unsigned int)(stackPtr-stack));
	}

	// If you need more than 8 parameters, just add it here...

	// ---------------------------- ALL INTERNAL AFTER HERE ----------------------------
	
	/// \internal
	/// Identifies an RPC function, by string identifier and if it is a C or C++ function
	struct RPCIdentifier
	{
		char *uniqueIdentifier;
		bool isObjectMember;
	};

	/// \internal
	/// The RPC identifier, and a pointer to the function
	struct LocalRPCFunction
	{
		RPCIdentifier identifier;
		void *functionPtr;
	};

	/// \internal
	/// The RPC identifier, and the index of the function on a remote system
	struct RemoteRPCFunction
	{
		RPCIdentifier identifier;
		unsigned int functionIndex;
	};

	/// \internal
	static int RemoteRPCFunctionComp( const RPCIdentifier &key, const RemoteRPCFunction &data );

	/// \internal
	/// Writes number of parameters to push on the stack
	static void SerializeHeader(char *&out, unsigned int numParams);

	/// \internal
	/// Writes size of each paramter to push on the stack, and endian swaps if necessary
	static void SerializeParamHeader(char *out, unsigned int paramLength, unsigned int *writeOffset, bool IsReal, bool endianSwap);

	/// \internal
	/// Writes each paramter on the stack with memcpy
	static void SerializeParamData(char *out, void *paramData, unsigned int paramLength, unsigned int *writeOffset, bool endianSwap);

	/// \internal
	/// Sends the RPC call, with a given serialized stack
	bool SendCall(const char *uniqueIdentifier, const char *stack, unsigned int bytesOnStack);

protected:

	// --------------------------------------------------------------------------------------------
	// Packet handling functions
	// --------------------------------------------------------------------------------------------
	void OnAttach(RakPeerInterface *peer);
	virtual PluginReceiveResult OnReceive(RakPeerInterface *peer, Packet *packet);
	virtual void OnAutoRPCCall(SystemAddress systemAddress, unsigned char *data, unsigned int lengthInBytes);
	virtual void OnRPCRemoteIndex(SystemAddress systemAddress, unsigned char *data, unsigned int lengthInBytes);
	virtual void OnCloseConnection(RakPeerInterface *peer, SystemAddress systemAddress);
	virtual void OnShutdown(RakPeerInterface *peer);

	void Clear(void);

	void SendError(SystemAddress target, unsigned char errorCode);
	unsigned GetLocalFunctionIndex(RPCIdentifier identifier);
	bool GetRemoteFunctionIndex(SystemAddress systemAddress, RPCIdentifier identifier, unsigned int *outerIndex, unsigned int *innerIndex);


	bool IsReal( double )      { return 1; }
	bool IsReal( float )       { return 1; }
	bool IsReal( long double ) { return 1; }

	template <class item>
	bool IsReal(item) { return 0; }

	template <class item>
	bool IsPtr( item* )        { return 1; }

	template <class item>
	bool IsPtr( item )         { return 0; }

	bool IsRakString( const RakNet::RakString rakString )         { return 1; }

	template <class item>
	bool IsRakString( item )         { return 0; }

	template <class item>
	void Push( char*& p, item const i ) {
		memcpy( (void*)p, (void*)&i, sizeof( i ) );
		p += sizeof( i );
	}
	template <class item>
	void Push( char*& p, item*const i ) {
		memcpy( (void*)p, (void*)i, sizeof( *i ) );
		p += sizeof( *i );
	}

	template <class item>
	void Push( char*& p, item const*const i ) {
		memcpy( (void*)p, (void*)i, sizeof( *i ) );
		p += sizeof( *i );
	}

	void Push( char*& p, const RakString i ) {
		size_t len = i.GetLength();
		memcpy( (void*)p, i.C_String(), len );
		p += len;
	}

	template <class item>
	size_t D_size( item const ) { return sizeof( item ); }

	template <class item>
	size_t D_size( item const*const ) { return sizeof( item ); }

	template <class item>
	size_t D_size( item*const ) { return sizeof( item ); }

	size_t D_size( const RakString item ) { return item.GetLength(); }

	template <class item>
	void PushHeader( char*& p, item const i, int endianSwap ) {
		unsigned int   s = (unsigned int) D_size( i );
		unsigned char  f =
			( IsReal( i ) << AutoRPC::REAL_PARAM_SC )
			|
			( IsPtr( i ) << AutoRPC::REF_PARAM_SC  )
			|
			( IsRakString( i ) << AutoRPC::RAKSTRING_PARAM_SC  )
			|
			( endianSwap << AutoRPC::ENDIAN_SWAP_SC  )
			;
		Push( p, s );
		Push( p, f );
	}

	enum {
		// always useful to define "shift count" (SC) or "Bit index" for bit parameters
		REAL_PARAM_SC = 0,    REAL_PARAM  = 1 << REAL_PARAM_SC,
		ENDIAN_SWAP_SC = 1,   DO_ENDIAN_SWAP = 1 << ENDIAN_SWAP_SC,
		REF_PARAM_SC  = 2,    REF_PARAM   = 1 << REF_PARAM_SC,
		RAKSTRING_PARAM_SC  = 3,   RAKSTRING_PARAM   = 1 << RAKSTRING_PARAM_SC,
	};

	// THIS STRUCTURE LAYOUT IS HARDCODED INTO THE ASSEMBLY.  Unfortunately, that appears to be the
	// only way to do it.
	struct CallParams {
#if AUTO_RPC_ABI
#if AUTO_RPC_FLOAT_REG_PARAMS
		// on most platforms, just a bool telling us whether we need any floats.
		unsigned       numRealParams;

#if AUTO_RPC_CREATE_FLOAT_MAP
		//
		// bitmask: bit(n) set indicate parameter n is a float, not an int.
		//
		unsigned       realMap;
#endif

		// N.B. these may not have type HardwareReal - they're not promoted or converted.
#if AUTO_RPC_ALLOC_SEPARATE_FLOATS
		HardwareReal   realParams[ AUTO_RPC_FLOAT_REG_PARAMS ];
#endif

#endif // AUTO_RPC_FLOAT_REG_PARAMS

		unsigned       numIntParams;
#if !AUTO_RPC_ALLOC_SEPARATE_FLOATS && AUTO_RPC_FLOAT_REG_PARAMS && AUTO_RPC_CREATE_FLOAT_MAP
		union {
			HardwareReal realParams[ AUTO_RPC_FLOAT_REG_PARAMS ];
#endif
			NaturalWord  intParams[ ( AUTO_RPC_MAX_PARAMS > AUTO_RPC_INT_REG_PARAMS ? AUTO_RPC_MAX_PARAMS : AUTO_RPC_INT_REG_PARAMS ) + AUTO_RPC_STACK_PADDING ];

#if !AUTO_RPC_ALLOC_SEPARATE_FLOATS && AUTO_RPC_FLOAT_REG_PARAMS && AUTO_RPC_CREATE_FLOAT_MAP
		};
#endif

		char      refParams[ AUTO_RPC_MAX_PARAMS * AUTO_RPC_REF_ALIGN ];
#endif // AUTO_RPC_ABI
	};

	static bool DeserializeParametersAndBuildCall(
		CallParams &call,
		char *in, unsigned int inLength,
		void *lastParam, void *thisPtr);

	static bool CallWithStack( AutoRPC::CallParams& call, void *functionPtr );

	DataStructures::List<LocalRPCFunction> localFunctions;
	DataStructures::Map<SystemAddress, DataStructures::OrderedList<RPCIdentifier, RemoteRPCFunction, AutoRPC::RemoteRPCFunctionComp> *> remoteFunctions;

	RakNetTime outgoingTimestamp;
	PacketPriority outgoingPriority;
	PacketReliability outgoingReliability;
	char outgoingOrderingChannel;
	SystemAddress outgoingSystemAddress;
	bool outgoingBroadcast;
	NetworkID outgoingNetworkID;
	RakNet::BitStream outgoingExtraData;

	RakNetTime incomingTimeStamp;
	SystemAddress incomingSystemAddress;
	RakNet::BitStream incomingExtraData;

	RakPeerInterface *rakPeer;
	NetworkIDManager *networkIdManager;
	char currentExecution[512];
};

} // End namespace

#endif

#ifdef _MSC_VER
#pragma warning( pop )
#endif
