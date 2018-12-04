#include "CalcServer.h"
#include "Debug\Debug.h"


CalcServer::CalcServer()
	: bActiveEncrypt(false)
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
	MAC_PRINTLINE;

	std::string RecvedStr;
	RecvFromClient();

	RecvedStr = ReadBufferWIthMode();


#ifdef CONSOLE_DEBUG
	CON_NEWLINE;
	std::cout << CON_SERVER_TAG << CON_DEBUG_TAG <<
		"Buffer >> " << Buffer << " In " << __func__ << std::endl;
#endif // CONSOLE_DEBUG


	// 클라이언트가 로그인 여부를 확인할 때 처리합니다.
	if (RecvedStr.compare(CalcClient::Request::LOGINFLAG) == 0)
	{
		ProcessLoginFlag();
	}
	// 클라이언트가 로그인 절차를 수행한다고 하면 처리합니다.
	else if (RecvedStr.compare(CalcClient::Request::USERLOGIN) == 0)
	{
		ProcessUserLogin();
	}
	// 클라이언트가 회원가입 프로세스를 진행한다고 하면 처리합니다.
	else if (RecvedStr.compare(CalcClient::Request::USERSIGNUP) == 0)
	{
		ProcessUserSignUp();
	}
	// 클라이언트가 처리할 정수값을 보내오면 처리합니다.
	else if (RecvedStr.compare(CalcClient::Request::INTEGERVALUE) == 0)
	{
		ProcessIntegerValue();
	}
	else if (RecvedStr.compare(CalcClient::Request::SYNCENCRYPT) == 0)
	{
		ProcessEncryptMode();
	}
	// 클라이언트가 종료 플래그를 보내오면 종료합니다.
	// TODO : 다중 유저 접속에 맞지 않는 코드이므로 그러한 요구가 발생하면 
	// 코드를 재구성할 것
	else if (RecvedStr.compare(CalcClient::Request::DISCONNECT) == 0)
	{
		std::cout << "SERVER::DISCONNECT" << std::endl;
		exit(1);
	}
	// 클라이언트가 전송한 잘못된 값에 대한 예외 처리? 유사 어서트? 수행
	else
	{
		std::cout << "SERVER::ERROR::Unknown Value in " << __func__ << std::endl;
		std::cout << "SERVER::ERROR::Buffer >> " << Buffer << std::endl;
		exit(1);
	}
}





void CalcServer::ProcessLoginFlag()
{
	// 로그인 여부를 검사합니다.
	if (ConnectInfo.Authorized())
	{
		WriteBufferWithMode(CalcServer::Respond::LoginFlag::LOGIN);
	}
	else
	{
		WriteBufferWithMode(CalcServer::Respond::LoginFlag::NOLOGIN);
	}

#ifdef CONSOLE_DEBUG
	std::cout << CON_SERVER_TAG << CON_DEBUG_TAG <<
		"Before Send Buffer >> " << Buffer << " In " << __func__ << std::endl;
#endif // CONSOLE_DEBUG


	// 클라이언트로 현재 로그인 세션 유지 여부를 전송합니다.
	SendToClient();
}





void CalcServer::ProcessUserLogin()
{
	std::string NeedToCheckID;
	std::string NeedToCheckPW;

	// 클라이언트로부터 ID와 PW를 수신합니다.
	RecvFromClient();
	NeedToCheckID = ReadBufferWIthMode();

	RecvFromClient();
	NeedToCheckPW = ReadBufferWIthMode();

	// 데이터베이스를 확인합니다.
	bool bSuccessLogin = UserDatabase.FindIDAndPW(NeedToCheckID, NeedToCheckPW);
	if (bSuccessLogin == true)
	{
		ConnectInfo.Connect(NeedToCheckID);
		WriteBufferWithMode(CalcServer::Respond::Login::SUCCESS);
	}
	else
	{
		WriteBufferWithMode(CalcServer::Respond::Login::MISMATCH);
	}


#ifdef CONSOLE_DEBUG
	std::cout << CON_SERVER_TAG << CON_DEBUG_TAG <<
		"Before Send Buffer >> " << Buffer << " In " << __func__ << std::endl;
#endif // CONSOLE_DEBUG


	// 클라이언트로 로그인 결과를 전송합니다.
	SendToClient();
}





void CalcServer::ProcessUserSignUp()
{
	std::string NeedToCheckID;
	std::string NeedToCheckPW;
	
	// 클라이언트로부터 ID와 PW를 수신합니다.
	RecvFromClient();
	NeedToCheckID = ReadBufferWIthMode();

	RecvFromClient();
	NeedToCheckPW = ReadBufferWIthMode();

	// 아이디가 있는지 확인합니다.
	bool bNoExistID = UserDatabase.ExistID(NeedToCheckID);
	if (bNoExistID == true)
	{
		WriteBufferWithMode(CalcServer::Respond::SignUp::SUCCESS);

		// 데이터베이스에 등록합니다.
		UserDatabase.InsertIDAndPW(NeedToCheckID, NeedToCheckPW);
	}
	else
	{
		WriteBufferWithMode(CalcServer::Respond::SignUp::IDALREADY);
	}

#ifdef CONSOLE_DEBUG
	std::cout << CON_SERVER_TAG << CON_DEBUG_TAG <<
		"Before Send Buffer >> " << Buffer << " In " << __func__ << std::endl;
#endif // CONSOLE_DEBUG


	// 클라이언트로 회원가입 결과를 전송합니다.
	SendToClient();
}





void CalcServer::ProcessIntegerValue()
{
	std::string RecvedStr;

	// 로그인이 인증되었는지 확인합니다.
	if (ConnectInfo.Authorized() == false)
	{
		std::cout << "SERVER::ERROR::Invalid ConnectInfo in " << __func__ << std::endl;
		exit(1);
	}

	// 클라이언트로부터 정수값을 수신합니다.
	RecvFromClient();

#ifdef CONSOLE_DEBUG
	std::cout << CON_SERVER_TAG << CON_DEBUG_TAG <<
		"After Recv Buffer >> " << Buffer << " In " << __func__ << std::endl;
#endif // CONSOLE_DEBUG

	// 클라이언트의 값을 정수로 변환합니다.
	int ReceivedValue = std::stoi(ReadBufferWIthMode());
	int SquareValue;

	SquareValue = static_cast<int>(std::pow(ReceivedValue, 2));

	// 문자열 값이 유효한지 아닌지 검사하고 그 결과를 기록합니다.
	std::string StringValue = std::to_string(SquareValue);
	if (StringValue.empty())
	{
		WriteBufferWithMode(Respond::Calc::NONVALID);
	}
	else
	{
		WriteBufferWithMode(Respond::Calc::SUCCESS);
	}

#ifdef CONSOLE_DEBUG
	std::cout << CON_SERVER_TAG << CON_DEBUG_TAG <<
		"Before Send Buffer >> " << Buffer << " In " << __func__ << std::endl;
#endif // CONSOLE_DEBUG

	// 문자열 유효에 대한 결과값을 전송합니다.
	SendToClient();

	// 계산이 성공했으면 계산 결과값도 전송합니다.
	RecvedStr = ReadBufferWIthMode();
	if (RecvedStr == Respond::Calc::SUCCESS)
	{
		std::string ReturnStr = std::to_string(SquareValue);
		WriteBufferWithMode(ReturnStr);
		SendToClient();
	}

#ifdef CONSOLE_DEBUG
	else
	{
		std::cout << CON_SERVER_TAG << CON_ERROR_TAG <<
			"Buffer String Mismatch In " << __func__ << std::endl;
	}
#endif // CONSOLE_DEBUG
}





void CalcServer::ProcessEncryptMode()
{

	std::string RecvedStr;

	// 클라이언트로부터 사용하는 암호화 알고리즘을 수신하고 파싱하여 결과값을 캐시합니다.
	RecvFromClient();
	Encryptor::EMode ServerEMode;
	ServerEMode = static_cast<Encryptor::EMode>(std::stoi(ReadBufferWIthMode()));


	// TODO : 추후에 서버가 클라이언트의 이러한 요청을 거부하게 될 경우,
	// 이 곳에서 그 코드를 작성하기 바람.
	// 클라이언트에게 요청 수립 메시지를 보낸다,
	WriteBufferWithMode(Respond::Encrypt::ACCEPT);
	SendToClient();
	


	// 클라이언트로부터 받은 암호화 모듈을 설정합니다.
	SetEncryptFlagWithEMode(ServerEMode);
}

const SOCKET * CalcServer::GetClientSocket()
{
	return &ClientSock;
}

void CalcServer::SetEncryptFlagWithEMode(Encryptor::EMode InEMode)
{
	// 암호자 모듈이 초기화되어 있지 않으면 초기화를 수행합니다.
	if (ServerEncryptor == nullptr)
	{
		ServerEncryptor.reset(new Encryptor(&ClientSock));
	}

	// NONE		: 암호화 모듈의 암호화 사용을 해제합니다.
	// ELSE..	: 해당 암호화 알고리즘으로 암호화 모듈이 사용합니다. 
	if (InEMode == Encryptor::EMode::NONE)
	{
		bActiveEncrypt = false;
		ServerEncryptor->ChangeModule(InEMode);

		return;
	}
	else if (InEMode == Encryptor::EMode::DH)
	{
		bActiveEncrypt = true;
		ServerEncryptor->CoInitNetworkMode(EncryptionBase::NetMode::HOST);
		ServerEncryptor->ChangeModule(InEMode);
	}
	else if (InEMode == Encryptor::EMode::RSA)
	{
		bActiveEncrypt = true;
		ServerEncryptor->CoInitNetworkMode(EncryptionBase::NetMode::HOST);
		ServerEncryptor->ChangeModule(InEMode);
	}
	else
	{
		bActiveEncrypt = true;
		ServerEncryptor->ChangeModule(InEMode);
	}
	
	MAC_PRNT_MSG_WITH_PAD("CheckMate", 1);
}

void CalcServer::WriteBufferWithMode(std::string InText)
{
	// 클라이언트가 암호화 모드일 경우입니다.
	if (bActiveEncrypt == true)
	{
		std::string CipherText;

		ServerEncryptor->ToCipher(CipherText, InText);
		strcpy_s(Buffer, CipherText.c_str());

		return;
	}

	// 바닐라 모드인 경우입니다.
	strcpy_s(Buffer, InText.c_str());
}

std::string CalcServer::ReadBufferWIthMode()
{
	// 암호화 모드에서 암호 모듈에 대한 유효성 검사를 수행합니다.
	if ((bActiveEncrypt == true) && (ServerEncryptor == nullptr))
	{
		std::cout << "SERVER::ERROR::ENCRYPTOR Does Not Exist";
		exit(1);
	}


	std::string FinalString;

	// 클라이언트가 암호화 모드일 경우입니다.
	if (bActiveEncrypt == true)
	{
		ServerEncryptor->ToPlain(FinalString, Buffer);
		return FinalString;
	}

	// 바닐라 모드인 경우입니다.
	return Buffer;
}

void CalcServer::SendToClient()
{
	if (send(ClientSock, Buffer, sizeof(Buffer), 0) == SOCKET_ERROR)
	{
		std::cout << "SERVER::ERROR::SEND Fail With Message In " << __func__ << std::endl;
		exit(1);
	}
}

void CalcServer::RecvFromClient()
{
	if (recv(ClientSock, Buffer, sizeof(Buffer), 0) == SOCKET_ERROR)
	{
		std::cout << "SERVER::ERROR::RECV Fail In " << __func__ << std::endl;
		exit(1);
	}
}
