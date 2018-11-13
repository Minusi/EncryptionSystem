#pragma once
#include <iostream>
#include <stdlib.h>
#include <WinSock2.h>



class CalcClient
{
private:
	typedef sockaddr_in SOCKADDRIN;

public:
	CalcClient();
	~CalcClient();

	// 서버와의 소켓 통신을 위한 초기화를 수행합니다.
	void Init();

	// 서버와의 소켓 통신을 종료하기 위한 절차를 수행합니다.
	void Finalize();

private:
	SOCKET ClientSocket;
	WSADATA WSAData;
	SOCKADDRIN ClientSockInfo;

	int ClientPort;
	std::string ServerIP;
};

