#pragma once

#include <memory>
#include <list>
#include "BlockQueue.h"



namespace Net
{
	class AddressIPv4;
	class Socket;
	class ByteArray;

	struct Package;

	class Node
	{
	public:
		Node() {}

		Node(const Node&) = delete;
		Node& operator=(const Node&) = delete;

		~Node() {}

		static Node* Create();

		bool Bind(const AddressIPv4& To);

		bool SendStream(void* Data, size_t Size, const AddressIPv4& To);
		bool SendDiagram(void* Data, size_t Size, const AddressIPv4& To);
	
		bool PollSend();
		bool PollRecv();
	protected:
		std::unique_ptr<Socket> mSocket;
		std::unique_ptr<ByteArray> mStreamBuffer;
		std::unique_ptr<ByteArray> mDiagramBuffer;
		std::unique_ptr<ByteArray> mRecvBuffer;
		BlockQueue<Package*> mSendQueue;
		BlockQueue<Package*> mRecvQueue;
	};
}