#pragma once

#include <iostream>
#include <WinSock2.h>

#include "Client\CalcClient.h"

#include "UserDB.h"



/*
 * 계산 서버 클래스는 클라이언트와의 TCP 통신을 제공하고
 * 전체 통신 루프를 제공합니다.
 * 다른 클래스들에 의존하여 계산 기능, 암호화 기능, 정보 저장 기능을 구현합니다
 */
class CalcServer
{
#define BUFFERSIZE 1024
public:
	enum class Mode
	{
		NONE, CLIENT_LOGIN_FLAG, CLIENT_LOGIN, CLIENT_SIGNUP, CLIENT_CALC
	};

	struct Result
	{
	public:
		struct LoginFlag
		{
		public:
			static std::string const NOLOGIN;
			static std::string const LOGIN;
		};

		struct Login
		{
		public:
			static std::string const IDMISMATCH;
			static std::string const PWMISTMATCH;
			static std::string const SUCCESS;
		};

		struct SignUp
		{
		public:
			static std::string const IDALREADY;
			static std::string const SUCCESS;
		};

		struct Calc
		{
		public:
			static std::string const SUCCESS;
			static std::string const NONVALID;
		};
	};
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

	// 메인 루프를 처리하는 함수입니다,
	void MainLoop();



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

private:
	// 윈도우 TCP 변수
	SOCKET ServerSock, ClientSock;				//**************
	WSADATA WSAData;							//클라이언트 및
	SOCKADDRIN ServerSockInfo, ClientSockInfo;	//서버 소켓 센션
	int SizeOfClientInfo;						//**************

	char Buffer[BUFFERSIZE];					//네트워크 통신 버퍼
	
	UserDB UserDatabase;						//클라이언트 정보
	LoginUserInfo ConnectInfo;					//로그인 유저 정보
};