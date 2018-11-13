#pragma once
#include <iostream>
#include <WinSock2.h>

#include "UICalcServer.h"
#include "FlowCalcServer.h"



class CalcServer
{
private:
	typedef sockaddr_in SOCKADDRIN;
public:
	// 계산 서버의 생성자입니다. 내부적으로 Init()을 수행합니다.
	CalcServer();

	// 계산 서버의 소멸자입니다. 내부적으로 Finalize()를 수행합니다.
	~CalcServer();

	// 소켓 통신을 위한 초기화를 수행합니다.
	void Init();

	// 소켓 통신을 종료히가 위한 마무리를 수행합니다.
	void Finalize();

	// 클라이언트의 요청을 처리하는 함수입니다.
	void ProcessRequest();

	// 
private:
	SOCKET ServerSock, ClientSock;
	WSADATA WSAData;
	SOCKADDRIN ServerSockInfo, ClientSockInfo;
	int SizeOfClientInfo;

	UICalcServer UIServer;
	FlowCalcServer FlowServer;
};

