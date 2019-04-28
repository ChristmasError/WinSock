#pragma once
#include"WinSock.h"
WinSock::WinSock()
{
	// ��ʼ��
	socket = 0;
	addr = { 0 };
	ip = NULL;
	port = 0;
}
/////////////////////////////////////////////////////////////////
// ����&ж��WinSocket��
bool WinSock::LoadSocketLib()
{
	WSADATA wsaData;
	int nResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (NO_ERROR != nResult)
	{
		this->_ShowMessage("��ʼ��WinSock 2.2 ʧ�ܣ�\n");
		return false;
	}
	else
		return true;
}
void WinSock::UnloadSocketLib()
{
	WSACleanup();
}

char* WinSock::GetLocalIP()
{
	//  ����������Ļ�����
	char szHost[256];
	//  ȡ�ñ�����������
	::gethostname(szHost, 256);
	//  ͨ���������õ���ַ��Ϣ��һ�����������ж�����������IP��ַ
	hostent *pHost = ::gethostbyname(szHost);

	struct in_addr addr;
	//��õ�һ��Ϊ���ص�IP��ַ�������ֽ���
	char *p = pHost->h_addr_list[0];

	// ip��ַת��Ϊ�ַ�����ʽ
	memmove(&addr, p, 4);
	//memcpy(&addr.S_un.S_addr, p, pHost->h_length);
	ip = inet_ntoa(addr);
	printf("����IP : %s\n", ip);
	return ip;
}
int WinSock::CreateSocket()
{
	GetLocalIP();
	socket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == socket)
		std::cout << "����socketʧ�ܣ�\n";
	return socket;
}
int WinSock::CreateWSASocket()
{
	GetLocalIP();
	socket = ::WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == socket)
		this->_ShowMessage("����WSAsocketʧ�ܣ�\n");
	return socket;
}

bool WinSock::Bind(unsigned short port)
{
	GetLocalIP();
	if (socket == INVALID_SOCKET)
	{
		std::cerr << "INVALID_SOCKET!\n";
		return false;
	}
	//�����˿ڳɹ���
	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = (inet_addr(ip));
	//��
	if (::bind(socket, (sockaddr*)&saddr, sizeof(saddr)) != 0)
	{
		printf("�� %d �˿�ʧ��!\n", port);
		return false;
	}
	printf("�� %d �˿ڳɹ�!\n", port);
	//����
	return true;
}
WinSock WinSock::Accept()
{
	SOCKADDR_IN   caddr;
	socklen_t len = sizeof(caddr);
	WinSock acCilent;
	int clientsock = accept(socket, (sockaddr*)&caddr, &len); //�ɹ�����һ���µ�socket
	if (clientsock <= 0)
		return acCilent;
	printf("���ӿͻ��� [%d] !\n", clientsock);
	ip = inet_ntoa(caddr.sin_addr);
	acCilent.port = ntohs(caddr.sin_port);
	acCilent.socket = clientsock;
	printf("�ͻ���ip��%s , �˿ںţ�%d\n", acCilent.ip, acCilent.port);
	return acCilent;
}

int WinSock::Recv(char* buf, int bufsize)
{
	return recv(socket, buf, bufsize, 0);
}

int WinSock::Send(const char* buf, int size)
{
	int s = 0;
	while (s != size)
	{
		int len = send(socket, buf + s, size - s, 0);
		if (len <= 0)
			break;
		s += len;
	}
	return s;
}

bool WinSock::SetBlock(bool isblock)
{
	if (socket == INVALID_SOCKET)
	{
		std::cerr << "INVALID_SOCKET!\n";
		return false;
	}
	unsigned long ul = 0;
	if (!isblock)
		ul = 1;
	ioctlsocket(socket, FIONBIO, &ul);
	return true;
}

bool WinSock::Connect(const char* ip, unsigned short port, int timeout)
{
	if (socket == INVALID_SOCKET)
	{
		std::cerr << "INVALID_SOCKET!\n";
		return false;
	}
	sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = inet_addr(ip);

	fd_set set;
	if (connect(socket, (sockaddr*)&saddr, sizeof(saddr)) != 0)
	{
		FD_ZERO(&set);
		FD_SET(socket, &set);
		timeval tm;
		tm.tv_sec = 0;
		tm.tv_usec = timeout * 1000;
		if (select(socket + 1, 0, &set, 0, &tm) <= 0)
		{
			printf("����ʧ�ܻ�ʱ!\n");
			printf("���� %s : %d ʧ��!: %s\n", ip, port, strerror(errno));
			return false;
		}
	}

	printf("���� %s : %d �ɹ�!\n", ip, port);
	return true;
}
void WinSock::Close()
{
	if (socket <= 0)
		return;
	std::cout << "���ӹرգ� \n";
	closesocket(socket);
}
WinSock::~WinSock()
{
}
/////////////////////////////////////////////////////////////////
// private:
void WinSock::_ShowMessage(const char* msg, ...) const
{
	std::cout << msg;
}