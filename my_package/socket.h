// socket.h file
#ifndef	SOCKET_H_
#define	SOCKET_H_

//#pragma warning(disble:4251)

#ifdef _WIN32
#include <WinSock2.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd.h>	
#include <sys/socket.h>	
#include <arpa/inet.h>
#endif

#include <iostream>
#include <tuple>
#include <string>
#include <cstring>
#include <algorithm>
#include <thread>
#include <memory>
#include "util.h"
#include "../common/dll_export.h"
#include "../common/safe_queue.h"
#include "../common/network_info.h"	
#include "define.h"
#include "../ErrorCode/error_code.h"

// socket interface
namespace morai::comm {	
	// Socket interface class
	class PUBLIC_EXPORT ISocket
	{		
	public:
		virtual ~ISocket() {};
		virtual bool Open(int port) = 0;
		virtual bool Open(NetworkInfo &networkInfo) = 0;	
		virtual void Close() = 0;
	};

	// socket base class
	class PUBLIC_EXPORT SocketBase : protected ISocket
	{
	public:
		SocketBase();
		virtual ~SocketBase();
		virtual bool Open(int port) override;
		virtual bool Open(NetworkInfo &networkInfo) override; 
		virtual void Close() override;
			
		bool IsOpened() const;
	protected: // after protected
#ifdef _WIN32
		SOCKET m_socket;	
		bool m_bWsaStart; 
#else
		int m_socket;
#endif
		NetworkInfo m_networkInfo; // network info : tcp / udp and so on

	protected:

	private:
		bool m_bIsOpened;

		virtual bool OpenUDPSocket();
		virtual bool OpenTCPSocket();
	};

	// server interface
	class PUBLIC_EXPORT IServer
	{
		public:			
			virtual ~IServer() {}
			virtual morai::msg::ReturnValue Start(const char* server_ip, int server_port) = 0;
			virtual morai::msg::ReturnValue Start(AddressFamily family, int server_port) = 0;
			virtual bool Stop() = 0;
			virtual bool IsRunning() = 0;
	};

	// client interface
	class PUBLIC_EXPORT IClient
	{
		public:
			virtual ~IClient() {}
			virtual morai::msg::ReturnValue Connect(const char* server_ip, int server_port) = 0;
			virtual bool Disconnect() = 0;
			virtual bool IsRunning() = 0;			
	};

	struct PUBLIC_EXPORT DataQueueItem
	{
		DataQueueItem() : size(0), data(nullptr) {
		}

		~DataQueueItem() {
			DeletePtr(data);
		}

		int size;
		char* data;		
	};
	
	class PUBLIC_EXPORT std::thread;

	class PUBLIC_EXPORT ISocketThreadHandler
	{
	public:	
		virtual ~ISocketThreadHandler() {}
		virtual void StartThread() = 0;
		virtual void StopThread() = 0;	
		virtual void Work() = 0;	
	protected:
		float frequency;
		SafeQueue<std::shared_ptr<DataQueueItem>> DataQueue;
		std::thread thread;		
	};
}

#endif	// SOCKET_H_