#pragma once
#include <iostream>
#include <string>



class UICalcServer
{
public:
	// UICalcServer의 생성자입니다.
	UICalcServer();

	// UICalcServer의 소멸자입니다.
	~UICalcServer();

	// 계산 서버에서 접속시 출력할 인터페이스입니다.
	std::string GetMainInterface();

	// 로그인 시에 출력할 인터페이스입니다.
	std::string GetLoginInterface();

	// 계산 시에 출력할 인터페이스입니다.
	std::string GetCalcInterface();

	// 회원가입 시에 출력할 인터페이스입니다.
	std::string GetSignUpInterface();

	// 유저가 ID를 입력하는 창에서 출력할 인터페이스입니다.
	std::string GetUserIDInterface();

	// 유저가 PW를 입력하는 창에서 출력할 인터페이스입니다.
	std::string GetUserPWInterface();

	// 유저가 정수를 입력하는 창에서 출력할 인터페이스입니다.
	std::string GetCalcIntegerInterface();

	// 유저가 정수를 입력한 결과를 출력할 인터페이스입니다.
	std::string GetCalcValueInterface();
};

