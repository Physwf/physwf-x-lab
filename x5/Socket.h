#pragma once

#ifdef WIN32
#include <WinSock2.h>
#else
typedef int SOCKET;
#endif

namespace Net
{
	class Socket
	{
	public:
		Socket();
		Socket(SOCKET InSocket);

		Socket(const Socket&) = delete;
		Socket& operator=(const Socket&) = delete;

		~Socket();
	public:
		static Socket* Create(int InAF, int InType, int InProtocal);
		void Close();
		bool Bind(const sockaddr_in* addr_in);
		Socket* Accept();
		bool Connect(const sockaddr_in* addr_in);
		bool Send(const void* Data, int& Size);
		bool Recv();
		int SendTo(void* InData, int InSize, const sockaddr_in* to);
		int RecvFrom(void* OutBuffer, int BufferSize, sockaddr_in* OutFrom);

		bool IsValid() const;
		SOCKET Handle() const { return hSocket; };
	private:
		SOCKET hSocket;
	};
};

