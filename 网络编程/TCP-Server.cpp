#include <iostream>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

#define LISTENING_PORT 5050

int main()
{
    // 初始化winsock
    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);

    int wsOk = WSAStartup(ver, &wsData);
    if (wsOk != 0)
    {
        cerr << "Can't initialize winsock! Quitting!" << endl;
        return 0;
    }

    // 新建socket
    SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == INVALID_SOCKET)
    {
        cerr << "Can't create a socket! Quitting" << endl;
        return 0;
    }

    // 绑定ip port到socket
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(LISTENING_PORT);
    hint.sin_addr.S_un.S_addr = INADDR_ANY; // could use inet_pton too...

    bind(listening, (sockaddr*)&hint, sizeof(hint));

    // 开始监听
    listen(listening, SOMAXCONN);

    // 等待连接
    sockaddr_in client;
    int clientSize = sizeof(client);
    SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
    if (clientSocket == INVALID_SOCKET)
    {
        cerr << "Can't accept clientSocket! Quitting" << endl;
        return 0;
    }

    char host[NI_MAXHOST]; // client's remote name
    char service[NI_MAXSERV]; // service the client is connect on

    ZeroMemory(host, NI_MAXHOST); // same as memset(host, 0, NI_MAXHOST);
    ZeroMemory(service, NI_MAXSERV);

    if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
    {
        cout << host << " connected on port " << service << endl;
    }
    else
    {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        cout << host << " connected on port " << ntohs(client.sin_port) << endl;
    }

    // 关闭socket
    closesocket(listening);

    // while loop: accept and echo message back to client
    char buf[4096];

    while(true)
    {
        ZeroMemory(buf, 4096);
        // wait for client to send data
        int bytesReceived = recv(clientSocket, buf, 4096, 0);
        if (bytesReceived == SOCKET_ERROR)
        {
            cerr << "Error in recv(). Quitting" << endl;
            break;
        }

        if (bytesReceived == 0)
        {
            cout << "Client disconnected " << endl;
            break;
        }

        // echo message back to client
        send(clientSocket, buf, bytesReceived + 1, 0);
    }

    // close the socket
    closesocket(clientSocket);

    // clean up winsock
    WSACleanup();

    return 0;
}