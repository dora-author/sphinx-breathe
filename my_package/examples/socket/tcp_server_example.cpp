#include <iostream>
#include <thread>
#include <cstring>
#include <memory>
#include <set>
#include <map>

#include "define.h"
#include "tcpserver.h"
#include "util.h"

using namespace morai;
using namespace morai::comm;
using namespace morai::comm::tcp;
using namespace std;

void CallbackReceive(SOCKET_ID client_socket, char* data, int recv_size)
{
	cout << "ATCPServerActor::CallbackReceive" << endl;

	if (recv_size <= 0)
	{
		// client ���� ����.
		// Receive Error.
		cout << "Client Socket Error -- ID : " << client_socket << endl;
	}
	else
	{
		cout << "Client ID : " << client_socket << endl;
		cout << "Receive Size : " << recv_size << endl;
		cout << "Receive Data : " << data << endl;
	}
}

int main(int argc, const char* argv[])
{
	std::unique_ptr<TcpServer> tcp_server = std::make_unique<TcpServer>();
	auto callback = std::bind(&CallbackReceive, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	tcp_server->SetCallbackFunction(callback, nullptr);

	morai::msg::ReturnValue ret = tcp_server->Start(AddressFamily::Any, 8889);
	if (!ret.result)
	{
		cerr << "Failed Start TCP Server : " << ret.GetErrorCodeToString() << endl;
		return 0;
	}

	bool loop = true;
	while (loop)
	{
		string str_input;
		std::cin >> str_input;
		
		if (str_input.compare("exit") == 0)
		{
			cerr << "EXIT !!" << endl;
			loop = false;
			break;
		}
		else
		{
			// test message
			tcp_server->SendAll(str_input.c_str(), (int)str_input.size() + 1);
			//tcp_server->Send("127.0.0.1", str_input.c_str(), (int)str_input.size() + 1);
		}		
	}

	tcp_server->Stop();
	return 0;
}

/*
while(server.running()) {
	std::cout << "wait for connect" << std::endl;
	Socket* client = server.waitForConnect();

	std::thread t1 = std::thread(connectionHandler, std::ref(server), client);

	if (t1.joinable())

	{
		t1.detach();
	}
}

if (server.stop())
{
	std::cout << "server stopped" << std::endl;
}
*/