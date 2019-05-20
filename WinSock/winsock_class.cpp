#pragma once

#include "winsock_class.h"
#include <iostream>
WinSock::WinSock()
{
	// 初始化
	socket = INVALID_SOCKET;
	addr = { 0 };
	ip = NULL;
	port = 0;
}

WinSock::~WinSock()
{
	if (socket != INVALID_SOCKET)
	{
		closesocket(socket);
		socket = INVALID_SOCKET;
	}
	//if(ip!=NULL)
	//	delete[] ip;
}
/////////////////////////////////////////////////////////////////
// 加载&卸载WinSocket库
bool WinSock::LoadSocketLib()
{
	WSADATA wsaData;
	int nResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (NO_ERROR != nResult)
	{
		this->_ShowMessage("初始化WinSock 2.2 失败!\n");
		return false;
	}
	else
		return true;
}
void WinSock::UnloadSocketLib()
{
	WSACleanup();
}

const char* WinSock::GetLocalIP()
{
	//  存放主机名的缓冲区
	char szHost[256];
	//  取得本地主机名称
	::gethostname(szHost, 256);
	//  通过主机名得到地址信息，一个主机可能有多个网卡，多个IP地址
	hostent *pHost = ::gethostbyname(szHost);

	struct in_addr addr;
	//获得第一个为返回的IP地址（网络字节序）
	char *p = pHost->h_addr_list[0];

	// ip地址转换为字符串形式
	memmove(&addr, p, 4);
	//memcpy(&addr.S_un.S_addr, p, pHost->h_length);
	ip = inet_ntoa(addr);

	return ip;
}
int WinSock::CreateSocket()
{
	if (ip == NULL)
		GetLocalIP();
	socket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == socket)
		std::cout << "创建socket失败!\n";
	return socket;
}
int WinSock::CreateWSASocket()
{
	if (ip == NULL)
		GetLocalIP();
	socket = ::WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == socket)
		this->_ShowMessage("创建WSAsocket失败!\n");
	return socket;
}

bool WinSock::Bind(unsigned short port)
{
	if (socket == INVALID_SOCKET)
	{
		std::cerr << "INVALID_SOCKET!\n";
		return false;
	}
	//创建端口成功后
	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = (inet_addr(ip));
	//绑定
	if (::bind(socket, (sockaddr*)&saddr, sizeof(saddr)) != 0)
	{
		printf("绑定 %d 端口失败!\n", port);
		return false;
	}
	printf("绑定 %d 端口成功!\n", port);
	//监听
	return true;
}
WinSock WinSock::Accept()
{
	SOCKADDR_IN   caddr;
	socklen_t len = sizeof(caddr);
	WinSock acCilent;
	int clientsock = accept(socket, (sockaddr*)&caddr, &len); //成功返回一个新的socket
	if (clientsock <= 0)
		return acCilent;
	printf("连接客户端 [%d] !\n", clientsock);
	ip = inet_ntoa(caddr.sin_addr);
	acCilent.port = ntohs(caddr.sin_port);
	acCilent.socket = clientsock;
	printf("客户端ip：%s , 端口号：%d\n", acCilent.ip, acCilent.port);
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
			printf("连接失败或超时!\n");
			printf("连接 %s : %d 失败!: %s\n", ip, port, strerror(errno));
			return false;
		}
	}

	printf("连接 %s : %d 成功!\n", ip, port);
	return true;
}
void WinSock::Close()
{
	if (socket <= 0)
		return;
	std::cout << "连接关闭! \n";
	closesocket(socket);
}

/////////////////////////////////////////////////////////////////
// private:
void WinSock::_ShowMessage(const char* msg, ...) const
{
	std::cout << msg;
}