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
	// �������
	SOCKET sock_server, newsock[MAX_USER + 1];
	sockaddr_in addr;
	sockaddr_in client_addr;
	char msgbuffer[BUFFER_LEN];

	// ���͸��ͻ��˵���Ϣ
	char msg[] = "connect succeed. Please send a message to me. \n";
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
	sock_server = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_server == INVALID_SOCKET)
	{
		cerr << "Can't create a socket! Quitting" << endl;
		WSACleanup();
		return 0;
	}

	// �����׽���Ϊ������ģʽ
	unsigned long ul = 1;
	int nRet = ioctlsocket(sock_server, FIONBIO, &ul); // ����
	if (nRet == SOCKET_ERROR)
	{
		cerr << "ioctlsocket failed! error:" << WSAGetLastError() << endl;
		closesocket(sock_server);
		WSACleanup();
		return 0;
	}

	// ��ip port��socket
	int addr_len = sizeof(sockaddr_in);
	ZeroMemory((void*)&addr, addr_len); // ��սṹ��
	addr.sin_family = AF_INET;
	addr.sin_port = htons(LISTENING_PORT);
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	if (bind(sock_server, (sockaddr*)&addr, sizeof(addr)) != 0)
	{
		cerr << "��ַ��ʧ�ܣ�������룺" << WSAGetLastError() << endl;
		closesocket(sock_server);
		WSACleanup();
		return 0;
	}
	// ��ʼ����
	if (listen(sock_server, SOMAXCONN) != 0)
	{
		cerr << "listen����ʧ��!������룺" << WSAGetLastError() << endl;
		closesocket(sock_server);
		WSACleanup();
		return 0;
	}
	else
	{
		cout << "server listening...." << endl;
	}
	

	// ѭ���������������󣬲��շ�����
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
					cout << "accept����ʧ��" << endl;
					break;
				}
			}
			else
			{
				cout << "��" << inet_ntoa(client_addr.sin_addr) << "���ӳɹ�!��ǰ��Ծ����" << successLinkedCount << endl;
				while (send(newsock[successLinkedCount], msg, sizeof(msg), 0) < 0)
				{
					err = WSAGetLastError();
					if (err != WSAEWOULDBLOCK)
					{
						cout << "���ݷ���ʧ�ܣ����ӶϿ�" << endl;
						closesocket(newsock[successLinkedCount]);
						successLinkedCount--;
						break;
					}
				}
			}
		}
		send(newsock[successLinkedCount], msg, sizeof(msg), 0);
		i = 1;
		// ���Խ����������׽�������
		while (i <= successLinkedCount)
		{
			memset((void*)msgbuffer, 0, sizeof(msgbuffer));
			if (recv(newsock[i], msgbuffer, sizeof(msgbuffer), 0) < 0)
			{
				err = WSAGetLastError();
				if (err != WSAEWOULDBLOCK)
				{
					cout << "������Ϣʧ��" << err << endl;
					closesocket(newsock[successLinkedCount]);
					// ���ǵ�i��socket
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
				// ͨ����ϣ��ر�socket
				closesocket(newsock[i]);
				for (k = i; k < successLinkedCount; k++)
				{
					newsock[k] = newsock[k + 1];
				}
				successLinkedCount--;
			}
		}
	}
	// ��������
	for (i = 1; i < successLinkedCount++; i++)
	{
		closesocket(newsock[i]);
	}
	closesocket(sock_server);
	WSACleanup();

	system("pause");
	return 0;
}