#include "CalcServer.h"


CalcServer::CalcServer()
{
	Init();
}


CalcServer::~CalcServer()
{
	Finalize();
}

void CalcServer::Init()
{
	{
		std::cout << "Server::Init" << std::endl;
	}

	// WSA 데이터 초기화
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0)
	{
		std::cout << " 초기화 실패 at WSAStartup() " << std::endl;
		return;
	}

	// 서버 소켓 초기화
	ServerSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ServerSock == INVALID_SOCKET)
	{
		std::cout << " 서버 소켓 생성 실패 at socket() " << std::endl;
		return;
	} 

	// 메모리 제로화
	memset(&ServerSockInfo, 0, sizeof(ServerSockInfo));

	// 서버 SOCKADDRIN 초기화
	ServerSockInfo.sin_family = AF_INET;
	ServerSockInfo.sin_port = htons(12834);
	ServerSockInfo.sin_addr.S_un.S_addr = htonl(INADDR_ANY);


	if (bind(ServerSock, (SOCKADDR*)&ServerSockInfo, sizeof(ServerSockInfo)) == SOCKET_ERROR)
	{
		std::cout << " 서버 소켓 바인딩 실패 at bind() " << std::endl;
		return;
	}

	if (listen(ServerSock, 5) == SOCKET_ERROR)
	{
		std::cout << " 대기열 실패 at listen() " << std::endl;
		return;
	}

	SizeOfClientInfo = sizeof(ClientSockInfo);
	std::cout << " 클라이언트로부터의 접속을 기다리는 중..." << std::endl;

	ClientSock = accept(ServerSock, (SOCKADDR*)&ClientSockInfo, &SizeOfClientInfo);
	if (ClientSock == INVALID_SOCKET)
	{
		std::cout << " 클라이언트 소켓 생성 실패 at socket() " << std::endl;
		return;
	}
	else
	{
		std::cout << " 클라이언트와의 접속 완료 ! 퇴근 " << std::endl;
		return;
	}
}

void CalcServer::Finalize()
{
	closesocket(ServerSock);
	closesocket(ClientSock);

	WSACleanup();
}
