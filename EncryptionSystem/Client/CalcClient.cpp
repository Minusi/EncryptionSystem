#include "CalcClient.h"
#include <WS2tcpip.h>


CalcClient::CalcClient() 
	: ClientPort(12834)
	, ServerIP("127.0.0.1")
	, pClientUI(CalcUI::GetInstance())
	, CurrentMode(Mode::NONE)
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
		std::cout << "ERROR::초기화 실패 at WSAStartup() " << std::endl;
		return;
	}

	ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ClientSocket == INVALID_SOCKET)
	{
		std::cout << "ERROR::클라이언트 소켓 생성 실패 at socket() " << std::endl;
		return;
	}

	// 메모리 영역 제로 초기화 수행
	memset(&ClientSockInfo, 0, sizeof(ClientSockInfo));

	// 포트가 올바른지 체크
	if (ClientPort < 0)
	{
		std::cout << "ERROR::클라이언트 포트 번호 초기화 실패 in Init() " << std::endl;
		return;
	}

	// ServerIP가 올바른지 체크
	if (ServerIP.empty())
	{
		std::cout << "ERROR::초기화되지 않은 서버 IP in Init() " << std::endl;
		return;
	}

	// 클라이언트 SOCKADDRIN 구조체 초기화
	ClientSockInfo.sin_family = AF_INET;
	ClientSockInfo.sin_port = htons(ClientPort);
	inet_pton(AF_INET, ServerIP.c_str(), &ClientSockInfo.sin_addr.S_un.S_addr);

	// 서버와 클라이언트 연결
	if (connect(ClientSocket, (SOCKADDR*)&ClientSockInfo, sizeof(ClientSockInfo)) == SOCKET_ERROR)
	{
		std::cout << "ERROR::서버 접속 실패 at connect() " << std::endl;
		return;
	}

	MainLoop();
}

void CalcClient::Finalize()
{
	closesocket(ClientSocket);
	WSACleanup();
}

void CalcClient::MainLoop()
{
	CurrentMode = Mode::MAIN;

	while (1)
	{
		ProcessMode();
	}
}

void CalcClient::ProcessMode()
{
	if (CurrentMode == Mode::MAIN)
	{
		ProcessMainMode();
	}
	else if (CurrentMode == Mode::Login)
	{
		ProcessLoginMode();
	}
	else if (CurrentMode == Mode::SignUp)
	{
		ProcessSignUpMode();
	}
	else if (CurrentMode == Mode::Calc)
	{
		ProcessCalcMode();
	}
}

void CalcClient::ProcessMainMode()
{
	pClientUI->PrintMainUI();
	pClientUI->PrintIntInterface();

	enum class Select
	{
		NONE, LOGIN, SIGNUP, CALC, QUIT
	};
	int value;
	std::cin >> value;

	Select SelectValue = static_cast<Select>(value);
	
	// 로그인 버튼을 눌렀을 때의 동작입니다.
	if (SelectValue == Select::LOGIN)
	{
		strcpy_s(Buffer, CalcClient::Request::LOGINFLAG.c_str());

		// 서버에게 로그인 정보를 달라고 요청합니다.
		if (send(ClientSocket, Buffer, sizeof(Buffer), 0) == SOCKET_ERROR)
		{
			std::cout << "ERROR::SEND Fail In " << __func__ << std::endl;
			exit(1);
		}

		// 서버로부터 로그인 정보를 받아옵니다.
		if (recv(ClientSocket, Buffer, sizeof(Buffer), 0) == SOCKET_ERROR)
		{
			std::cout << "ERROR::RECV Fail In " << __func__ << std::endl;
			exit(1);
		}

		// 로그인 정보에 대한 파싱을 수행합니다.
		if ((strcmp(Buffer, CalcServer::Result::LoginFlag::NOLOGIN.c_str())) == 0)
		{
			SetCurrentMode(Mode::Login);
		}
		else if ((strcmp(Buffer, CalcServer::Result::LoginFlag::LOGIN.c_str())) == 0)
		{
			SetCurrentMode(Mode::Calc);
		}
	}
	// 회원가입 버튼을 눌렀을 때의 동작입니다.
	else if (SelectValue == Select::SIGNUP)
	{
		strcpy_s(Buffer, "REQUEST LOGIN FLAG");

		// 서버에게 로그인 정보를 달라고 요청합니다.
		if (send(ClientSocket, Buffer, sizeof(Buffer), 0) == SOCKET_ERROR)
		{
			std::cout << "ERROR::SEND Fail In ProcessMainMode" << std::endl;
			exit(1);
		}

		// 서버로부터 로그인 정보를 받아옵니다.
		if (recv(ClientSocket, Buffer, sizeof(Buffer), 0) == SOCKET_ERROR)
		{
			std::cout << "ERROR::RECV Fail In ProcessMainMode" << std::endl;
			exit(1);
		}

		// 로그인 정보에 대한 파싱을 수행합니다.
		if ((strcmp(Buffer, CalcServer::Result::LoginFlag::NOLOGIN.c_str())) == 0)
		{
			SetCurrentMode(Mode::SignUp);
		}
		else if ((strcmp(Buffer, CalcServer::Result::LoginFlag::LOGIN.c_str())) == 0)
		{
			SetCurrentMode(Mode::MAIN);
		}
	}
	// 계산 버튼을 눌렀을 때의 동작입니다.
	else if (SelectValue == Select::CALC)
	{
		strcpy_s(Buffer, "REQUEST LOGIN FLAG");

		// 서버에게 로그인 정보를 달라고 요청합니다.
		if (send(ClientSocket, Buffer, sizeof(Buffer), 0) == SOCKET_ERROR)
		{
			std::cout << "ERROR::SEND Fail With Message In ProcessMainMode" << std::endl;
			exit(1);
		}

		// 서버로부터 로그인 정보를 받아옵니다.
		if (recv(ClientSocket, Buffer, sizeof(Buffer), 0) == SOCKET_ERROR)
		{
			std::cout << "ERROR::RECV Fail In ProcessMainMode" << std::endl;
			exit(1);
		}

		// 로그인 정보에 대한 파싱을 수행하니다.
		if ((strcmp(Buffer, CalcServer::Result::LoginFlag::NOLOGIN.c_str())) == 0)
		{
			SetCurrentMode(Mode::Login);
		}
		else if ((strcmp(Buffer, CalcServer::Result::LoginFlag::LOGIN.c_str())) == 0)
		{
			SetCurrentMode(Mode::Calc);
		}

	}
	// 나가기 버튼을 눌렀을 때의 동작입니다.
	else if (SelectValue == Select::QUIT)
	{
		std::cout << "QUIT::Application Will Be Exit" << std::endl;
		strcpy_s(Buffer, CalcClient::Request::DISCONNECT.c_str());
		if (send(ClientSocket, Buffer, sizeof(Buffer), 0) == SOCKET_ERROR)
		{
			std::cout << "ERROR::SEND Fail In " << __func__ << std::endl;
		}
		exit(1);
	}
	// 초기화 에러시 나타나는 항목입니다.
	else if (SelectValue == Select::NONE)
	{
		std::cout << "CLIENT::ALARM::NONE Value Occur" << std::endl;
		std::cout << "CLINET::ALARM::NONE Means System Maybe Exposes Cautions" << std::endl;
		system("pause");
	}
	// 잘못된 값 입력시 나타나는 항목입니다.
	else
	{
		std::cout << "CLIENT::ALARM::Invalid Value" << std::endl;
		system("pause");
	}
}

void CalcClient::ProcessLoginMode()
{
	std::string UserID, UserPW;

	pClientUI->PrintSignInUI();
	// 사용자 아이디를 입력받습니다.
	pClientUI->PrintIDInterface();
	std::cin >> UserID;

	// 사용자 비밀번호를 입력받습니다.
	pClientUI->PrintPWInterface();
	std::cin >> UserPW;

	// 서버에게 사용자 정보를 보낼 것이라고 알립니다.
	strcpy_s(Buffer, "REQUEST USER LOGIN");
	if (send(ClientSocket, Buffer, sizeof(Buffer), 0) == SOCKET_ERROR)
	{
		std::cout << "CLIENT::ERROR::SEND Fail With Message In " << __func__ << std::endl;
		exit(1);
	}

	// ID와 비밀번호를 서버로 전송합니다.
	strcpy_s(Buffer, UserID.c_str());
	if (send(ClientSocket, Buffer, sizeof(Buffer), 0) == SOCKET_ERROR)
	{
		std::cout << "CLIENT::ERROR::SEND Fail With ID In " << __func__ << std::endl;
		exit(1);
	}
	strcpy_s(Buffer, UserPW.c_str());
	if (send(ClientSocket, Buffer, sizeof(Buffer), 0) == SOCKET_ERROR)
	{
		std::cout << "CLIENT::ERROR::SEND Fail With PW In " << __func__ << std::endl;
		exit(1);
	}

	// 서버로부터 로그인에 대한 결과를 가져옵니다.
	if (recv(ClientSocket, Buffer, sizeof(Buffer), 0) == SOCKET_ERROR)
	{
		std::cout << "CLIENT::ERROR::RECV Fail In " << __func__ << std::endl;
		exit(1);
	}
	
	// 로그인 결과에 대한 파싱을 수행합니다.
	if (strcmp(Buffer, CalcServer::Result::Login::MISMATCH.c_str()) == 0)
	{
		std::cout << std::endl;
		std::cout << "CLIENT::WARN::Input Mismatch" << std::endl;
		system("pause");
		
		SetCurrentMode(Mode::MAIN);
	}
	else if (strcmp(Buffer, CalcServer::Result::Login::SUCCESS.c_str()) == 0)
	{
		std::cout << std::endl;
		std::cout << "CLIENT::ALARM::Login Success" << std::endl;
		system("pause");

		SetCurrentMode(Mode::MAIN);
	}
	else
	{
		std::cout << "CLIENT::ERROR::Invalid Message In " << __func__ << std::endl;
		exit(1);
	}
}

void CalcClient::ProcessSignUpMode()
{
	std::string UserID, UserPW;

	pClientUI->PrintSignUpUI();
	// 사용자 아이디를 입력받습니다.
	pClientUI->PrintIDInterface();
	std::cin >> UserID;

	// 사용자 비밀번호를 입력받습니다.
	pClientUI->PrintPWInterface();
	std::cin >> UserPW;

	// 서버에게 회원가입 정보를 보낼 것이라고 알립니다.
	strcpy_s(Buffer, "REQUEST USER SIGNUP");
	if (send(ClientSocket, Buffer, sizeof(Buffer), 0) == SOCKET_ERROR)
	{
		std::cout << "CLIENT::ERROR::SEND Fail With Message In " << __func__ << std::endl;
		exit(1);
	}

	// ID와 비밀번호를 서버로 전송합니다.
	strcpy_s(Buffer, UserID.c_str());
	if (send(ClientSocket, Buffer, sizeof(Buffer), 0) == SOCKET_ERROR)
	{
		std::cout << "CLIENT::ERROR::SEND Fail With ID In " << __func__ << std::endl;
		exit(1);
	}
	strcpy_s(Buffer, UserPW.c_str());
	if (send(ClientSocket, Buffer, sizeof(Buffer), 0) == SOCKET_ERROR)
	{
		std::cout << "ERROR::SEND Fail With PW In " << __func__ << std::endl;
		exit(1);
	}

	// 서버로부터 회원가입에 대한 결과를 가져옵니다.
	if (recv(ClientSocket, Buffer, sizeof(Buffer), 0) == -1)
	{
		std::cout << "ERROR::RECV Fail In ProcessSignUpMode" << std::endl;
		exit(1);
	}

	// 로그인 결과에 대한 파싱을 수행합니다.
	if (strcmp(Buffer, CalcServer::Result::SignUp::IDALREADY.c_str()) == 0)
	{
		std::cout << std::endl;
		std::cout << "CLIENT::ALARM::ID Already Exist" << std::endl;
		system("pause");

		SetCurrentMode(Mode::MAIN);
	}
	else if (strcmp(Buffer, CalcServer::Result::SignUp::SUCCESS.c_str()) == 0)
	{
		std::cout << std::endl;
		std::cout << "CLIENT::ALARM::SignUp Success" << std::endl;
		system("puase");

		SetCurrentMode(Mode::MAIN);
	}
	else
	{
		std::cout << "CLIENT::ERROR::Invalid Message In " << __func__ << std::endl;
		exit(1);
	}
}

void CalcClient::ProcessCalcMode()
{
	int Value;
	// 계산 UI를 보여줍니다.	
	pClientUI->PrintSquareUI();
	pClientUI->PrintIntInterface();

	// 사용자 입력값을 저장합니다.
	std::cin >> Value;
	
	// 서버에게 정수값을 보낼 것이라고 알립니다.
	strcpy_s(Buffer, "REQUEST INTEGER VALUE");
	if (send(ClientSocket, Buffer, sizeof(Buffer), 0) == SOCKET_ERROR)
	{
		std::cout << "CLIENT::ERROR::SEND Fail With Message In " << __func__ << std::endl;
		exit(1);
	}

	// 서버에게 정수값을 보냅니다.
	strcpy_s(Buffer, std::to_string(Value).c_str());
	if (send(ClientSocket, Buffer, sizeof(Buffer), 0) == SOCKET_ERROR)
	{
		std::cout << "CLIENT::ERROR::SEND Fail With Value In " << __func__ << std::endl;
		exit(1);
	}

	// 서버로부터 계산 결과 메시지와 정수값을 가져옵니다.
	if (recv(ClientSocket, Buffer, sizeof(Buffer), 0) == SOCKET_ERROR)
	{
		std::cout << "CLIENT::ERROR::RECV Fail In " << __func__ << std::endl;
		exit(1);
	}

	// 계산 결과 메시지에 대한 파싱을 수행합니다.
	if (strncmp(Buffer, CalcServer::Result::Calc::SUCCESS.c_str(), 20) == 0)
	{
		// 계산 결과값을 받아옵니다.
		if (recv(ClientSocket, Buffer, sizeof(Buffer), 0) == SOCKET_ERROR)
		{
			std::cout << "CLIENT::ERROR::RECV Fail In " << __func__ << std::endl;
			exit(1);
		}
		std::string IntegerValue = Buffer;
		std::cout << "CLINET::ALARM::Squared Value is " << IntegerValue << std::endl;
		system("pause");

		SetCurrentMode(Mode::MAIN);
	}
	else if (strcmp(Buffer, CalcServer::Result::Calc::NONVALID.c_str()) == 0)
	{
		std::cout << "CLIENT::ALARM::Invalid Input Value" << std::endl;
		system("pause");

		SetCurrentMode(Mode::MAIN);
	}
	else
	{
		std::cout << "CLIENT::ERROR::Invalid Message In " << __func__ << std::endl;
		exit(1);
	}
}

void CalcClient::SetCurrentMode(Mode InMode)
{
	CurrentMode = InMode;
}

CalcClient::Mode CalcClient::GetCurrentMode() const
{
	return CurrentMode;
}

std::string const CalcClient::Request::LOGINFLAG("REQUEST LOGIN FLAG");
std::string const CalcClient::Request::USERLOGIN("REQUEST USER LOGIN");
std::string const CalcClient::Request::USERSIGNUP("REQUEST USER SIGNUP");
std::string const CalcClient::Request::INTEGERVALUE("REQUEST INTEGER VALUE");
std::string const CalcClient::Request::DISCONNECT("REQUEST DISCONNECT");