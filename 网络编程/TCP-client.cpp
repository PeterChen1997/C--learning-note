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
		// ������Ϣ����ʾ
		memset((void*)&msgbuffer, 0, sizeof(msgbuffer));
		size = recv(socket, msgbuffer, sizeof(msgbuffer), 0);
		if (size < 0)
		{
			cout << "������Ϣʧ�ܣ�������룺" << WSAGetLastError() << endl;
			closesocket(socket);
			WSACleanup();
			break;
		}
		else if (size == 0)
		{
			cout << "server�ر�����" << endl;
			closesocket(socket);
		}
		else
		{
				cout << endl << "����ˣ�" << msgbuffer << endl;
		}
	}
}


int main(int argc, char** argv)
{
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
	SOCKET socket_client = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_client == INVALID_SOCKET)
	{
		cerr << "Can't create a socket! Quitting" << endl;
		WSACleanup();
		return 0;
	}

	// ��������ַ
	//char IP[20];
	sockaddr_in server_addr;
	int addr_len = sizeof(sockaddr_in);
	//cout << "�����������IP: ";
	//cin >> IP;
	memset((void*)&server_addr, 0, addr_len); // ��ַ�ṹ��0
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	//server_addr.sin_addr.S_un.S_addr = inet_addr(IP);
	server_addr.sin_addr.S_un.S_addr = inet_addr(ADDR);

	// ���������������
	if (connect(socket_client, (sockaddr*)&server_addr, addr_len) != 0)
	{
		cout << "����ʧ�ܣ�������룺" << WSAGetLastError() << endl;
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
		// ������Ϣ
		cout << "������Ҫ���͵���Ϣ��";
		cin >> msgbuffer;
		size = send(socket_client, msgbuffer, sizeof(msgbuffer), 0);
		if (size < 0)
		{
			cout << "������Ϣʧ�ܣ�������룺" << WSAGetLastError() << endl;
		}
		else if (size == 0)
		{
			cout << "server�ر�����" << endl;
		}
		else
		{
			// cout << "��Ϣ���ͳɹ�" << msgbuffer << endl;
		}
	}

	closesocket(socket_client);
	WSACleanup();
	return 0;
}