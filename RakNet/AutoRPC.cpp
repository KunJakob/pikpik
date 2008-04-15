#include "AutoRPC.h"
#include "RakMemoryOverride.h"
#include "RakAssert.h"
#include "StringCompressor.h"
#include "BitStream.h"
#include "Types.h"
#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "NetworkIDObject.h"
#include "NetworkIDManager.h"
#include <stdlib.h>

using namespace RakNet;

// if stdint.h is universal and can be relied on, remove all these definitions, and use the global
// ones.
#ifdef _STDINT_H
// top tip: these types are mandated to be two's a compliment, so bitops work on them
typedef ::uint8_t             uint8_t;
typedef ::uint16_t            uint16_t;
typedef ::uint32_t            uint32_t;
typedef ::uint64_t            uint64_t;
#else
typedef unsigned char         uint8_t;
typedef unsigned short        uint16_t;
typedef unsigned int          uint32_t;
typedef unsigned long long    uint64_t;
#endif


#ifdef _MSC_VER
#pragma warning( push )
#endif

int AutoRPC::RemoteRPCFunctionComp( const RPCIdentifier &key, const RemoteRPCFunction &data )
{
	if (key.isObjectMember==false && data.identifier.isObjectMember==true)
		return -1;
	if (key.isObjectMember==true && data.identifier.isObjectMember==false)
		return 1;
	return strcmp(key.uniqueIdentifier, data.identifier.uniqueIdentifier);
}

AutoRPC::AutoRPC()
{
	currentExecution[0]=0;
	rakPeer=0;
	networkIdManager=0;
	outgoingTimestamp=0;
	outgoingPriority=HIGH_PRIORITY;
	outgoingReliability=RELIABLE_ORDERED;
	outgoingOrderingChannel=0;
	outgoingBroadcast=true;
	incomingTimeStamp=0;
	DataStructures::Map<SystemAddress, DataStructures::OrderedList<RPCIdentifier, RemoteRPCFunction, AutoRPC::RemoteRPCFunctionComp> *>::IMPLEMENT_DEFAULT_COMPARISON();
}

AutoRPC::~AutoRPC()
{
	Clear();
}
void AutoRPC::SetNetworkIDManager(NetworkIDManager *idMan)
{
	networkIdManager=idMan;
}
bool AutoRPC::RegisterFunction(const char *uniqueIdentifier, void *functionPtr, bool isObjectMember)
{
	if (uniqueIdentifier==0 || functionPtr==0)
	{
		RakAssert(0);
		return false;
	}

	RPCIdentifier identifier;
	identifier.isObjectMember=isObjectMember;
	identifier.uniqueIdentifier=(char*) uniqueIdentifier;
	unsigned localIndex = GetLocalFunctionIndex(identifier);
	// Already registered?
	if (localIndex!=(unsigned)-1 && localFunctions[localIndex].functionPtr!=0)
		return false;
	if (localIndex!=(unsigned)-1)
	{
		// Reenable existing
		localFunctions[localIndex].functionPtr=functionPtr;
	}
	else
	{
		// Add new
		LocalRPCFunction func;
		func.functionPtr=functionPtr;
		func.identifier.isObjectMember=isObjectMember;
		func.identifier.uniqueIdentifier = (char*) rakMalloc(strlen(uniqueIdentifier)+1);
		strcpy(func.identifier.uniqueIdentifier, uniqueIdentifier);
		localFunctions.Insert(func);
	}
	return true;
}
bool AutoRPC::UnregisterFunction(const char *uniqueIdentifier, bool isObjectMember)
{
	if (uniqueIdentifier==0)
	{
		RakAssert(0);
		return false;
	}

	RPCIdentifier identifier;
	identifier.isObjectMember=isObjectMember;
	identifier.uniqueIdentifier=(char*) uniqueIdentifier;
	unsigned localIndex = GetLocalFunctionIndex(identifier);
	// Not registered?
	if (localIndex==(unsigned)-1)
		return false;
	// Leave the id in, in case the function is set again later. That way we keep the same remote index
	localFunctions[localIndex].functionPtr=0;
	return true;
}
void AutoRPC::SetTimestamp(RakNetTime timeStamp)
{
	outgoingTimestamp=timeStamp;
}
void AutoRPC::SetSendParams(PacketPriority priority, PacketReliability reliability, char orderingChannel)
{
	outgoingPriority=priority;
	outgoingReliability=reliability;
	outgoingOrderingChannel=orderingChannel;
}
void AutoRPC::SetRecipientAddress(SystemAddress systemAddress, bool broadcast)
{
	outgoingSystemAddress=systemAddress;
	outgoingBroadcast=broadcast;
}
void AutoRPC::SetRecipientObject(NetworkID networkID)
{
	outgoingNetworkID=networkID;
}
RakNet::BitStream *AutoRPC::SetOutgoingExtraData(void)
{
	return &outgoingExtraData;
}
RakNetTime AutoRPC::GetLastSenderTimestamp(void) const
{
	return incomingTimeStamp;
}
SystemAddress AutoRPC::GetLastSenderAddress(void) const
{
	return incomingSystemAddress;
}
RakPeerInterface *AutoRPC::GetRakPeer(void) const
{
	return rakPeer;
}
const char *AutoRPC::GetCurrentExecution(void) const
{
	return (const char *) currentExecution;
}
RakNet::BitStream *AutoRPC::GetIncomingExtraData(void)
{
	return &incomingExtraData;
}
bool AutoRPC::SendCall(const char *uniqueIdentifier, const char *stack, unsigned int bytesOnStack)
{
	SystemAddress systemAddr;
	RPCIdentifier identifier;
	unsigned int outerIndex;
	unsigned int innerIndex;

	if (uniqueIdentifier==0)
		return false;

	identifier.uniqueIdentifier=(char*) uniqueIdentifier;
	identifier.isObjectMember=(outgoingNetworkID!=UNASSIGNED_NETWORK_ID);

	RakNet::BitStream bs;
	if (outgoingTimestamp!=0)
	{
		bs.Write((MessageID)ID_TIMESTAMP);
		bs.Write(outgoingTimestamp);
	}
	bs.Write((MessageID)ID_AUTO_RPC_CALL);
	bs.WriteCompressed(outgoingExtraData.GetNumberOfBitsUsed());
	bs.Write(&outgoingExtraData);
	int writeOffset = bs.GetWriteOffset();
	if (outgoingBroadcast)
	{
		unsigned systemIndex;
		for (systemIndex=0; systemIndex < rakPeer->GetMaximumNumberOfPeers(); systemIndex++)
		{
			systemAddr=rakPeer->GetSystemAddressFromIndex(systemIndex);
			if (systemAddr!=UNASSIGNED_SYSTEM_ADDRESS)
			{
				if (outgoingNetworkID!=UNASSIGNED_NETWORK_ID)
				{
					bs.Write(true);
					bs.Write(outgoingNetworkID);
				}
				else
				{
					bs.Write(false);
				}
				if (GetRemoteFunctionIndex(systemAddr, identifier, &outerIndex, &innerIndex))
				{
					// Write a number to identify the function if possible, for faster lookup and less bandwidth
					bs.Write(true);
					bs.WriteCompressed(remoteFunctions[outerIndex]->operator [](innerIndex).functionIndex);
				}
				else
				{
					bs.Write(false);
					stringCompressor->EncodeString(uniqueIdentifier, 512, &bs, 0);
				}

				bs.WriteCompressed(bytesOnStack);
				bs.WriteAlignedBytes((const unsigned char*) stack, bytesOnStack);
				rakPeer->Send(&bs, outgoingPriority, outgoingReliability, outgoingOrderingChannel, systemAddr, false);

				// Start writing again after ID_AUTO_RPC_CALL
				bs.SetWriteOffset(writeOffset);
			}
		}
	}
	else
	{
		systemAddr = outgoingSystemAddress;
		if (systemAddr!=UNASSIGNED_SYSTEM_ADDRESS)
		{
			if (GetRemoteFunctionIndex(systemAddr, identifier, &outerIndex, &innerIndex))
			{
				// Write a number to identify the function if possible, for faster lookup and less bandwidth
				bs.Write(true);
				bs.WriteCompressed(remoteFunctions[outerIndex]->operator [](innerIndex).functionIndex);
			}
			else
			{
				bs.Write(false);
				stringCompressor->EncodeString(uniqueIdentifier, 512, &bs, 0);
			}

			bs.WriteCompressed(bytesOnStack);
			bs.WriteAlignedBytes((const unsigned char*) stack, bytesOnStack);
			rakPeer->Send(&bs, outgoingPriority, outgoingReliability, outgoingOrderingChannel, systemAddr, false);
		}
		else
			return false;
	}
	return true;
}
void AutoRPC::OnAttach(RakPeerInterface *peer)
{
	rakPeer=peer;
	outgoingSystemAddress=UNASSIGNED_SYSTEM_ADDRESS;
	outgoingNetworkID=UNASSIGNED_NETWORK_ID;
	incomingSystemAddress=UNASSIGNED_SYSTEM_ADDRESS;

}
PluginReceiveResult AutoRPC::OnReceive(RakPeerInterface *peer, Packet *packet)
{
	RakNetTime timestamp=0;
	unsigned char packetIdentifier, packetDataOffset;
	if ( ( unsigned char ) packet->data[ 0 ] == ID_TIMESTAMP )
	{
		if ( packet->length > sizeof( unsigned char ) + sizeof( RakNetTime ) )
		{
			packetIdentifier = ( unsigned char ) packet->data[ sizeof( unsigned char ) + sizeof( RakNetTime ) ];
			memcpy(&timestamp, packet->data+sizeof(unsigned char), sizeof(RakNetTime));
			packetDataOffset=sizeof( unsigned char )*2 + sizeof( RakNetTime );
		}
		else
			return RR_STOP_PROCESSING_AND_DEALLOCATE;
	}
	else
	{
		packetIdentifier = ( unsigned char ) packet->data[ 0 ];
		packetDataOffset=sizeof( unsigned char );
	}

	switch (packetIdentifier)
	{
	case ID_DISCONNECTION_NOTIFICATION:
	case ID_CONNECTION_LOST:
		OnCloseConnection(peer, packet->systemAddress);
		return RR_CONTINUE_PROCESSING;
	case ID_AUTO_RPC_CALL:
		incomingTimeStamp=timestamp;
		incomingSystemAddress=packet->systemAddress;
		OnAutoRPCCall(packet->systemAddress, packet->data+packetDataOffset, packet->length-packetDataOffset);
		return RR_STOP_PROCESSING_AND_DEALLOCATE;
	case ID_AUTO_RPC_REMOTE_INDEX:
		OnRPCRemoteIndex(packet->systemAddress, packet->data+packetDataOffset, packet->length-packetDataOffset);
		return RR_STOP_PROCESSING_AND_DEALLOCATE;
	}

	return RR_CONTINUE_PROCESSING;
}
void AutoRPC::OnCloseConnection(RakPeerInterface *peer, SystemAddress systemAddress)
{
	(void) peer;
	if (remoteFunctions.Has(systemAddress))
	{
		DataStructures::OrderedList<RPCIdentifier, RemoteRPCFunction, AutoRPC::RemoteRPCFunctionComp> *theList = remoteFunctions.Get(systemAddress);
		unsigned i;
		for (i=0; i < theList->Size(); i++)
		{
			if (theList->operator [](i).identifier.uniqueIdentifier)
				rakFree(theList->operator [](i).identifier.uniqueIdentifier);
		}
		delete theList;
		remoteFunctions.Delete(systemAddress);
	}
}
void AutoRPC::OnAutoRPCCall(SystemAddress systemAddress, unsigned char *data, unsigned int lengthInBytes)
{
	RakNet::BitStream bs(data,lengthInBytes,false);

	int numberOfBitsUsed;
	incomingExtraData.Reset();
	bs.ReadCompressed(numberOfBitsUsed);
	if (numberOfBitsUsed > (int) incomingExtraData.GetNumberOfBitsAllocated())
		incomingExtraData.AddBitsAndReallocate(numberOfBitsUsed-(int) incomingExtraData.GetNumberOfBitsAllocated());
	bs.ReadBits(incomingExtraData.GetData(), numberOfBitsUsed, false);
	incomingExtraData.SetWriteOffset(numberOfBitsUsed);


	char inputStack[ARPC_MAX_STACK_SIZE];
//	const unsigned int outputStackSize = ARPC_MAX_STACK_SIZE+128*4; // Enough padding to round up to 4 for each parameter, max 128 parameters
//	char outputStack[outputStackSize];

	NetworkIDObject *networkIdObject;
	NetworkID networkId;
	bool hasNetworkId;
	bool hasFunctionIndex;
	unsigned int functionIndex;
	unsigned int bytesOnStack;
	char strIdentifier[512];
	bs.Read(hasNetworkId);
	if (hasNetworkId)
	{
		bs.Read(networkId);
		if (networkIdManager==0 && (networkIdManager=rakPeer->GetNetworkIDManager())==0)
		{
			// Failed - Tried to call object member, however, networkIDManager system was never registered
			SendError(systemAddress, RPC_ERROR_NETWORK_ID_MANAGER_UNAVAILABLE);
			return;
		}
		networkIdObject = (NetworkIDObject*) networkIdManager->GET_OBJECT_FROM_ID(networkId);
		if (networkIdObject==0)
		{
			// Failed - Tried to call object member, object does not exist (deleted?)
			SendError(systemAddress, RPC_ERROR_OBJECT_DOES_NOT_EXIST);
			return;
		}
	}
	else
	{
		networkIdObject=0;
	}
	bs.Read(hasFunctionIndex);
	if (hasFunctionIndex)
	{
		bs.ReadCompressed(functionIndex);

		if (functionIndex>localFunctions.Size())
		{
			// Failed - other system specified a totally invalid index
			// Possible causes: Bugs, attempts to crash the system, requested function not registered
			SendError(systemAddress, RPC_ERROR_FUNCTION_INDEX_OUT_OF_RANGE);
			return;
		}
	}
	else
	{
		stringCompressor->DecodeString(strIdentifier,512,&bs,0);

		// Find the registered function with this str
		for (functionIndex=0; functionIndex < localFunctions.Size(); functionIndex++)
		{
			if (localFunctions[functionIndex].identifier.isObjectMember == (networkIdObject!=0) &&
				strcmp(localFunctions[functionIndex].identifier.uniqueIdentifier, strIdentifier)==0)
			{
				// SEND RPC MAPPING
				RakNet::BitStream outgoingBitstream;
				outgoingBitstream.Write((MessageID)ID_AUTO_RPC_REMOTE_INDEX);
				outgoingBitstream.Write(hasNetworkId);
				outgoingBitstream.WriteCompressed(functionIndex);
				stringCompressor->EncodeString(strIdentifier,512,&outgoingBitstream,0);
				rakPeer->Send(&outgoingBitstream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, systemAddress, false);
				break;
			}
		}

		if (functionIndex==localFunctions.Size())
		{
			for (functionIndex=0; functionIndex < localFunctions.Size(); functionIndex++)
			{
				if (strcmp(localFunctions[functionIndex].identifier.uniqueIdentifier, strIdentifier)==0)
				{
					if (localFunctions[functionIndex].identifier.isObjectMember==true && networkIdObject==0)
					{
						// Failed - Calling C++ function as C function
						SendError(systemAddress, RPC_ERROR_CALLING_CPP_AS_C);
						return;
					}

					if (localFunctions[functionIndex].identifier.isObjectMember==false && networkIdObject!=0)
					{
						// Failed - Calling C++ function as C function
						SendError(systemAddress, RPC_ERROR_CALLING_C_AS_CPP);
						return;
					}
				}
			}

			SendError(systemAddress, RPC_ERROR_FUNCTION_NOT_REGISTERED);
			return;
		}
	}

	if (localFunctions[functionIndex].functionPtr==0)
	{
		// Failed - Function was previously registered, but isn't registered any longer
		SendError(systemAddress, RPC_ERROR_FUNCTION_NO_LONGER_REGISTERED);
		return;
	}

	bs.ReadCompressed(bytesOnStack);
	if (bytesOnStack > ARPC_MAX_STACK_SIZE)
	{
		// Failed - Not enough bytes on predetermined stack. Shouldn't hit this since the sender also uses this value
		SendError(systemAddress, RPC_ERROR_STACK_TOO_SMALL);
		return;
	}

	bs.ReadAlignedBytes((unsigned char *) inputStack,bytesOnStack);
//	unsigned int bytesWritten;
//	unsigned char numParameters;
//	unsigned int parameterLengths[64]; // 64 is arbitrary, just needs to be more than whatever might be serialized


	AutoRPC::CallParams call;

	if (DeserializeParametersAndBuildCall(call, inputStack, bytesOnStack, this, networkIdObject)==false)

//	if (DeserializeParametersAndBuildCall(outputStack, outputStackSize, &bytesWritten,
//		inputStack, bytesOnStack,
//		&numParameters,
//		parameterLengths, 64)==false)
	{
		// Failed - Couldn't deserialize
		SendError(systemAddress, RPC_ERROR_STACK_DESERIALIZATION_FAILED);
		return;
	}

	strncpy(currentExecution, localFunctions[functionIndex].identifier.uniqueIdentifier, sizeof(currentExecution)-1);

	if (!CallWithStack( call, localFunctions[functionIndex].functionPtr)){
		// Failed - Couldn't deserialize
		SendError(systemAddress, RPC_ERROR_STACK_DESERIALIZATION_FAILED);
		return;
	}


	currentExecution[0]=0;
}
void AutoRPC::OnRPCRemoteIndex(SystemAddress systemAddress, unsigned char *data, unsigned int lengthInBytes)
{
	// A remote system has given us their internal index for a particular function.
	// Store it and use it from now on, to save bandwidth and search time
	bool objectExists;
	char strIdentifier[512];
	unsigned int insertionIndex;
	unsigned int remoteIndex;
	RemoteRPCFunction newRemoteFunction;
	RakNet::BitStream bs(data,lengthInBytes,false);
	RPCIdentifier identifier;
	bs.Read(identifier.isObjectMember);
	bs.ReadCompressed(remoteIndex);
	stringCompressor->DecodeString(strIdentifier,512,&bs,0);
	identifier.uniqueIdentifier=strIdentifier;

	if (strIdentifier[0]==0)
		return;

	DataStructures::OrderedList<RPCIdentifier, RemoteRPCFunction, AutoRPC::RemoteRPCFunctionComp> *theList;
	if (remoteFunctions.Has(systemAddress))
	{
		theList = remoteFunctions.Get(systemAddress);
		insertionIndex=theList->GetIndexFromKey(identifier, &objectExists);
		if (objectExists==false)
		{
			newRemoteFunction.functionIndex=remoteIndex;
			newRemoteFunction.identifier.isObjectMember=identifier.isObjectMember;
			newRemoteFunction.identifier.uniqueIdentifier = (char*) rakMalloc(strlen(strIdentifier)+1);
			strcpy(newRemoteFunction.identifier.uniqueIdentifier, strIdentifier);
			theList->InsertAtIndex(newRemoteFunction, insertionIndex);
		}
	}
	else
	{
		theList = new DataStructures::OrderedList<RPCIdentifier, RemoteRPCFunction, AutoRPC::RemoteRPCFunctionComp>;

		newRemoteFunction.functionIndex=remoteIndex;
		newRemoteFunction.identifier.isObjectMember=identifier.isObjectMember;
		newRemoteFunction.identifier.uniqueIdentifier = (char*) rakMalloc(strlen(strIdentifier)+1);
		strcpy(newRemoteFunction.identifier.uniqueIdentifier, strIdentifier);
		theList->InsertAtEnd(newRemoteFunction);

		remoteFunctions.SetNew(systemAddress,theList);
	}
}

void AutoRPC::SendError(SystemAddress target, unsigned char errorCode)
{
	RakNet::BitStream bs;
	bs.Write((MessageID)ID_RPC_REMOTE_ERROR);
	bs.Write(errorCode);
	rakPeer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, target, false);
}
void AutoRPC::OnShutdown(RakPeerInterface *peer)
{
	(void) peer;
	Clear();
}
void AutoRPC::Clear(void)
{
	unsigned i,j;
	for (j=0; j < remoteFunctions.Size(); j++)
	{
		DataStructures::OrderedList<RPCIdentifier, RemoteRPCFunction, AutoRPC::RemoteRPCFunctionComp> *theList = remoteFunctions[j];
		for (i=0; i < theList->Size(); i++)
		{
			if (theList->operator [](i).identifier.uniqueIdentifier)
				rakFree(theList->operator [](i).identifier.uniqueIdentifier);
		}
		delete theList;
	}
	remoteFunctions.Clear();
	outgoingExtraData.Reset();
	incomingExtraData.Reset();
}
unsigned AutoRPC::GetLocalFunctionIndex(AutoRPC::RPCIdentifier identifier)
{
	unsigned i;
	for (i=0; i < localFunctions.Size(); i++)
	{
		if (localFunctions[i].identifier.isObjectMember==identifier.isObjectMember &&
			strcmp(localFunctions[i].identifier.uniqueIdentifier,identifier.uniqueIdentifier)==0)
			return i;
	}
	return (unsigned) -1;
}
bool AutoRPC::GetRemoteFunctionIndex(SystemAddress systemAddress, AutoRPC::RPCIdentifier identifier, unsigned int *outerIndex, unsigned int *innerIndex)
{
	bool objectExists=false;
	if (remoteFunctions.Has(systemAddress))
	{
		*outerIndex = remoteFunctions.GetIndexAtKey(systemAddress);
		DataStructures::OrderedList<RPCIdentifier, RemoteRPCFunction, AutoRPC::RemoteRPCFunctionComp> *theList = remoteFunctions[*outerIndex];
		*innerIndex = theList->GetIndexFromKey(identifier, &objectExists);
	}
	return objectExists;
}
void AutoRPC::SerializeHeader(char *&out, unsigned int numParams)
{
	*out = (char) numParams;
	out++;
	//out[*writeOffset]=(char) numParams;
	//*writeOffset+=sizeof(unsigned char);
}
void AutoRPC::SerializeParamHeader(char *out, unsigned int paramLength, unsigned int *writeOffset, bool isFloatOrDouble, bool endianSwap)
{
	memcpy(out+*writeOffset, &paramLength, sizeof(unsigned int) );
#ifndef __BITSTREAM_NATIVE_END
	if (RakNet::BitStream::IsNetworkOrder()==false)
		RakNet::BitStream::ReverseBytesInPlace((unsigned char*) (out+*writeOffset),sizeof(unsigned int));
#endif
	*writeOffset+=sizeof(unsigned int);

	*(out+*writeOffset)=0;
	if (isFloatOrDouble==true)
		*(out+*writeOffset) |= 1;
	if (endianSwap==true)
		*(out+*writeOffset) |= 2;
	*writeOffset+=1;

}

void AutoRPC::SerializeParamData(char *out, void *paramData, unsigned int paramLength, unsigned int *writeOffset, bool endianSwap)
{
	(void) endianSwap;

	memcpy(out+*writeOffset,paramData,paramLength);
#ifndef __BITSTREAM_NATIVE_END
	if (endianSwap && RakNet::BitStream::IsNetworkOrder()==false)
		RakNet::BitStream::ReverseBytesInPlace((unsigned char *) (out+*writeOffset),paramLength);
#endif
	*writeOffset+=paramLength;
}


//
// @params
//            call: [IN/OUT] workspace to build parameters that we will pass to function
//              in: [IN/OUT] is the serialized buffer - used as a temporary working for swapping
//                  parameters.
//        inLength: [IN] is the length of the above
//       lastParam: [IN] final parameter, added onto the list
//         thisPtr: [IN] if not zero - the value of this (added to start of list).
//
// @returns:
//         true: parameter list created successfully.
//        false: if deserialization fails for some reason.
//
bool AutoRPC::DeserializeParametersAndBuildCall(
	AutoRPC::CallParams &call,
	char *in, unsigned int inLength,
	void *lastParam, void *thisPtr
	) {

#if AUTO_RPC_ABI

		NaturalWord *intCallParam = call.intParams;

		char        *refParam = call.refParams;

#if AUTO_RPC_ALLOC_SEPARATE_FLOATS
		HardwareReal *realCallParam = call.realParams;
#endif

#if AUTO_RPC_CREATE_FLOAT_MAP
		call.realMap = 0;
		call.numRealParams = 0;
#endif

#if AUTO_RPC_ABI == AUTO_RPC_ABI_SYSV_AMD64
		// large structure parameters have to be bumped up here - which corresponds with the start
		// of the parameters that *are* passed on the stack.
		NaturalWord *memParam = &call.intParams[ AUTO_RPC_INT_REG_PARAMS ];
#endif

		// this is first arg - assume space ;-)
#pragma warning(disable:4311) // pointer truncation
		if ( thisPtr )
			*(intCallParam++) = reinterpret_cast<NaturalWord>( thisPtr );

		unsigned int serializedArgs = *(unsigned char*)in;

		unsigned char* header = (unsigned char*)in + 1;

		unsigned char* data = &header[ serializedArgs * ( sizeof( unsigned int ) + 1 ) ];

		// check we have the entire header in buffer
		if ( data > (unsigned char*) &in[ inLength ] )
			return 0;

		for ( unsigned int i = 0; i < serializedArgs; i++ )
		{

			// read heade entry
			int const plen = *(unsigned int*)header;
			header += sizeof( unsigned int );
			unsigned char const flags = *( header++ );

#ifndef __BITSTREAM_NATIVE_END
			if (RakNet::BitStream::DoEndianSwap())
				RakNet::BitStream::ReverseBytesInPlace( (unsigned char*)&plen , sizeof( plen ) );
#endif

			if ( !plen || data + plen > (unsigned char*)&in[ inLength ] )
				return 0;

#ifndef __BITSTREAM_NATIVE_END
			if ( flags & DO_ENDIAN_SWAP )
				RakNet::BitStream::ReverseBytesInPlace( (unsigned char*)data , plen );
#endif

			// convert pointer to ref.
			if (  ( flags & REF_PARAM )
#if AUTO_RPC_PARAMETER_REFERENCE_THRESHOLD
				|| plen > AUTO_RPC_PARAMETER_REFERENCE_THRESHOLD
#endif
				)
			{
				char *nextRefParam = refParam + AUTO_RPC__ALIGN_P2( plen, AUTO_RPC_REF_ALIGN );

				if ( nextRefParam >= AUTO_RPC_ARRAY_END( call.refParams ) || intCallParam + 1 >= AUTO_RPC_ARRAY_END( call.intParams ) )
					return 0;

				memcpy( refParam, data, plen );

#pragma warning(disable:4311) // pointer truncation
				*( intCallParam++ ) = reinterpret_cast<NaturalWord>( refParam );

				refParam = nextRefParam;

				data += plen;

				continue;
			}

			// Guarantee we have space on the output stack to accommodate the parameter.
			NaturalWord *nextParam = (NaturalWord*)( (char*)intCallParam + AUTO_RPC_ALIGN_P2( plen, NaturalWord ) );
			if ( nextParam >= AUTO_RPC_ARRAY_END( call.intParams ) )
				return 0;

#if AUTO_RPC_ALLOC_SEPARATE_FLOATS
			if ( ( flags & REAL_PARAM )
				// long doubles, of various sizes (10, 16), all get passed on the stack
				&& plen <= sizeof(double)
				// once we've allocated all our floats, they get treated as ordinary int params
				&& realCallParam < AUTO_RPC_ARRAY_END( call.realParams )
				)
			{

				if ( plen != sizeof( float ) && plen != sizeof( double ) ) {
					printf("illegal float size %d\n", plen );
					// We can't handle it - it's not a real real :lol:
					return 0;
				}

#ifdef __BIG_ENDIAN__
				memcpy( (char*)( realCallParam + 1 ) - plen, data, plen );
#else
				memcpy( (char*)realCallParam, data, plen );
#endif

#if !AUTO_RPC_INT_SHADOW_OF_FLOATS
				// we didn't use the int slot, so don't allow an advance.
				nextParam = intCallParam;
#endif

				// next time, we use the next Real slot
				realCallParam++;
			}
#if !AUTO_RPC_INT_SHADOW_OF_FLOATS
			else
#endif
#endif  // AUTO_RPC_ALLOC_SEPARATE_FLOATS
			{
				// the processor can atomically zero-extend small types, so even with the test,
				// it should be faster than memcpy+memset.
				if ( plen == 1 )
					*intCallParam = *(uint8_t*)data;    // should resolve to movzx and a move
				else if ( plen == 2 )
					*intCallParam = *(uint16_t*)data;  // if network order replace use htons(), and skip EndianSwap()
				else if ( plen == 4 )
					*intCallParam = *(uint32_t*)data;  // if network order replace use htonl(), and skip EndianSwap()
#if AUTO_RPC_AUTORPC_WORD == 64
				else if ( plen == 8 )
					*intCallParam = *(uint64_t*)data;
#endif

#if AUTO_RPC_ABI == AUTO_RPC_ABI_SYSV_AMD64
				//
				// SYSV ABI: aggregates greater 16 bytes must go on the stack;
				// in pratise, that means they can't come below AUTO_RPC_INT_REG_PARAMS when we call a function.
				//
				else if ( plen > 16 || ( plen > 8 && intCallParam == &call.intParams[ AUTO_RPC_INT_REG_PARAMS - 1] ) || ( flags & REAL_PARAM ) )
				{
					if ( intCallParam < memParam )
					{
						NaturalWord*const nextMemParam = (NaturalWord*)( (char*)memParam + AUTO_RPC_ALIGN_P2( plen, NaturalWord ) );

						if ( nextMemParam >= AUTO_RPC_ARRAY_END( call.intParams ) )
							return 0;

						memcpy( memParam, data, plen );

						// prevent advancing the ptr slot, since we didn't use it.
						nextParam = intCallParam;

						// but advance the memparam
						memParam = nextMemParam;
					}
					else
					{
						memcpy( (void*)intCallParam, data, plen );
					}
				}
#endif // AUTO_RPC_ABI_SYSV_AMD64
				else
				{
					// We don't need to worry about alignment, because any type that's not a whole multiple
					// of the natual word size will be an aggregate and that should be at the base of memory -
					// this is true for some PowerPC systems  (see [e]) but not all.  But hey, you
					// probably should be passing such structs by reference.
					//
					// Zeroing is also unecessary as code shouldn't be reading beyodn the bounds of the structure.
					//
					memcpy( (void*)intCallParam, data, plen );
				}

			}

#if AUTO_RPC_ABI == AUTO_RPC_ABI_SYSV_AMD64
			// skip over any stored "class MEMORY" (see [b]) parameters.
			if ( nextParam == &call.intParams[AUTO_RPC_INT_REG_PARAMS] )
				intCallParam = memParam;
			else
#endif
				// advence to next output param
				intCallParam = nextParam;

#if !AUTO_RPC_ALLOC_SEPARATE_FLOATS && AUTO_RPC_CREATE_FLOAT_MAP
			if ( ( flags & REAL_PARAM ) && i < AUTO_RPC_FLOAT_REG_PARAMS && ( plen == sizeof( double ) || plen == sizeof( float ) ) )
			{
				call.numRealParams++;
				call.realMap |= ( 1 << i );
			}
#endif

			// advance to next input arg.
			data += plen;
		}

		// space for lastParam?
		if ( &intCallParam[1] >= AUTO_RPC_ARRAY_END( call.intParams ) )
			return 0;

#pragma warning(disable:4311) // pointer truncation
		*( intCallParam++ ) = reinterpret_cast<NaturalWord >( lastParam );

#if AUTO_RPC_ABI == AUTO_RPC_ABI_SYSV_AMD64
		// figure out how many args we have notched up.
		if ( memParam > &call.intParams[AUTO_RPC_INT_REG_PARAMS] && memParam > intCallParam )
			intCallParam = memParam;
#endif

		// convert from ptrdif_t to unsigned int; should be small enough, even if its 64-bit pointers.
		call.numIntParams = ( unsigned int )( intCallParam - call.intParams );

#if AUTO_RPC_FLOAT_REG_PARAMS && AUTO_RPC_ALLOC_SEPARATE_FLOATS
		call.numRealParams = ( unsigned int )( realCallParam - call.realParams );
#endif

		return 1;
#else // AUTO_RPC_ABI
		return 0;
#endif

}


//
// @params
//   callParams: [IN] parameter list
//  functionPtr: [IN] function to call.
//
// @returns:
//         true: function was called.
//        false: too many parameters, probably.
//
bool AutoRPC::CallWithStack( AutoRPC::CallParams& call, void *functionPtr ) {
#if AUTO_RPC_ABI
	// Are we x86-32?
#if !defined( NO_ASM ) && ( defined(__i386__) || defined( _M_IX86 ) || defined( __INTEL__ ) )
#if !defined(__GNUC__)
	// Intel dialect assembly
	NaturalWord const paramc = call.numIntParams;
#pragma warning(disable:4311) // pointer truncation
	NaturalWord const paramv = reinterpret_cast<NaturalWord>( call.intParams );
	_asm
	{

			// Load numbytes.
			mov         ecx, paramc

			// allocate space on the stack for all these params
			lea         edi,dword ptr[ecx * 4]
			sub         esp,edi

			// setup source of copy
			mov         esi, paramv

			// Setup the destination of the copy: the return stack.
			mov         edi,esp

			// copy data
			rep movs    dword ptr es:[edi],dword ptr [esi]

			// call the function
			call        functionPtr

			// Restore the stack to its state, prior to our invocation.
			//
			// Detail: edi is one of the registers that must be preserved
			// across function calls. (The compiler should be saving it for us.)
			//
			// We left edi pointing to the end of the block copied; i.e. the state
			// of the stack prior to copying our params.  So by loading it
			// into the esp we can restore the return stack to the state prior
			// to the copy.
			//
			mov         esp,edi
	};
#else
	// GCC has its own form of asm block - so we'll always have to write two versions.
	// Therefore, as we're writing it twice, we use the ATT dialect, because only later
	// gcc support Intel dialect.  This one also aligns the stack to a multiple of 16 bytes; which
	// windows doesn't seem to care about.
	// Be aware, we can't use offset of to get the address, as gcc insists on sticking.
	// NaturalWord const paramv = reinterpret_cast<NaturalWord>( call.intParams );
	asm (\
		"lea    4(%%ecx),%%esi\n\
		mov    (%%ecx),%%ecx\n\
		lea    (,%%ecx,4),%%edi\n\
		sub    %%edi,%%esp\n\
		mov    $12,%%edx\n\
		and    %%esp,%%edx\n\
		sub    %%edx,%%esp\n\
		mov    %%esp,%%edi\n\
		rep movsl %%ds:(%%esi),%%es:(%%edi)\n\
		add    %%edx,%%edi\n\
		call   *%1\n\
		mov    %%edi,%%esp"\
		: /* no outputs */\
		: "c" ( &call ), "m" (functionPtr)\
		: "%edi" , "%esi", "%edx", "%eax"\
		);
#endif  // GNUC vs non GNUC
	return 1;
#elif !defined( NO_ASM ) && ( defined( _M_X64 ) || defined( __x86_64__ ) || defined( _M_AMD64 ) )
#if AUTO_RPC_ABI == AUTO_RPC_ABI_WIN_AMD64
	NaturalWord const paramv = reinterpret_cast<NaturalWord>( call.intParams );
	ASSEMBLY_BLOCK {
		// rcx := number of qwords to copy
		mov         rcx, paramc

			// r9 := 0
			sub         r9,r9

			// rsi => our parameter list.
			mov         rsi, paramv

			// r9 := -(number of qwords to copy)
			sub         r9,rcx

			// Preparation to align the stack to 16 byte boundary
			mov         rdx,8

			// rdi => projected bottom of stack
			lea         rdi,dword ptr[rsp + r9 * 8]

		// figure out if stack needs aligning
		and         rdx,rdi

			// save stack into rbx
			mov         rbx,rsp

			// align stack
			sub         rdi,rdx
			mov         rsp,rdi

			// rdx => our parameter list
			mov         rdx,rsi

			//
			// copy data - we copy all parameters, because we have to
			// create a shadow area; and this way its easiest.
			//
			rep movs     qword ptr es:[edi],qword ptr [esi]

		// load registers
		// rcx|xmm0, rdx|xmm1,r8|xmm2,r9|xmm3
		mov          rcx,qword ptr [rdx]
		mov          r8,qword ptr 16[rdx]
		movq         xmm0,rcx
			mov          r9,qword ptr 24[rdx]
		movq         xmm2,r8
			mov          rdx,qword ptr 8[rdx]
		movq         xmm3,r9
			movq         xmm1,rdx

			// call the function
			call        functionPtr

			// Restore the stack to its state, prior to our invocation -
			// saved in rbx.
			mov         rsp,rbx
	}
#elif AUTO_RPC_ABI == AUTO_RPC_ABI_SYSV_AMD64
	//
	// GCC won't generate a stack frame on higher optimization levels, so we don't touch it.
	// on -O3 it inlines the code, breaking it because of the jump reference.
	//
	// I figure a 64-bit compiler will be recent enough to do Intel syntax. May need to change
	// my mind on that. NB. Structure members are hard coded into this.
	//
	asm (\
		".intel_syntax noprefix\n\
		push        rbx\n\
		mov         rax,rsi\n\
		push        r15\n\
		mov         ecx,dword ptr[rdi+8+8*8]\n\
		lea         rsi,[rdi+8+8*8+8]\n\
		mov         r15,rsp\n\
		lea         rbx,[rdi+8]\n\
		sub         r8,r8\n\
		sub         rcx,6\n\
		lea         r9,[rsi + 6 * 8]\n\
		jbe         .L1\n\
		sub         r8,rcx\n\
		mov         rdx,8\n\
		lea         rdi,qword ptr[rsp + r8 * 8]\n\
		and         rdx,rdi\n\
		mov         rsi,r9\n\
		sub         rdi,rdx\n\
		mov         rsp,rdi\n\
		rep movsq   \n\
		.L1:\n\
		movq         xmm0,[rbx]\n\
		movq         xmm1,[rbx+8]\n\
		movq         xmm2,[rbx+16]\n\
		movq         xmm3,[rbx+24]\n\
		movq         xmm4,[rbx+32]\n\
		movq         xmm5,[rbx+40]\n\
		movq         xmm6,[rbx+48]\n\
		movq         xmm7,[rbx+56]\n\
		mov          rdi,[r9-48]\n\
		mov          rsi,[r9-40]\n\
		mov          rdx,[r9-32]\n\
		mov          rcx,[r9-24]\n\
		mov          r8,[r9-16]\n\
		mov          r9,[r9-8]\n\
		call         rax\n\
		mov          rsp,r15\n\
		pop          r15\n\
		pop          rbx\n\
		.att_syntax prefix"\
		: /* no outputs */\
		: "D" ( &call ), "S" (functionPtr)\
		: "%rdx", "%rcx" , "%r8", "%r9", "%rax",\
		"%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7" );
	// : "D", ( &call ), "c" ( &call.numIntParams ), "S" ( paramv ), "b" ( floatv ), "a" (functionPtr)
#else
#error unsupport ABI
#endif
	return 1;
#else
	// NO_ASM or no x86-32/x86-64
	//
	//  4.  Passing the parameters.
	//
	//  The compiler knows how to call functions, so having sorted out the argument list,
	//  we just pass it to a function of the correct form - and let the compiler align stacks,
	//  load registers, place parameters where they should be.
	//
	//  This is particularly important as GCC has control over the stack frame, and it can
	//  improperly frame it - for instance utilising red zones to save args, rather than pushing them.
	//  On PowerPC it must create the parameter passing area, too.
	//
	//  The most brute force way, is to code a function call for every possible number of parameters
	//
	//  switch( paramc ) {
	//        case 1:   ( (void(AUTO_RPC_CALLSPEC*)(NaturalWord)) myfunc)( callParam[0] ); break;
	//        case 2:   ( (void(AUTO_RPC_CALLSPEC*)(NaturalWord,NaturalWord)) myfunc)( callParam[0], callParam[1] ); break;
	//        ...
	//        case 64:  ( (void(AUTO_RPC_CALLSPEC*)(NaturalWord,NaturalWord)) myfunc)( callParam[0], callParam[1], ... , callParam[63] ); break;
	//  }
	//
	//  This is the only way to code WIN32 stdcall, for example, as the args must match exactly;
	//  and so the only way to call from C if you need to call WINAPI routines.
	//
	//  2) Fortunately most calling conventions allowing excessive args. So this means we could
	//  write something like below:
	//
	//     ( (void(*)(...)) myfunc )( args[0],...,args[63] );
	//
	//  And although this should work, its a huge performance penalty copying between memory
	//  locations for so many args.
	//
	//  So we compromise - and do a stepped sequence. Noticing that the WIN64 ABI alwys requires
	//  space for three args anyway.
	//
	//  And on SysV x64 systems, the first 6 args are passed in reg; so again, these are only
	//  copies into register, not memory copies.  And finally that if we load word[n], word[n+1]
	//  is loaded into the cache - thus the overhead for loading is not as big as it might be.
	//
	//  For most realistic cases, a dozen args would be excessive.  Obviously, if you have
	//  a tested assembler equivalent, its probably better to use that.
	//
	//
#if AUTO_RPC_FLOAT_REG_PARAMS
	if ( call.numRealParams == 0 )
#endif
	{
		if ( call.numIntParams <= 3 )
		{
			( (void(AUTO_RPC_CALLSPEC*)(AUTO_RPC_NW_3)) functionPtr )( AUTO_RPC_INT_ARGS_3(  call ) );
			return 1;
		}
		if ( call.numIntParams <= 6 )
		{
			( (void(AUTO_RPC_CALLSPEC*)(AUTO_RPC_NW_6)) functionPtr )( AUTO_RPC_INT_ARGS_6( call ) );
			return 1;
		}
		if ( call.numIntParams <= 9 )
		{
			((void(AUTO_RPC_CALLSPEC*)(AUTO_RPC_NW_9))functionPtr)( AUTO_RPC_INT_ARGS_9( call ) );
			return 1;
		}
		if ( call.numIntParams <= 12 )
		{
			((void(AUTO_RPC_CALLSPEC*)(AUTO_RPC_NW_12))functionPtr)( AUTO_RPC_INT_ARGS_12( call ) );
			return 1;
		}
		if ( call.numIntParams <= 32 )
		{
			((void(AUTO_RPC_CALLSPEC*)(AUTO_RPC_NW_32))functionPtr)( AUTO_RPC_INT_ARGS_32( call ) );
			return 1;
		}
		if ( call.numIntParams <= 64 )
		{
			((void(AUTO_RPC_CALLSPEC*)(AUTO_RPC_NW_64))functionPtr)( AUTO_RPC_INT_ARGS_64( call ) );
			return 1;
		}
	}
#if AUTO_RPC_FLOAT_REG_PARAMS && !AUTO_RPC_ALLOC_SEPARATE_FLOATS
	else
	{
		if ( call.numIntParams > 64 ) return 0;

		switch( call.realMap )
		{
			// case 0: - no floats, never happens here.

		case 1: ( (void(AUTO_RPC_CALLSPEC*)(HardwareReal,NaturalWord,NaturalWord,NaturalWord,AUTO_RPC_NW_4_64))functionPtr)(
					call.realParams[0], call.intParams[1], call.intParams[2], call.intParams[3],
					AUTO_RPC_INT_ARGS_4_64( call )
					);
			break;

		case 2:
			((void(AUTO_RPC_CALLSPEC*)(NaturalWord,HardwareReal,NaturalWord,NaturalWord,AUTO_RPC_NW_4_64))functionPtr)(
				call.intParams[0], call.realParams[1], call.intParams[2], call.intParams[3],
				AUTO_RPC_INT_ARGS_4_64( call )
				);
			break;

		case 3:
			((void(AUTO_RPC_CALLSPEC*)(HardwareReal,HardwareReal,NaturalWord,NaturalWord,AUTO_RPC_NW_4_64))functionPtr)(
				call.realParams[0], call.realParams[1], call.intParams[2], call.intParams[3],
				AUTO_RPC_INT_ARGS_4_64( call )
				);
			break;

		case 4: ( (void(AUTO_RPC_CALLSPEC*)(NaturalWord,NaturalWord,HardwareReal,NaturalWord,AUTO_RPC_NW_4_64))functionPtr)(
					call.intParams[0], call.intParams[1], call.realParams[2], call.intParams[3],
					AUTO_RPC_INT_ARGS_4_64( call )
					);
			break;

		case 5:
			((void(AUTO_RPC_CALLSPEC*)(HardwareReal,NaturalWord,HardwareReal,NaturalWord,AUTO_RPC_NW_4_64))functionPtr)(
				call.realParams[0], call.intParams[1], call.realParams[2], call.intParams[3],
				AUTO_RPC_INT_ARGS_4_64( call )
				);
			break;

		case 6:
			((void(AUTO_RPC_CALLSPEC*)(NaturalWord,HardwareReal,HardwareReal,NaturalWord,AUTO_RPC_NW_4_64))functionPtr)(
				call.intParams[0], call.realParams[1], call.realParams[2], call.intParams[3],
				AUTO_RPC_INT_ARGS_4_64( call )
				);
			break;

		case 7:
			((void(AUTO_RPC_CALLSPEC*)(HardwareReal,HardwareReal,HardwareReal,NaturalWord,AUTO_RPC_NW_4_64))functionPtr)(
				call.realParams[0], call.realParams[1], call.realParams[2], call.intParams[3],
				AUTO_RPC_INT_ARGS_4_64( call )
				);
			break;

		case 8: ( (void(AUTO_RPC_CALLSPEC*)(NaturalWord,NaturalWord,NaturalWord,HardwareReal,AUTO_RPC_NW_4_64))functionPtr)(
					call.intParams[0], call.intParams[1], call.intParams[2], call.realParams[3],
					AUTO_RPC_INT_ARGS_4_64( call )
					);
			break;

		case 9:
			((void(AUTO_RPC_CALLSPEC*)(HardwareReal,NaturalWord,NaturalWord,HardwareReal,AUTO_RPC_NW_4_64))functionPtr)(
				call.realParams[0], call.intParams[1], call.intParams[2], call.realParams[3],
				AUTO_RPC_INT_ARGS_4_64( call )
				);
			break;
		case 10:
			((void(AUTO_RPC_CALLSPEC*)(NaturalWord,HardwareReal,NaturalWord,HardwareReal,AUTO_RPC_NW_4_64))functionPtr)(
				call.intParams[0], call.realParams[1], call.intParams[2], call.realParams[3],
				AUTO_RPC_INT_ARGS_4_64( call )
				);
			break;


		case 11:
			((void(AUTO_RPC_CALLSPEC*)(HardwareReal,HardwareReal,NaturalWord,HardwareReal,AUTO_RPC_NW_4_64))functionPtr)(
				call.realParams[0], call.realParams[1], call.intParams[2], call.realParams[3],
				AUTO_RPC_INT_ARGS_4_64( call )
				);
			break;

		case 12: ( (void(AUTO_RPC_CALLSPEC*)(NaturalWord,NaturalWord,HardwareReal,HardwareReal,AUTO_RPC_NW_4_64))functionPtr)(
					 call.intParams[0], call.intParams[1], call.realParams[2], call.realParams[3],
					 AUTO_RPC_INT_ARGS_4_64( call )
					 );
			break;

		case 13:
			((void(AUTO_RPC_CALLSPEC*)(HardwareReal,NaturalWord,HardwareReal,HardwareReal,AUTO_RPC_NW_4_64))functionPtr)(
				call.realParams[0], call.intParams[1], call.realParams[2], call.realParams[3],
				AUTO_RPC_INT_ARGS_4_64( call )
				);
			break;

		case 14:
			((void(AUTO_RPC_CALLSPEC*)(NaturalWord,HardwareReal,HardwareReal,HardwareReal,AUTO_RPC_NW_4_64))functionPtr)(
				call.intParams[0], call.realParams[1], call.realParams[2], call.realParams[3],
				AUTO_RPC_INT_ARGS_4_64( call )
				);
			break;

		case 15:
			((void(AUTO_RPC_CALLSPEC*)(HardwareReal,HardwareReal,HardwareReal,HardwareReal,AUTO_RPC_NW_4_64))functionPtr)(
				call.realParams[0], call.realParams[1], call.realParams[2], call.realParams[3],
				AUTO_RPC_INT_ARGS_4_64( call )
				);
			break;

		default: return 0;
		}
	}
#elif AUTO_RPC_FLOAT_REG_PARAMS
	else
	{
		// we pass FLOAT args last for powerpc compatibility. And although it means we pass them twice,
		// they should end up in the correct floating point register, with the rest of the integers in the
		// correct place...
		//
		// NB if you want to write inline asm for powerpc, you'll have to be put it in a separate
		// "naked" function to that uou can setup the parameter passing area and ensure its big enough.
		// (GCC will delete functions that are unused - it will delete the body of functions that
		// aren't called.)
		//
		if ( call.numIntParams <= 3 )
		{
			((void(AUTO_RPC_CALLSPEC*)(AUTO_RPC_NW_3,AUTO_RPC_FLOAT_REG_TYPE))functionPtr)( AUTO_RPC_INT_ARGS_3( call ), AUTO_RPC_FLOAT_REG_ARGS( call ) );
			return 1;
		}
		if ( call.numIntParams <= 6 )
		{
			((void(AUTO_RPC_CALLSPEC*)(AUTO_RPC_NW_6,AUTO_RPC_FLOAT_REG_TYPE))functionPtr)( AUTO_RPC_INT_ARGS_6( call ),AUTO_RPC_FLOAT_REG_ARGS( call ) );
			return 1;
		}
		if ( call.numIntParams <= 9 )
		{
			((void(AUTO_RPC_CALLSPEC*)(AUTO_RPC_NW_9,AUTO_RPC_FLOAT_REG_TYPE))functionPtr)( AUTO_RPC_INT_ARGS_9( call ),AUTO_RPC_FLOAT_REG_ARGS( call ) );
			return 1;
		}
		if ( call.numIntParams <= 12 )
		{
			( (void(AUTO_RPC_CALLSPEC*)(AUTO_RPC_NW_12,AUTO_RPC_FLOAT_REG_TYPE)) functionPtr )( AUTO_RPC_INT_ARGS_12( call ), AUTO_RPC_FLOAT_REG_ARGS( call ) );
			return 1;
		}
		if ( call.numIntParams <= 64 )
		{
			( (void(AUTO_RPC_CALLSPEC*)(AUTO_RPC_NW_64,AUTO_RPC_FLOAT_REG_TYPE)) functionPtr )( AUTO_RPC_INT_ARGS_64( call ), AUTO_RPC_FLOAT_REG_ARGS( call ) );
			return 1;
		}
	}
#endif   // AUTO_RPC_FLOAT_REG_PARAMS
	return 0;
#endif   // NO_ASM
#else  // AUTO_RPC_ABI
	return 0;
#endif
}
// --8<---8<----8<----8<---END

#ifdef _MSC_VER
#pragma warning( pop )
#endif

