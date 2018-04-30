#include <iostream>
#include <WS2tcpip.h>
#include <string.h>

#pragma comment (lib, "ws2_32.lib")
#pragma warning(disable:4996)

using namespace std;

#define LISTENING_PORT 5050
#define MSGSIZE 5000

int main(int argc, char** argv)
{
	// 定义变量
	SOCKET sockserver, newsock;
	// 保存监听套接字以及多个已连接套接字的描述符
	SOCKET sockArray[WSA_MAXIMUM_WAIT_EVENTS];
	WSAEVENT eventArray[WSA_MAXIMUM_WAIT_EVENTS];
	// 保存与sockArray中关联的多个时间对象的句柄
	sockaddr_in addr;
	sockaddr_in clientaddr;
	char msgbuffer[MSGSIZE];

	// 发送给客户端的信息
	char msg[] = "Connect succeed. Please send a message to me. \n";
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
	sockserver = socket(AF_INET, SOCK_STREAM, 0);
	if (sockserver == INVALID_SOCKET)
	{
		cerr << "Can't create a socket! Quitting" << endl;
		WSACleanup();
		return 0;
	}


	// 绑定ip port到socket
	int addrlen = sizeof(sockaddr_in);
	ZeroMemory((void*)&addr, addrlen); // 清空结构体
	addr.sin_family = AF_INET;
	addr.sin_port = htons(LISTENING_PORT);
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	if (bind(sockserver, (sockaddr*)&addr, sizeof(addr)) != 0)
	{
		cerr << "地址绑定失败！错误代码：" << WSAGetLastError() << endl;
		closesocket(sockserver);
		WSACleanup();
		return 0;
	}
	// 开始监听
	if (listen(sockserver, SOMAXCONN) != 0)
	{
		cerr << "listen调用失败!错误代码：" << WSAGetLastError() << endl;
		closesocket(sockserver);
		WSACleanup();
		return 0;
	}
	else
	{
		cout << "server listening...." << endl;
	}

	// 为监听套接字sockserver创建事件对象，并注册网络事件
	int nEventTotal = 0;
	WSAEVENT hEvent = WSACreateEvent();
	WSAEventSelect(sockserver, hEvent, FD_ACCEPT | FD_CLOSE);
	// 将监听套接字及事件对象添加到数组中
	eventArray[nEventTotal] = hEvent;
	sockArray[nEventTotal] = sockserver;
	nEventTotal++;
	// 循环：接收连接请求，并收发数据
	int nIdx, ret;
	WSANETWORKEVENTS netEvent;
	while (true)
	{
		// 在所有事件对象上等待网络事件发生
		nIdx = WSAWaitForMultipleEvents(nEventTotal, eventArray, FALSE, WSA_INFINITE, FALSE);
		int i = nIdx - WSA_WAIT_EVENT_0;
		for (int i = nIdx; i < nEventTotal; i++)
		{
			ret = WSAWaitForMultipleEvents(1, &eventArray[i], TRUE, 1000, FALSE);
			if (ret == WSA_WAIT_FAILED || ret == WSA_WAIT_TIMEOUT)
			{
				continue;
			}
			else
			{
				WSAEnumNetworkEvents(sockArray[i], eventArray[i], &netEvent);
				// 获取消息，并自动重置受信事件
				if (netEvent.lNetworkEvents & FD_ACCEPT)
				{
					if (netEvent.iErrorCode[FD_ACCEPT_BIT] == 0)
					{
						if (nEventTotal > WSA_MAXIMUM_WAIT_EVENTS)
						{
							cout << "Too many connections!" << endl;
							continue;
						}
						newsock = accept(sockserver, (sockaddr*)&clientaddr, &addrlen);
						if (newsock == INVALID_SOCKET)
						{
							// 连接出错，关闭socket event
							for (int j = 0; j < nEventTotal; j++)
							{
								closesocket(sockArray[i]);
								WSACloseEvent(eventArray[i]);
							}
							WSACleanup();
						}
						hEvent = WSACreateEvent();
						WSAEventSelect(newsock, hEvent, FD_READ | FD_CLOSE);
						// 将已连接socket和event obj 添加到相应数组
						eventArray[nEventTotal] = hEvent;
						sockArray[nEventTotal] = newsock;
						// TODO:add info
						cout << inet_ntoa(clientaddr.sin_addr) << ":" << ntohs(clientaddr.sin_port) << "连接成功"   << endl;
						cout << "当前在线人数为：" << nEventTotal << "人" << endl;
						nEventTotal++;
						send(newsock, msg, sizeof(msg), 0);
					}
				}
				else if (netEvent.lNetworkEvents & FD_READ)
				{
					// 处理FD_READ通知信息
					getpeername(sockArray[i], (sockaddr*)&clientaddr, &addrlen);
					if (netEvent.iErrorCode[FD_READ_BIT] == 0)
					{
						memset((void*)msgbuffer, 0, sizeof(msgbuffer));
						int nRcv = recv(sockArray[i], msgbuffer, sizeof(msgbuffer), 0);
						if (nRcv > 0)
						{
							cout << "IP为" << inet_ntoa(clientaddr.sin_addr) << ":" << ntohs(clientaddr.sin_port) << "的用户：" << msgbuffer << endl;
							// 发送消息给在线用户
							for (int x = 1; x < nEventTotal; x++)
							{
								if (x == i)
								{
									continue;
								}
								cout << "发送数据给所有人，总数：" << nEventTotal - 1 << endl;
								cout << "内容为：" << msgbuffer << endl;
								send(sockArray[x], msgbuffer, sizeof(msgbuffer), 0);
							}
						}
						else if (nRcv == 0)
						{
							cout << "break when nRcv == 0";
							break;
						}
						else if (nRcv == SOCKET_ERROR)
						{
							cerr << "error in receiving message" << endl;
							break;
						}
						else
						{
							int err = WSAGetLastError();
							cout << "接收信息失败" << err << endl;
						}
					}
				}
				else if (netEvent.lNetworkEvents & FD_CLOSE)
				{
					// 处理FD_CLOSE
					if (netEvent.iErrorCode[FD_CLOSE_BIT] == 0)
					{
						closesocket(sockArray[i]);
						WSACloseEvent(eventArray[i]);
						for (int j = i; j < nEventTotal - 1; j++)
						{
							sockArray[j] = sockArray[j + 1];
							eventArray[j] = eventArray[j + 1];
						}
						nEventTotal--;
					}
				}
			}
		}
	}

	system("pause");
	return 0;
}