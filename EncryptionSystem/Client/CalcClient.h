#pragma once

#include <stdlib.h>
#include <WinSock2.h>
#include <iostream>
#include <string>
#include <memory>
#include <cstdio>

#include "Encryption\Encryptor.h"
#include "NetBase.h"
#include "CalcUI.h"
#include "Server\CalcServer.h"



class CalcClient
{
public:
	enum class Mode
	{
		NONE, MAIN, LOGIN, SIGNUP, CALC, ENCRYPT
	};
	
	struct Request
	{
	public:
		static std::string const LOGINFLAG;
		static std::string const USERLOGIN;
		static std::string const USERSIGNUP;
		static std::string const INTEGERVALUE;
		static std::string const DISCONNECT;
		static std::string const SYNCENCRYPT;
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

	void SetCurrentMode(Mode InMode);		// CurrentMode의 뮤테이터
	Mode GetCurrentMode() const;			// CurrentMode의 액세서

	const SOCKET* GetServerSocket() const;	// 서버 소켓에 대한 참조를 반환합니다.

private:
	// Encryptor의 초기화 수행 및 암호화 모드에 따라 T/F를 설정합니다.
	// ClientEncryptor를 사용하기 위해서는 반드시 이 멤버 함수를 통해서
	// 초기화가 이루어진 뒤여야 합니다.
	void SetEncryptFlagWithEMode(Encryptor::EMode InEMode);

	// 클라이언트 모드에 따른 버퍼 쓰기를 수행합니다.				
	void WriteBufferWithMode(std::string InText);

	// 클라이언트 모드에 따른 버퍼 읽기를 수행합니다.
	std::string ReadBufferWIthMode();

	void SendToServer();					// 서버로 메시지를 보냅니다.
	void RecvFromServer();					// 서버로부터 메시지를 받습니다.



	void ProcessMainMode();					// 메인 모드 상태를 처리합니다.
	void ProcessLoginMode();				// 로그인 모드 상태를 처리합니다.
	void ProcessSignUpMode();				// 회원가입 모드 상태를 처리합니다.
	void ProcessCalcMode();					// 계산 모드 상태를 처리합니다.
	void ProcessEncryptMode();				// 암호화 모드 상태를 처리합니다.

private:
	SOCKET ServerSocket;					//********************
	WSADATA WSAData;						//클라이언트 소켓 섹션
	SOCKADDRIN ClientSockInfo;				//********************

	unsigned short ClientPort;				//******************
											//네트워크 접속 섹션
	std::string ServerIP;					//******************

	char Buffer[BUFFERSIZE];				//네트워크 통신 버퍼

	std::shared_ptr<CalcUI> pClientUI;		//클라이언트 UI
	Mode CurrentMode;						//클라이언트 현재 모드

	//////////////////////
	// 통신 암호화 모듈 //
	//////////////////////
	std::unique_ptr<class Encryptor> ClientEncryptor;
	bool bActiveEncrypt;
};