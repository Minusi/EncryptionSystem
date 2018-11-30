#pragma once

#include <stdlib.h>
#include <WinSock2.h>
#include <iostream>
#include <string>
#include <memory>
#include <cstdio>

#include "NetBase.h"
#include "CalcUI.h"
#include "Server\CalcServer.h"


class CalcClient
{
#define BUFFERSIZE 256
public:
	enum class Mode
	{
		NONE, MAIN, Login, SignUp, Calc
	};
	
	struct Request
	{
	public:
		static std::string const LOGINFLAG;
		static std::string const USERLOGIN;
		static std::string const USERSIGNUP;
		static std::string const INTEGERVALUE;
		static std::string const DISCONNECT;
	};

private:
	typedef sockaddr_in SOCKADDRIN;

public:
	// CalcClient의 생성자입니다.
	CalcClient();	

	// CalcClient의 소멸자입니다.
	~CalcClient();

	// 서버와의 소켓 통신을 위한 초기화를 수행합니다.
	void Init();

	// 서버와의 소켓 통신을 종료하기 위한 절차를 수행합니다.
	void Finalize();

	// 메인 루프를 처리하는 함수입니다.
	void MainLoop();



	void ProcessMode();						// 현재 클라이언트 상태를 처리합니다.
	void ProcessMainMode();					// 메인 모드 상태를 처리합니다.
	void ProcessLoginMode();				// 로그인 모드 상태를 처리합니다.
	void ProcessSignUpMode();				// 회원가입 모드 상태를 처리합니다.
	void ProcessCalcMode();					// 계산 모드 상태를 처리합니다.

	void SetCurrentMode(Mode InMode);		// CurrentMode의 뮤테이터
	Mode GetCurrentMode() const;			// CurrentMode의 액세서

private:
	SOCKET ClientSocket;					//********************
	WSADATA WSAData;						//클라이언트 소켓 섹션
	SOCKADDRIN ClientSockInfo;				//********************

	unsigned short ClientPort;				//******************
											//네트워크 접속 섹션
	std::string ServerIP;					//******************

	NetBuffer Buffer;						//네트워크 통신 버퍼

	std::shared_ptr<CalcUI> pClientUI;		//클라이언트 UI
	Mode CurrentMode;						//클라이언트 현재 모드
};