#include "CalcClient.h"
#include <WS2tcpip.h>


CalcClient::CalcClient() : ClientPort(12834), ServerIP("127.0.0.1")
{
	//TODO : 현재는 디폴트가 로컬호스트이지만 나중에 nullptr로 바꾸고 다른 입력받을 수단을 생성할 것
	Init();
}


CalcClient::~CalcClient()
{
	Finalize();
}

void CalcClient::Init()
{
	{
		std::cout << "Client::Init" << std::endl;
	}

	// WSA 초기화
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0)
	{
		std::cout << " 초기화 실패 at WSAStartup() " << std::endl;
		return;
	}

	ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ClientSocket == INVALID_SOCKET)
	{
		std::cout << " 클라이언트 소켓 생성 실패 at socket() " << std::endl;
		return;
	}

	// 메모리 영역 제로 초기화 수행
	memset(&ClientSockInfo, 0, sizeof(ClientSockInfo));

	// 포트가 올바른지 체크
	if (ClientPort < 0)
	{
		std::cout << " 클라이언트 포트 번호 초기화 실패 in Init() " << std::endl;
		return;
	}

	// ServerIP가 올바른지 체크
	if (ServerIP.empty())
	{
		std::cout << " 초기화되지 않은 서버 IP in Init() " << std::endl;
		return;
	}

	// 클라이언트 SOCKADDRIN 구조체 초기화
	ClientSockInfo.sin_family = AF_INET;
	ClientSockInfo.sin_port = htons(ClientPort);
	inet_pton(AF_INET, ServerIP.c_str(), &ClientSockInfo.sin_addr.S_un.S_addr);

	// 서버와 클라이언트 연결
	if (connect(ClientSocket, (SOCKADDR*)&ClientSockInfo, sizeof(ClientSockInfo)) == SOCKET_ERROR)
	{
		std::cout << " 서버 접속 실패 at connect() " << std::endl;
		return;
	}
}

void CalcClient::Finalize()
{
	closesocket(ClientSocket);
	WSACleanup();
}
