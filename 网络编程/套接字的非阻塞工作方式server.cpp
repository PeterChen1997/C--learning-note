#include <iostream>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")
#pragma warning(disable:4996)

using namespace std;

#define LISTENING_PORT 5050
#define MAX_USER 5
#define BUFFER_LEN 500

int main(int argc, char** argv)
{
	// 定义变量
	SOCKET sock_server, newsock[MAX_USER + 1];
	sockaddr_in addr;
	sockaddr_in client_addr;
	char msgbuffer[BUFFER_LEN];

	// 发送给客户端的信息
	char msg[] = "connect succeed. Please send a message to me. \n";
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
	sock_server = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_server == INVALID_SOCKET)
	{
		cerr << "Can't create a socket! Quitting" << endl;
		WSACleanup();
		return 0;
	}

	// 设置套接字为非阻塞模式
	unsigned long ul = 1;
	int nRet = ioctlsocket(sock_server, FIONBIO, &ul); // 设置
	if (nRet == SOCKET_ERROR)
	{
		cerr << "ioctlsocket failed! error:" << WSAGetLastError() << endl;
		closesocket(sock_server);
		WSACleanup();
		return 0;
	}

	// 绑定ip port到socket
	int addr_len = sizeof(sockaddr_in);
	ZeroMemory((void*)&addr, addr_len); // 清空结构体
	addr.sin_family = AF_INET;
	addr.sin_port = htons(LISTENING_PORT);
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	if (bind(sock_server, (sockaddr*)&addr, sizeof(addr)) != 0)
	{
		cerr << "地址绑定失败！错误代码：" << WSAGetLastError() << endl;
		closesocket(sock_server);
		WSACleanup();
		return 0;
	}
	// 开始监听
	if (listen(sock_server, SOMAXCONN) != 0)
	{
		cerr << "listen调用失败!错误代码：" << WSAGetLastError() << endl;
		closesocket(sock_server);
		WSACleanup();
		return 0;
	}
	else
	{
		cout << "server listening...." << endl;
	}
	

	// 循环：接收连接请求，并收发数据
	int successLinkedCount = 0;
	int err;
	int i, k;

	while (true)
	{
		if (successLinkedCount < MAX_USER)
		{
			successLinkedCount++;
			if ((newsock[successLinkedCount] = accept(sock_server, (sockaddr*)&addr, &addr_len)) == INVALID_SOCKET)
			{
				successLinkedCount--;
				err = WSAGetLastError();
				if (err != WSAEWOULDBLOCK)
				{
					cout << "accept调用失败" << endl;
					break;
				}
			}
			else
			{
				cout << "与" << inet_ntoa(client_addr.sin_addr) << "连接成功!当前活跃数：" << successLinkedCount << endl;
				while (send(newsock[successLinkedCount], msg, sizeof(msg), 0) < 0)
				{
					err = WSAGetLastError();
					if (err != WSAEWOULDBLOCK)
					{
						cout << "数据发送失败，连接断开" << endl;
						closesocket(newsock[successLinkedCount]);
						successLinkedCount--;
						break;
					}
				}
			}
		}
		send(newsock[successLinkedCount], msg, sizeof(msg), 0);
		i = 1;
		// 尝试接收已连接套接字数据
		while (i <= successLinkedCount)
		{
			memset((void*)msgbuffer, 0, sizeof(msgbuffer));
			if (recv(newsock[i], msgbuffer, sizeof(msgbuffer), 0) < 0)
			{
				err = WSAGetLastError();
				if (err != WSAEWOULDBLOCK)
				{
					cout << "接受消息失败" << err << endl;
					closesocket(newsock[successLinkedCount]);
					// 覆盖第i个socket
					for (k = i; k < successLinkedCount; k++)
					{
						newsock[k] = newsock[k + 1];
					}
					successLinkedCount--;
				}
				else
				{
					i++;
				}
			}
			else
			{
				getpeername(newsock[i], (sockaddr*)&client_addr, &addr_len);
				cout << "The message from " << inet_ntoa(client_addr.sin_addr) << ":" << msgbuffer << endl;
				// 通信完毕，关闭socket
				closesocket(newsock[i]);
				for (k = i; k < successLinkedCount; k++)
				{
					newsock[k] = newsock[k + 1];
				}
				successLinkedCount--;
			}
		}
	}
	// 结束处理
	for (i = 1; i < successLinkedCount++; i++)
	{
		closesocket(newsock[i]);
	}
	closesocket(sock_server);
	WSACleanup();

	system("pause");
	return 0;
}