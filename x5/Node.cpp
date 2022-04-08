#include "Node.h"
#include "Socket.h"
#include "Address.h"
#include "ByteArray.h"

#define MTU_ETHERNET 1472
#define MTU_INTERNET 548

#define MTU MTU_ETHERNET

using namespace Net;

namespace Net
{
	struct FrameInfo
	{
		UINT32 Flag : 1;
		UINT32 Size : 15;
		UINT32 Seq  : 16;
	};

	struct Package
	{
		ULONG IPv4;
		UINT16 Port;
		FrameInfo Frame;
	};

}


Node* Node::Create()
{
	Socket* socket = Socket::Create(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (socket != NULL)
	{
		Node* node = new Node();
		node->mSocket.reset(socket);
		return node;
	}
	return NULL;
}

bool Node::Bind(const AddressIPv4& To)
{
	if (mSocket)
	{
		sockaddr_in addr;
		To.ToSOCKADDR_IN(addr);
		return mSocket->Bind(&addr);
	}
	return false;
}

bool Node::SendStream(void* Data, size_t Size, const AddressIPv4& To)
{
	if (Size > 0b111111111111111) return false;
	Package* pPack = (Package*)mStreamBuffer->Alloc(sizeof(Package) + Size);
	memcpy(pPack, &To, sizeof(AddressIPv4));
	pPack->Frame.Flag = 1;
	pPack->Frame.Size = Size;
	static UINT16 Seq = 0;
	pPack->Frame.Seq = Seq++;
	memcpy(pPack + sizeof(Package), Data, Size);
	mSendQueue.Push(pPack);
}

bool Node::SendDiagram(void* Data, size_t Size, const AddressIPv4& To)
{
	if (Size > 0b111111111111111) return false;
	Package* pPack = (Package*)mDiagramBuffer->Alloc(sizeof(Package) + Size);
	memcpy(pPack, &To, sizeof(AddressIPv4));
	pPack->Frame.Flag = 0;
	pPack->Frame.Size = Size;
	pPack->Frame.Seq = 0;
	memcpy(pPack + sizeof(Package), Data, Size);
	mSendQueue.Push(pPack);
}

bool Node::PollSend()
{
	Package* pPack = mSendQueue.Pop();
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(pPack->IPv4);
	addr.sin_port = htons(pPack->Port);
	mSocket->SendTo(pPack, pPack->Frame.Size + sizeof(Package), &addr);
}

bool Node::PollRecv()
{
	BYTE Temp[MTU];
	sockaddr_in addr;
	int RecvLen = mSocket->RecvFrom(Temp, MTU, &addr);
	if (RecvLen == 0 || RecvLen == SOCKET_ERROR) return false;

}
