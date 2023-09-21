
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <thread>
#include <mutex>

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#endif

//#include "util.cpp"

#include "tcpclient.h"

using namespace morai;
using namespace morai::comm;
using namespace morai::comm::tcp;
using namespace std;

mutex _mutex;

template<typename T>
void SAFE_DELETE(T obj)
{
    if (obj != nullptr)
    {
        delete obj;
        obj = nullptr;
    }
}

void SendWhile(TcpClient* tcp_client)
{
    if (tcp_client == nullptr) return;
    
    while (true)
    {
        if (!tcp_client->IsRunning()) continue;

        string str_input;
        cin >> str_input;

        const char* input_char = str_input.c_str();

        int msg_len = (int)strlen(input_char) + 1;

        char* message = new char[msg_len];
#ifdef _WIN32
        strcpy_s(message, msg_len, input_char);
#else
        strcpy(message, input_char);
#endif

        if (strlen(input_char) == 0) continue;


        int ret_send = tcp_client->Send(message, msg_len);
        cout << "Send : " << message << endl;
        if (ret_send == -1)
        {
            cout << "Send Error" << endl;

            if (message != nullptr)
                delete[] message;
            break;
        }

        if (strcmp(message, "exit") == 0)
        {
            cout << "Input Exit" << endl;
            
            if (message != nullptr)
                delete[] message;

            break;
        }

        if(message != nullptr)
            delete[] message;
    }
}

void ReceiveWhile(TcpClient* tcp_client)
{
    bool is_exit = false;

    while (true)
    {
        if (tcp_client == nullptr)
        {
            cout << "Tcp Client is Null" << endl;
            break;
        }

        if (!tcp_client->IsRunning()) continue;

        char* recv_data = nullptr;
        int recv_size = tcp_client->Recv(recv_data);

        if (recv_data != nullptr)
        {
            cout << "Receive Message : " << recv_data << endl;

            if (strcmp(recv_data, "exit") == 0)
            {
                is_exit = true;
                
            }
            
            delete[] recv_data;

            if (is_exit)
                break;
        }
        else
        {
            break;
        }
    }
}


int main(int argc, char **argv)
{
    std::unique_ptr<TcpClient> tcp_client = std::make_unique<TcpClient>();

    const char* ip = "127.0.0.1";
    int port = 8889;

    morai::msg::ReturnValue ret = tcp_client->Connect(ip, port);
    if (!ret.result)
    {
        cerr << " connect error : " << ret.GetErrorCodeToString() << endl;
        return 0;
    }

    std::thread send_thread(SendWhile, tcp_client.get());

    std::thread recv_thread(ReceiveWhile, tcp_client.get());
    
    if(send_thread.joinable())
        send_thread.join();
    if (recv_thread.joinable())
        recv_thread.join();

    tcp_client->Disconnect();
    return 0;
}
