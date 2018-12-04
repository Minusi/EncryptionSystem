#pragma once

#include <iostream>
#include <string>
#include <cmath>
#include <memory>
#include <WinSock2.h>


#include "Encryption\Encryptor.h"
#include "NetBase.h"
#include "Client\CalcClient.h"
#include "UserDB.h"
#include "Debug\Macro.h"



/*
 * 계산 서버 클래스는 클라이언트와의 TCP 통신을 제공하고
 * 전체 통신 루프를 제공합니다. 이것은 명백히 단일 책임 원칙에 위배되지만
 * 이것을 위반해서라도 빠르게 끝내고 싶다는 점에 대해서 양해를 부탁드립니다.
 * 다른 클래스들에 의존하여 계산 기능, 암호화 기능, 정보 저장 기능을 구현합니다
 */
class CalcServer
{
public:
	enum class Mode
	{
		NONE, CLIENT_LOGIN_FLAG, CLIENT_LOGIN, CLIENT_SIGNUP, CLIENT_CALC
	};

	struct Respond
	{
	public:
		Respond() = delete;
		struct LoginFlag
		{
		public:
			LoginFlag() = delete;
			static std::string const NOLOGIN;
			static std::string const LOGIN;
		};

		struct Login
		{
		public:
			Login() = delete;
			static std::string const MISMATCH;
			static std::string const SUCCESS;
		};

		struct SignUp
		{
		public:
			SignUp() = delete;
			static std::string const IDALREADY;
			static std::string const SUCCESS;
		};

		struct Calc
		{
		public:
			Calc() = delete;
			static std::string const SUCCESS;
			static std::string const NONVALID;
		};

		struct Encrypt
		{
		public:
			Encrypt() = delete;
			static std::string const ACCEPT;
			static std::string const REJECT;
		};
	};

private:
	typedef sockaddr_in SOCKADDRIN;

public:
	// 계산 서버의 생성자입니다. 내부적으로 Init()을 수행합니다.
	CalcServer();

	// 계산 서버의 소멸자입니다. 내부적으로 Finalize()를 수행합니다.
	~CalcServer();

	// 클라이언트 소켓에 대한 참조를 반환합니다.
	const SOCKET* GetClientSocket();

private:
	// 소켓 통신을 위한 초기화를 수행합니다.
	void Init();

	// 소켓 통신을 종료히가 위한 마무리를 수행합니다.
	void Finalize();

	// 메인 루프를 처리하는 함수입니다,
	void MainLoop();



	// Encryptor의 초기화 수행 및 암호화 모드에 따라 T/F를 설정합니다.
	// ServerEncryptor를 사용하기 위해서는 반드시 이 멤버 함수를 통해서
	// 초기화가 이루어진 뒤여야 합니다.	
	void SetEncryptFlagWithEMode(Encryptor::EMode InEMode);

	// 클라이언트 모드에 따른 버퍼 쓰기를 수행합니다.				
	void WriteBufferWithMode(std::string InText);

	// 클라이언트 모드에 따른 버퍼 읽기를 수행합니다.
	std::string ReadBufferWIthMode();

	// 클라이언트로 메시지를 보냅니다.
	void SendToClient();					

	// 클라이언트로부터 메시지를 받습니다.
	void RecvFromClient();					



	// 클라이언트에서 들어온 데이터를 분석하고 처리합니다.
	void ParseClientData();

	// 클라이언트 로그인 여부를 처리합니다.
	void ProcessLoginFlag();

	// 클라이언트 로그인 프로세스를 진행합니다.
	void ProcessUserLogin();
	
	// 클라이언트 회원가입 프로세스를 처리합니다.
	void ProcessUserSignUp();

	// 클라이언트에서 보내온 계산해야할 정수값을 처리합니다.
	void ProcessIntegerValue();
	
	// 클라이언트에서 보내온 암호화 방ㅇ식에 대해 처리합니다.
	void ProcessEncryptMode();

private:
	/////////////////////
	// 윈도우 TCP 변수 //
	/////////////////////
	SOCKET ServerSock, ClientSock;				//**************
	WSADATA WSAData;							//클라이언트 및
	SOCKADDRIN ServerSockInfo, ClientSockInfo;	//서버 소켓 센션
	int SizeOfClientInfo;						//**************

	char Buffer[BUFFERSIZE];					//네트워크 통신 버퍼
	
	///////////////////////
	// 유저 데이터베이스 //
	///////////////////////
	UserDB UserDatabase;						//클라이언트 정보
	LoginUserInfo ConnectInfo;					//로그인 유저 정보

	//////////////////////
	// 통신 암호화 모듈 //
	//////////////////////
	std::unique_ptr<class Encryptor> ServerEncryptor;
	bool bActiveEncrypt;
	
};