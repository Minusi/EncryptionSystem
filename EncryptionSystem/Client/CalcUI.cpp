#include "CalcUI.h"

std::shared_ptr<CalcUI> CalcUI::GetInstance()
{
	static std::shared_ptr<CalcUI> pInstance(new CalcUI());
	return pInstance;
}

void CalcUI::PrintMainUI() const
{
	system("cls");
	std::cout <<
		"============================================\n"
		"=          Welcome to Calc Server!         =\n"
		"============================================\n"
		"\n"
		"============================================\n"
		"=         S   E   R   V   I   C   E         \n"
		"============================================\n"
		"=         1 . L O G I N                    =\n"
		"=         2 . S I G N   U P                =\n"
		"=         3 . C A L C U L A T E            =\n"
		"=         4 . Q U I T                      =\n"
		"============================================\n"
		"\n" 
		<<std::endl;
}

void CalcUI::PrintSignUpUI() const
{
	system("cls");
	std::cout <<
		"============================================\n"
		"=         SingUp Your ID & Password        =\n"
		"============================================\n"
		<< std::endl;
}

void CalcUI::PrintSignInUI() const
{
	system("cls");
	std::cout <<
		"============================================\n"
		"=            SingIn Your Account           =\n"
		"============================================\n"
		<< std::endl;
}

void CalcUI::PrintSquareUI() const
{
	system("cls");
	std::cout <<
		"============================================\n"
		"=       Input Number To Get Number^2       =\n"
		"============================================\n"
		<< std::endl;
}

void CalcUI::PrintIDInterface() const
{
	std::cout << "  ID : ";
}

void CalcUI::PrintPWInterface() const
{
	std::cout << "  PW : ";
}

void CalcUI::PrintCommInterface() const
{
	std::cout << "TEST> ";
}

void CalcUI::PrintIntInterface() const
{
	std::cout << "TEST> ";
}

void CalcUI::PrintDummyInput() const
{
	std::cout << "BLOCK::아무 키나 누르면 진행합니다" << std::endl;
}

void CalcUI::PrintCalcResult(int InValue) const
{
	std::cout << "_CALC::계산된 값 : " << InValue << " 입니다." << std::endl;
}

