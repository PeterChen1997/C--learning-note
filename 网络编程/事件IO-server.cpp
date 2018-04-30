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
	// �������
	SOCKET sockserver, newsock;
	// ��������׽����Լ�����������׽��ֵ�������
	SOCKET sockArray[WSA_MAXIMUM_WAIT_EVENTS];
	WSAEVENT eventArray[WSA_MAXIMUM_WAIT_EVENTS];
	// ������sockArray�й����Ķ��ʱ�����ľ��
	sockaddr_in addr;
	sockaddr_in clientaddr;
	char msgbuffer[MSGSIZE];

	// ���͸��ͻ��˵���Ϣ
	char msg[] = "Connect succeed. Please send a message to me. \n";
	// ��ʼ��winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0)
	{
		cerr << "Can't initialize winsock! Quitting!" << endl;
		return 0;
	}

	// �½�socket
	sockserver = socket(AF_INET, SOCK_STREAM, 0);
	if (sockserver == INVALID_SOCKET)
	{
		cerr << "Can't create a socket! Quitting" << endl;
		WSACleanup();
		return 0;
	}


	// ��ip port��socket
	int addrlen = sizeof(sockaddr_in);
	ZeroMemory((void*)&addr, addrlen); // ��սṹ��
	addr.sin_family = AF_INET;
	addr.sin_port = htons(LISTENING_PORT);
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	if (bind(sockserver, (sockaddr*)&addr, sizeof(addr)) != 0)
	{
		cerr << "��ַ��ʧ�ܣ�������룺" << WSAGetLastError() << endl;
		closesocket(sockserver);
		WSACleanup();
		return 0;
	}
	// ��ʼ����
	if (listen(sockserver, SOMAXCONN) != 0)
	{
		cerr << "listen����ʧ��!������룺" << WSAGetLastError() << endl;
		closesocket(sockserver);
		WSACleanup();
		return 0;
	}
	else
	{
		cout << "server listening...." << endl;
	}

	// Ϊ�����׽���sockserver�����¼����󣬲�ע�������¼�
	int nEventTotal = 0;
	WSAEVENT hEvent = WSACreateEvent();
	WSAEventSelect(sockserver, hEvent, FD_ACCEPT | FD_CLOSE);
	// �������׽��ּ��¼�������ӵ�������
	eventArray[nEventTotal] = hEvent;
	sockArray[nEventTotal] = sockserver;
	nEventTotal++;
	// ѭ���������������󣬲��շ�����
	int nIdx, ret;
	WSANETWORKEVENTS netEvent;
	while (true)
	{
		// �������¼������ϵȴ������¼�����
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
				// ��ȡ��Ϣ�����Զ����������¼�
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
							// ���ӳ����ر�socket event
							for (int j = 0; j < nEventTotal; j++)
							{
								closesocket(sockArray[i]);
								WSACloseEvent(eventArray[i]);
							}
							WSACleanup();
						}
						hEvent = WSACreateEvent();
						WSAEventSelect(newsock, hEvent, FD_READ | FD_CLOSE);
						// ��������socket��event obj ��ӵ���Ӧ����
						eventArray[nEventTotal] = hEvent;
						sockArray[nEventTotal] = newsock;
						// TODO:add info
						cout << inet_ntoa(clientaddr.sin_addr) << ":" << ntohs(clientaddr.sin_port) << "���ӳɹ�"   << endl;
						cout << "��ǰ��������Ϊ��" << nEventTotal << "��" << endl;
						nEventTotal++;
						send(newsock, msg, sizeof(msg), 0);
					}
				}
				else if (netEvent.lNetworkEvents & FD_READ)
				{
					// ����FD_READ֪ͨ��Ϣ
					getpeername(sockArray[i], (sockaddr*)&clientaddr, &addrlen);
					if (netEvent.iErrorCode[FD_READ_BIT] == 0)
					{
						memset((void*)msgbuffer, 0, sizeof(msgbuffer));
						int nRcv = recv(sockArray[i], msgbuffer, sizeof(msgbuffer), 0);
						if (nRcv > 0)
						{
							cout << "IPΪ" << inet_ntoa(clientaddr.sin_addr) << ":" << ntohs(clientaddr.sin_port) << "���û���" << msgbuffer << endl;
							// ������Ϣ�������û�
							for (int x = 1; x < nEventTotal; x++)
							{
								if (x == i)
								{
									continue;
								}
								cout << "�������ݸ������ˣ�������" << nEventTotal - 1 << endl;
								cout << "����Ϊ��" << msgbuffer << endl;
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
							cout << "������Ϣʧ��" << err << endl;
						}
					}
				}
				else if (netEvent.lNetworkEvents & FD_CLOSE)
				{
					// ����FD_CLOSE
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