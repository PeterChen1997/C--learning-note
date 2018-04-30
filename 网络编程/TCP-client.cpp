#include <iostream>
#include <WS2tcpip.h>
#include <thread>

#pragma comment (lib, "ws2_32.lib")
#pragma warning(disable:4996)

using namespace std;

#define PORT 5050
#define ADDR "127.0.0.1"

void syncInfo(SOCKET socket)
{
	int size;
	char msgbuffer[5000];
	while (true)
	{
		// 接收信息并显示
		memset((void*)&msgbuffer, 0, sizeof(msgbuffer));
		size = recv(socket, msgbuffer, sizeof(msgbuffer), 0);
		if (size < 0)
		{
			cout << "接受信息失败！错误代码：" << WSAGetLastError() << endl;
			closesocket(socket);
			WSACleanup();
			break;
		}
		else if (size == 0)
		{
			cout << "server关闭连接" << endl;
			closesocket(socket);
		}
		else
		{
				cout << endl << "服务端：" << msgbuffer << endl;
		}
	}
}


int main(int argc, char** argv)
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
	SOCKET socket_client = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_client == INVALID_SOCKET)
	{
		cerr << "Can't create a socket! Quitting" << endl;
		WSACleanup();
		return 0;
	}

	// 服务器地址
	//char IP[20];
	sockaddr_in server_addr;
	int addr_len = sizeof(sockaddr_in);
	//cout << "请输入服务器IP: ";
	//cin >> IP;
	memset((void*)&server_addr, 0, addr_len); // 地址结构清0
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	//server_addr.sin_addr.S_un.S_addr = inet_addr(IP);
	server_addr.sin_addr.S_un.S_addr = inet_addr(ADDR);

	// 与服务器建立连接
	if (connect(socket_client, (sockaddr*)&server_addr, addr_len) != 0)
	{
		cout << "连接失败！错误代码：" << WSAGetLastError() << endl;
		closesocket(socket_client);
		WSACleanup();
		return 0;
	}

	int size;
	char msgbuffer[1000];
	thread	t(syncInfo, socket_client);
	t.detach();
	_sleep(500);

	while (true)
	{
		// 发送信息
		cout << "请输入要发送的信息：";
		cin >> msgbuffer;
		size = send(socket_client, msgbuffer, sizeof(msgbuffer), 0);
		if (size < 0)
		{
			cout << "发送信息失败！错误代码：" << WSAGetLastError() << endl;
		}
		else if (size == 0)
		{
			cout << "server关闭连接" << endl;
		}
		else
		{
			// cout << "信息发送成功" << msgbuffer << endl;
		}
	}

	closesocket(socket_client);
	WSACleanup();
	return 0;
}