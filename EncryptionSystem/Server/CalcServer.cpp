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
		std::cout << "초기화 실패 at WSAStartup() " << std::endl;
		return;
	}

	// 서버 소켓 초기화
	ServerSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ServerSock == INVALID_SOCKET)
	{
		std::cout << "서버 소켓 생성 실패 at socket() " << std::endl;
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
		std::cout << "서버 소켓 바인딩 실패 at " << __func__ << std::endl;
		return;
	}

	if (listen(ServerSock, 5) == SOCKET_ERROR)
	{
		std::cout << "대기열 실패 at " << __func__ << std::endl;
		return;
	}

	SizeOfClientInfo = sizeof(ClientSockInfo);
	std::cout << "SERVER::Waiting For Client" << std::endl;

	ClientSock = accept(ServerSock, (SOCKADDR*)&ClientSockInfo, &SizeOfClientInfo);
	if (ClientSock == INVALID_SOCKET)
	{
		std::cout << "클라이언트 소켓 생성 실패 at " << __func__ << std::endl;
		return;
	}

	MainLoop();
}

void CalcServer::Finalize()
{
	closesocket(ServerSock);
	closesocket(ClientSock);

	WSACleanup();
}

void CalcServer::MainLoop()
{
	while ( 1 )
	{
		ParseClientData();
	}
}

void CalcServer::ParseClientData()
{
	if (recv(ClientSock, Buffer, sizeof(Buffer), 0) == SOCKET_ERROR)
	{
		std::cout << "ERROR::RECV Fail In " << __func__ << std::endl;
	}

	// 클라이언트가 로그인 여부를 확인할 때 처리합니다.
	if (strcmp(Buffer, CalcClient::Request::LOGINFLAG.c_str()) == 0)
	{
		ProcessLoginFlag();
	}
	// 클라이언트가 로그인 절차를 수행한다고 하면 처리합니다.
	else if (strcmp(Buffer, CalcClient::Request::USERLOGIN.c_str()) == 0)
	{
		ProcessUserLogin();
	}
	// 클라이언트가 회원가입 프로세스를 진행한다고 하면 처리합니다.
	else if (strcmp(Buffer, CalcClient::Request::USERSIGNUP.c_str()) == 0)
	{
		ProcessUserSignUp();
	}
	// 클라이언트가 처리할 정수값을 보내오면 처리합니다.
	else if (strcmp(Buffer, CalcClient::Request::INTEGERVALUE.c_str()) == 0)
	{
		ProcessIntegerValue();
	}
	else if (strcmp(Buffer, CalcClient::Request::DISCONNECT.c_str()) == 0)
	{
		std::cout << "SERVER::DISCONNECT" << std::endl;
		exit(1);
	}
	else
	{
		std::cout << "ERROR::Unknown Value In " << __func__ << std::endl;
		exit(1);
	}
}

void CalcServer::ProcessLoginFlag()
{
	// 클라이언트로 현재 로그인 세션 유지 여부를 전송합니다.
	if (send(ClientSock, Buffer, sizeof(Buffer), 0) == SOCKET_ERROR)
	{
		std::cout << "ERROR::SEND Fail In " << __func__ << std::endl;
 	}
}

void CalcServer::ProcessUserLogin()
{
	// 클라이언트로부터 ID와 PW를 수신합니다.
	if (recv(ClientSock, Buffer, sizeof(Buffer), 0) == SOCKET_ERROR)
	{
		std::cout << "ERROR::RECV Fail With ID In " << __func__ << std::endl;
	}
	if (send(ClientSock, Buffer, sizeof(Buffer), 0) == SOCKET_ERROR)
	{
		std::cout << "ERROR::RECV Fail With PW In " << __func__ << std::endl;
	}

	// 클라이언트로 로그인 결과를 전송합니다.
	if (send(ClientSock, Buffer, sizeof(Buffer), 0) == SOCKET_ERROR)
	{
		std::cout << "ERROR::SEND Fail With Message In " << __func__ << std::endl;
	}
}

void CalcServer::ProcessUserSignUp()
{
	// 클라이언트로부터 ID와 PW를 수신합니다.
	if (recv(ClientSock, Buffer, sizeof(Buffer), 0) == SOCKET_ERROR)
	{
		std::cout << "ERROR::RECV Fail With ID In " << __func__ << std::endl;
	}
	if (send(ClientSock, Buffer, sizeof(Buffer), 0) == SOCKET_ERROR)
	{
		std::cout << "ERROR::RECV Fail With PW In " << __func__ << std::endl;
	}

	// 클라이언트로 회원가입 결과를 전송합니다.
	if (send(ClientSock, Buffer, sizeof(Buffer), 0) == SOCKET_ERROR)
	{
		std::cout << "ERROR::SEND Fail With Message In " << __func__ << std::endl;
	}

}

void CalcServer::ProcessIntegerValue()
{
	// 클라이언트로부터 정수값을 수신합니다.
	if (recv(ClientSock, Buffer, sizeof(Buffer), 0) == SOCKET_ERROR)
	{
		std::cout << "ERROR::RECV Fail With ID In " << __func__ << std::endl;
	}

	// 클라이언트의 값을 정수로 변환합니다.
	int ReceivedValue = atoi(Buffer);
	int SquareValue;
	if (ConnectInfo.IsValid())
	{
		SquareValue = ReceivedValue * ReceivedValue;
	}
	else
	{
		std::cout << "ERROR::Invalid ClientInfo in " << __func__ << std::endl;
		exit(1);
	}
	
	// 클라이언트로 메시지와 정수값을 전송합니다.
	strcpy_s(Buffer, std::to_string(SquareValue).c_str());
	if (send(ClientSock, Buffer, sizeof(Buffer), 0) == SOCKET_ERROR)
	{
		std::cout << "ERROR::SEND Fail With Message & Value In " << __func__ << std::endl;
		exit(1);
	}
}

std::string const CalcServer::Result::LoginFlag::NOLOGIN{ "RESPOND NOLOGIN" };
std::string const CalcServer::Result::LoginFlag::LOGIN{ "RESPOND LOGIN" };

std::string const CalcServer::Result::Login::IDMISMATCH("RESPOND ID MISMATCH");
std::string const CalcServer::Result::Login::PWMISTMATCH("RESPOND PW MISMATCH");
std::string const CalcServer::Result::Login::SUCCESS("RESPOND LOGIN SUCCESS");

std::string const CalcServer::Result::SignUp::IDALREADY("RESPOND ID ALREADY");
std::string const CalcServer::Result::SignUp::SUCCESS("RESPOND SIGNUP SUCEESS");

std::string const CalcServer::Result::Calc::SUCCESS("RESPOND CALC SUCCESS");
std::string const CalcServer::Result::Calc::NONVALID("RESPOND CALC NONVALID");