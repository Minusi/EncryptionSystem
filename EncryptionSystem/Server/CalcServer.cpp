#include "CalcServer.h"
#include "Debug\Debug.h"


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
#ifdef CONSOLE_DEBUG
	CON_NEWLINE;
	std::cout << CON_SERVER_TAG << CON_DEBUG_TAG <<
		__func__ << std::endl;
#endif // CONSOLE_DEBUG


	// WSA 데이터 초기화
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0)
	{
		std::cout << "SERVER::ERROR::WSAStartUp in " << __func__ << std::endl;
		exit(1);
	}

	// 서버 소켓 초기화
	ServerSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ServerSock == INVALID_SOCKET)
	{
		std::cout << "SERVER::ERROR::Socket in " << __func__ << std::endl;
		exit(1);
	} 

	// 메모리 제로화
	memset(&ServerSockInfo, 0, sizeof(ServerSockInfo));

	// 서버 SOCKADDRIN 초기화
	ServerSockInfo.sin_family = AF_INET;
	ServerSockInfo.sin_port = htons(12834);
	ServerSockInfo.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	// 소켓을 바인딩합니다.
	if (bind(ServerSock, (SOCKADDR*)&ServerSockInfo, sizeof(ServerSockInfo)) == SOCKET_ERROR)
	{
		std::cout << "SERVER::ERROR::Bind in " << __func__ << std::endl;
		exit(1);
	}

	// 소켓을 리슨합니다.
	if (listen(ServerSock, 5) == SOCKET_ERROR)
	{
		std::cout << "SERVER::ERROR::Listen in " << __func__ << std::endl;
		exit(1);
	}

	SizeOfClientInfo = sizeof(ClientSockInfo);


#ifdef CONSOLE_DEBUG
	std::cout << CON_SERVER_TAG << CON_DEBUG_TAG <<
		"Waiting For Client" << std::endl;
#endif


	// 클라이언트 측과 연결합니다.
	ClientSock = accept(ServerSock, (SOCKADDR*)&ClientSockInfo, &SizeOfClientInfo);
	if (ClientSock == INVALID_SOCKET)
	{
		std::cout << "SERVER::ERROR::Accept Error In " << __func__ << std::endl;
		exit(1);
	}


#ifdef CONSOLE_DEBUG
	std::cout << CON_SERVER_TAG << CON_DEBUG_TAG <<
		"Connect Client" << std::endl;
#endif

	UserDatabase.PrintUsersInfo();

	// 메인루프로 진입합니다.
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
		std::cout << "SERVER::ERROR::RECV In " << __func__ << std::endl;
	}

#ifdef CONSOLE_DEBUG
	CON_NEWLINE;
	std::cout << CON_SERVER_TAG << CON_DEBUG_TAG <<
		"Buffer >> " << Buffer << " In " << __func__ << std::endl;
#endif // CONSOLE_DEBUG


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
	// 클라이언트가 종료 플래그를 보내오면 종료합니다.
	// TODO : 다중 유저 접속에 맞지 않는 코드이므로 그러한 요구가 발생하면 
	// 코드를 재구성할 것
	else if (strcmp(Buffer, CalcClient::Request::DISCONNECT.c_str()) == 0)
	{
		std::cout << "SERVER::DISCONNECT" << std::endl;
		exit(1);
	}
	// 클라이언트가 전송한 잘못된 값에 대한 예외 처리? 유사 어서트? 수행
	else
	{
		std::cout << "SERVER::ERROR::Unknown Value in " << __func__ << std::endl;
		std::cout << "SERVER::ERROR::Unknown Value in Buffer >> " << Buffer << std::endl;
		exit(1);
	}
}

void CalcServer::ProcessLoginFlag()
{
	// 로그인 여부를 검사합니다.
	if (ConnectInfo.Authorized())
	{
		strcpy_s(Buffer, CalcServer::Result::LoginFlag::LOGIN.c_str());
	}
	else
	{
		strcpy_s(Buffer, CalcServer::Result::LoginFlag::NOLOGIN.c_str());
	}

#ifdef CONSOLE_DEBUG
	std::cout << CON_SERVER_TAG << CON_DEBUG_TAG <<
		"Before Send Buffer >> " << Buffer << " In " << __func__ << std::endl;
#endif // CONSOLE_DEBUG


	// 클라이언트로 현재 로그인 세션 유지 여부를 전송합니다.
	if (send(ClientSock, Buffer, sizeof(Buffer), 0) == SOCKET_ERROR)
	{
		std::cout << "SERVER::ERROR::SEND In " << __func__ << std::endl;
 	}
}

void CalcServer::ProcessUserLogin()
{
	std::string NeedToCheckID;
	std::string NeedToCheckPW;

	// 클라이언트로부터 ID와 PW를 수신합니다.
	if (recv(ClientSock, Buffer, sizeof(Buffer), 0) == SOCKET_ERROR)
	{
		std::cout << "ERROR::RECV Fail With ID In " << __func__ << std::endl;
	}
	NeedToCheckID += Buffer;

	if (recv(ClientSock, Buffer, sizeof(Buffer), 0) == SOCKET_ERROR)
	{
		std::cout << "ERROR::RECV Fail With PW In " << __func__ << std::endl;
	}
	NeedToCheckPW += Buffer;

	// 데이터베이스를 확인합니다.
	bool bSuccessLogin = UserDatabase.FindIDAndPW(NeedToCheckID, NeedToCheckPW);
	if (bSuccessLogin == true)
	{
		ConnectInfo.Connect(NeedToCheckID);
		strcpy_s(Buffer, CalcServer::Result::Login::SUCCESS.c_str());
	}
	else
	{
		strcpy_s(Buffer, CalcServer::Result::Login::MISMATCH.c_str());
	}


#ifdef CONSOLE_DEBUG
	std::cout << CON_SERVER_TAG << CON_DEBUG_TAG <<
		"Before Send Buffer >> " << Buffer << " In " << __func__ << std::endl;
#endif // CONSOLE_DEBUG


	// 클라이언트로 로그인 결과를 전송합니다.
	if (send(ClientSock, Buffer, sizeof(Buffer), 0) == SOCKET_ERROR)
	{
		std::cout << "ERROR::SEND Fail With Message In " << __func__ << std::endl;
	}
}

void CalcServer::ProcessUserSignUp()
{
	std::string NeedToCheckID;
	std::string NeedToCheckPW;
	
	// 클라이언트로부터 ID와 PW를 수신합니다.
	if (recv(ClientSock, Buffer, sizeof(Buffer), 0) == SOCKET_ERROR)
	{
		std::cout << "ERROR::RECV Fail With ID In " << __func__ << std::endl;
	}
	NeedToCheckID = Buffer;

	if (send(ClientSock, Buffer, sizeof(Buffer), 0) == SOCKET_ERROR)
	{
		std::cout << "ERROR::RECV Fail With PW In " << __func__ << std::endl;
	}
	NeedToCheckPW = Buffer;

	// 아이디가 있는지 확인합니다.
	bool bNoExistID = UserDatabase.ExistID(NeedToCheckID);
	if (bNoExistID == true)
	{
		strcpy_s(Buffer, CalcServer::Result::SignUp::SUCCESS.c_str());

		// 데이터베이스에 등록합니다.
		UserDatabase.InsertIDAndPW(NeedToCheckID, NeedToCheckPW);
	}
	else
	{
		strcpy_s(Buffer, CalcServer::Result::SignUp::IDALREADY.c_str());
	}

#ifdef CONSOLE_DEBUG
	std::cout << CON_SERVER_TAG << CON_DEBUG_TAG <<
		"Before Send Buffer >> " << Buffer << " In " << __func__ << std::endl;
#endif // CONSOLE_DEBUG


	// 클라이언트로 회원가입 결과를 전송합니다.
	if (send(ClientSock, Buffer, sizeof(Buffer), 0) == SOCKET_ERROR)
	{
		std::cout << "SERVER::ERROR::SEND Fail With Message In " << __func__ << std::endl;
	}
}

void CalcServer::ProcessIntegerValue()
{
	// 로그인이 인증되었는지 확인합니다.
	if (ConnectInfo.Authorized() == false)
	{
		std::cout << "SERVER::ERROR::Invalid ConnectInfo in " << __func__ << std::endl;
		exit(1);
	}

	// 클라이언트로부터 정수값을 수신합니다.
	if (recv(ClientSock, Buffer, sizeof(Buffer), 0) == SOCKET_ERROR)
	{
		std::cout << "SERVER::ERROR::RECV Fail With ID In " << __func__ << std::endl;
	}

#ifdef CONSOLE_DEBUG
	std::cout << CON_SERVER_TAG << CON_DEBUG_TAG <<
		"After Recv Buffer >> " << Buffer << " In " << __func__ << std::endl;
#endif // CONSOLE_DEBUG

	// 클라이언트의 값을 정수로 변환합니다.
	int ReceivedValue = atoi(Buffer);
	int SquareValue;

	SquareValue = std::pow(ReceivedValue, 2);

	// 문자열 값이 유효한지 아닌지 검사하고 그 결과를 기록합니다.
	std::string StringValue = std::to_string(SquareValue);
	if (StringValue.empty())
	{
		strcpy_s(Buffer, Result::Calc::NONVALID.c_str());
	}
	else
	{
		strcpy_s(Buffer, Result::Calc::SUCCESS.c_str());
	}

#ifdef CONSOLE_DEBUG
	std::cout << CON_SERVER_TAG << CON_DEBUG_TAG <<
		"Before Send Buffer >> " << Buffer << " In " << __func__ << std::endl;
#endif // CONSOLE_DEBUG

	// 문자열 유효에 대한 결과값을 전송합니다.
	if (send(ClientSock, Buffer, sizeof(Buffer), 0) == SOCKET_ERROR)
	{
		std::cout << "SERVER::ERROR::SEND Fail With Message In " << __func__ << std::endl;
		exit(1);
	}

	std::string StringBuffer = Buffer;
	// 계산이 성공했으면 계산 결과값도 전송합니다.
	if (StringBuffer == Result::Calc::SUCCESS.c_str())
	{
		std::string StringValue = std::to_string(SquareValue);
		strcpy_s(Buffer, StringValue.c_str());

		if (send(ClientSock, Buffer, sizeof(Buffer), 0) == SOCKET_ERROR)
		{
			std::cout << "SERVER::ERROR::SEND Fail With Value In " << __func__ << std::endl;
			exit(1);
		}
	}
#ifdef CONSOLE_DEBUG
	else
	{
		std::cout << CON_SERVER_TAG << CON_ERROR_TAG <<
			"Buffer String Mismatch In " << __func__ << std::endl;
	}
#endif // CONSOLE_DEBUG
}

std::string const CalcServer::Result::LoginFlag::NOLOGIN{ "RESPOND NOLOGIN" };
std::string const CalcServer::Result::LoginFlag::LOGIN{ "RESPOND LOGIN" };

std::string const CalcServer::Result::Login::MISMATCH("RESPOND MISMATCH");
std::string const CalcServer::Result::Login::SUCCESS("RESPOND LOGIN SUCCESS");

std::string const CalcServer::Result::SignUp::IDALREADY("RESPOND ID ALREADY");
std::string const CalcServer::Result::SignUp::SUCCESS("RESPOND SIGNUP SUCEESS");

std::string const CalcServer::Result::Calc::SUCCESS("RESPOND CALC SUCCESS");
std::string const CalcServer::Result::Calc::NONVALID("RESPOND CALC NONVALID");