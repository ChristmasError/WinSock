#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#pragma warning(disable : 4996) 

#include<winsock2.h>


#pragma comment(lib, "ws2_32.lib")
#define socklen_t int

class  WinSock
{
public:
	WinSock();
	virtual ~WinSock();

	bool		LoadSocketLib();
	void		UnloadSocketLib();
	int			CreateSocket();
	int  		CreateWSASocket();
	bool		Bind(unsigned short port);
	bool		Connect(const char*ip, unsigned short port, int timeout = 1000);
	WinSock		Accept();
	int			Recv(char* buf, int bufsize);
	int			Send(const char* buf, int sendsize);
	void		Close();
	bool		SetBlock(bool isblock);
	const char*       GetLocalIP();

	int					socket;
	SOCKADDR_IN         addr;
	const char*			ip;
	unsigned short		port;
private:
	void _ShowMessage(const char* ch, ...) const;
};

