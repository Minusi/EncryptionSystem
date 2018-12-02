#include "CalcClient.h"
#include <WS2tcpip.h>


CalcClient::CalcClient() 
	: ClientPort(12834)
	, ServerIP("127.0.0.1")
	, pClientUI(CalcUI::GetInstance())
	, CurrentMode(Mode::NONE)
	, bActiveEncrypt(false)
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

	ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ServerSocket == INVALID_SOCKET)
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
	if (connect(ServerSocket, (SOCKADDR*)&ClientSockInfo, sizeof(ClientSockInfo)) == SOCKET_ERROR)
	{
		std::cout << "ERROR::서버 접속 실패 at connect() " << std::endl;
		return;
	}

	MainLoop();
}



void CalcClient::Finalize()
{
	closesocket(ServerSocket);
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
	else if (CurrentMode == Mode::LOGIN)
	{
		ProcessLoginMode();
	}
	else if (CurrentMode == Mode::SIGNUP)
	{
		ProcessSignUpMode();
	}
	else if (CurrentMode == Mode::CALC)
	{
		ProcessCalcMode();
	}
	else if (CurrentMode == Mode::ENCRYPT)
	{
		ProcessEncryptMode();
	}
}



void CalcClient::ProcessMainMode()
{
	pClientUI->PrintMainUI();
	pClientUI->PrintIntInterface();

	enum class Select
	{
		NONE, LOGIN, SIGNUP, CALC, QUIT, ENCRYPT
	};
	int value;
	std::cin >> value;

	Select SelectValue = static_cast<Select>(value);
	


	// 로그인 버튼을 눌렀을 때의 동작입니다.
	if (SelectValue == Select::LOGIN)
	{
		// 서버에게 로그인 정보를 달라고 요청합니다.
		strcpy_s(Buffer, CalcClient::Request::LOGINFLAG.c_str());
		SendToServer();

		// 서버로부터 로그인 정보를 받아옵니다.
		RecvFromServer();

		// 로그인 정보에 대한 파싱을 수행합니다.
		if ((strcmp(Buffer, CalcServer::Respond::LoginFlag::NOLOGIN.c_str())) == 0)
		{
			SetCurrentMode(Mode::LOGIN);
		}
		else if ((strcmp(Buffer, CalcServer::Respond::LoginFlag::LOGIN.c_str())) == 0)
		{
			std::cout << "CLIENT::ALARM::Already Login" << std::endl;
			std::cout << "CLIENT::ALARM::Press Any Key To Move CALC" << std::endl;
			system("pause");
			SetCurrentMode(Mode::CALC);
		}
	}



	// 회원가입 버튼을 눌렀을 때의 동작입니다.
	else if (SelectValue == Select::SIGNUP)
	{
		strcpy_s(Buffer, "REQUEST LOGIN FLAG");

		// 서버에게 로그인 정보를 달라고 요청합니다.
		SendToServer();

		// 서버로부터 로그인 정보를 받아와 파싱을 수행합니다.
		RecvFromServer();
		if ((strcmp(Buffer, CalcServer::Respond::LoginFlag::NOLOGIN.c_str())) == 0)
		{
			SetCurrentMode(Mode::SIGNUP);
		}
		else if ((strcmp(Buffer, CalcServer::Respond::LoginFlag::LOGIN.c_str())) == 0)
		{
			std::cout << "CLIENT::ALARM::Already Login" << std::endl;
			system("pause");
			SetCurrentMode(Mode::MAIN);
		}
	}



	// 계산 버튼을 눌렀을 때의 동작입니다.
	else if (SelectValue == Select::CALC)
	{
		strcpy_s(Buffer, "REQUEST LOGIN FLAG");

		// 서버에게 로그인 정보를 달라고 요청합니다.
		SendToServer();

		// 서버로부터 로그인 정보를 받아와 파싱합니다.
		RecvFromServer();
		if ((strcmp(Buffer, CalcServer::Respond::LoginFlag::NOLOGIN.c_str())) == 0)
		{
			SetCurrentMode(Mode::LOGIN);
		}
		else if ((strcmp(Buffer, CalcServer::Respond::LoginFlag::LOGIN.c_str())) == 0)
		{
			SetCurrentMode(Mode::CALC);
		}

	}



	// 나가기 버튼을 눌렀을 때의 동작입니다.
	else if (SelectValue == Select::QUIT)
	{
		std::cout << "QUIT::Application Will Be Exit" << std::endl;
		strcpy_s(Buffer, CalcClient::Request::DISCONNECT.c_str());
		SendToServer();
		exit(1);
	}



	// 암호화 모드를 눌렀을 떄의 동작입니다.
	else if (SelectValue == Select::ENCRYPT)
	{
		SetCurrentMode(Mode::ENCRYPT);
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
	SendToServer();

	// ID와 비밀번호를 서버로 전송합니다.
	strcpy_s(Buffer, UserID.c_str());
	SendToServer();
	
	strcpy_s(Buffer, UserPW.c_str());
	SendToServer();

	// 서버로부터 로그인에 대한 결과를 가져옵니다.
	RecvFromServer();
	// 로그인 결과에 대한 파싱을 수행합니다.
	if (strcmp(Buffer, CalcServer::Respond::Login::MISMATCH.c_str()) == 0)
	{
		std::cout << std::endl;
		std::cout << "CLIENT::ALARM::Input Mismatch" << std::endl;
		system("pause");
		
		SetCurrentMode(Mode::MAIN);
	}
	else if (strcmp(Buffer, CalcServer::Respond::Login::SUCCESS.c_str()) == 0)
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
	SendToServer();

	// ID와 비밀번호를 서버로 전송합니다.
	strcpy_s(Buffer, UserID.c_str());
	SendToServer();
	strcpy_s(Buffer, UserPW.c_str());
	SendToServer();

	// 서버로부터 회원가입에 대한 결과를 가져옵니다.
	RecvFromServer();
	// 로그인 결과에 대한 파싱을 수행합니다.
	if (strcmp(Buffer, CalcServer::Respond::SignUp::IDALREADY.c_str()) == 0)
	{
		std::cout << std::endl;
		std::cout << "CLIENT::ALARM::ID Already Exist" << std::endl;
		system("pause");

		SetCurrentMode(Mode::MAIN);
	}
	else if (strcmp(Buffer, CalcServer::Respond::SignUp::SUCCESS.c_str()) == 0)
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
	SendToServer();

	// 서버에게 정수값을 보냅니다.
	strcpy_s(Buffer, std::to_string(Value).c_str());
	SendToServer();

	// 서버로부터 계산 결과 메시지와 정수값을 가져옵니다.
	RecvFromServer();

	// 계산 결과 메시지에 대한 파싱을 수행합니다.
	if (strncmp(Buffer, CalcServer::Respond::Calc::SUCCESS.c_str(), 20) == 0)
	{
		// 계산 결과값을 받아옵니다.
		RecvFromServer();

		std::string IntegerValue = Buffer;
		std::cout << "CLIENT::ALARM::Squared Value is " << IntegerValue << std::endl;
		system("pause");

		SetCurrentMode(Mode::MAIN);
	}
	else if (strcmp(Buffer, CalcServer::Respond::Calc::NONVALID.c_str()) == 0)
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

void CalcClient::ProcessEncryptMode()
{
	enum class Select
	{
		NONE, AES, DH, RSA, OPENSSL, EXIT
	};

	// UI를 출력하고 값을 입력 받습니다.
	pClientUI->PrintEncryptUI();
	pClientUI->PrintIntInterface();

	int Value;
	std::cin >> Value;
	
	Select SelectValue = static_cast<Select>(Value);
	Encryptor::EMode ClientEMode;

	// 유저가 선택한 값에 따라서 암호 모듈을 설정합니다.
	if (SelectValue == Select::NONE)
	{	
		ClientEMode = Encryptor::EMode::NONE;
	}
	else if (SelectValue == Select::AES)
	{
		ClientEMode = Encryptor::EMode::AES;
	}
	else if (SelectValue == Select::DH)
	{
		ClientEMode = Encryptor::EMode::DH;
	}
	else if (SelectValue == Select::RSA)
	{
		ClientEMode = Encryptor::EMode::RSA;
	}
	else if (SelectValue == Select::OPENSSL)
	{
		// TODO : 구현할 것
	}
	else if (SelectValue == Select::EXIT)
	{
		SetCurrentMode(Mode::MAIN);
	}
	else
	{
		std::cout << "CLIENT::ERROR::Invalid Select Value in " << __func__ << std::endl;
		exit(1);
	}

	SetEncryptFlagWithEMode(ClientEMode);

	// 서버에게 같은 암호화 방식을 수립해달라고 요청합니다.
	strcpy_s(Buffer, Request::SYNCENCRYPT.c_str());
	SendToServer();

	// 서버에게 현재 사용하는 암호화 알고리즘을 전송합니다.
	std::string EncryptString = std::to_string(static_cast<int>(ClientEMode));
	strcpy_s(Buffer, EncryptString.c_str());
	SendToServer();



	// 서버로부터 암호화 방식 요청에 대한 답변을 수신합니다.
	RecvFromServer();
	if (strcmp(Buffer, CalcServer::Respond::Encrypt::REJECT.c_str()) == 0)
	{
		ClientEncryptor->ChangeModule(Encryptor::EMode::NONE);
		bActiveEncrypt = false;
		
		std::cout << "CLIENT::ALARM::Server Reject Submitted Encryption Method in " << __func__ << std::endl;
		system("pause");
	}
	else if (strcmp(Buffer, CalcServer::Respond::Encrypt::ACCEPT.c_str()) == 0)
	{
		std::cout << "CLINET::ALARM::Server Accept" << std::endl;
		system("pause");
	}
	
	SetCurrentMode(Mode::MAIN);
}

void CalcClient::SetCurrentMode(Mode InMode)
{
	CurrentMode = InMode;
}

CalcClient::Mode CalcClient::GetCurrentMode() const
{
	return CurrentMode;
}

const SOCKET * CalcClient::GetServerSocket() const
{
	return &ServerSocket;
}

void CalcClient::SetEncryptFlagWithEMode(Encryptor::EMode InEMode)
{
	if (InEMode == Encryptor::EMode::NONE)
	{
		bActiveEncrypt = false;
		ClientEncryptor->ChangeModule(InEMode);
	}
	else
	{
		bActiveEncrypt = true;
		ClientEncryptor->ChangeModule(InEMode);
	}
}

void CalcClient::WriteBufferWithMode(std::string InText)
{
	// 클라이언트가 암호화 모드일 경우입니다.
	if (bActiveEncrypt == true)
	{
		std::string CipherText;

		ClientEncryptor->ToCipher(CipherText, InText);
		strcpy_s(Buffer, CipherText.c_str());

		return;
	}

	// 아무런 모드가 적용되지 않았을 때 경우입니다.
	strcpy_s(Buffer, InText.c_str());
}

std::string CalcClient::ReadBufferWIthMode()
{
	std::string FinalString;

	// 클라이언트가 암호화 모드일 경우입니다.
	if (bActiveEncrypt == true)
	{
		ClientEncryptor->ToPlain(FinalString, Buffer);
		return FinalString;
	}

	return Buffer;
}

void CalcClient::SendToServer()
{
	if (send(ServerSocket, Buffer, sizeof(Buffer), 0) == SOCKET_ERROR)
	{
		std::cout << "CLIENT::ERROR::SEND Fail With Message In " << __func__ << std::endl;
		exit(1);
	}
}

void CalcClient::RecvFromServer()
{
	if (recv(ServerSocket, Buffer, sizeof(Buffer), 0) == SOCKET_ERROR)
	{
		std::cout << "ERROR::RECV Fail In ProcessMainMode" << std::endl;
		exit(1);
	}
}
