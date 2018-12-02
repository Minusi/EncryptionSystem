#pragma once

#include <iostream>
#include <cstdlib>
#include <string>
#include <memory>



/* CalcUI 클래스는 사용자 단에서 사용자 인터페이스를 구현하며,
 * 사용자의 입력 및 화면 출력을 담당하는 클래스입니다.
 */
class CalcUI
{
private:
	CalcUI(){ }

public:
	static std::shared_ptr<CalcUI> GetInstance();				// 싱글턴 구현부입니다.
	
public:
	void PrintMainUI() const;			// 메인 UI를 출력합니다.
	void PrintSignUpUI() const;			// 회원가입 UI를 출력합니다.
	void PrintSignInUI() const;			// 로그인 UI를 출력합니다.
	void PrintSquareUI() const;			// 제곱 UI를 출력합니다.
	void PrintEncryptUI() const;		// 암호화 UI를 출력합니다.
	
	void PrintIDInterface() const;		// ID 입력을 받는 인터페이스를 출력합니다.
	void PrintPWInterface() const;		// PW 입력을 받는 인터페이스를 출력합니다.
	void PrintCommInterface() const;	// 정수 입력을 받는 인터페이스를 출력합니다.
	void PrintIntInterface() const;		// 버튼 입력을 받는 인터페이스를 출력합니다.

	void PrintDummyInput() const;		// 아무 키나 누를 때까지 블록합니다.
	void PrintCalcResult(int InValue) const;		// 계산 결과를 출력합니다.
};

