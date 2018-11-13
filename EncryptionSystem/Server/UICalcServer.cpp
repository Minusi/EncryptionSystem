#include "UICalcServer.h"



UICalcServer::UICalcServer()
{
}


UICalcServer::~UICalcServer()
{
}

std::string UICalcServer::GetMainInterface()
{
	std::string ReturnString;
	ReturnString =
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
		"============================================\n"
		"\n";
	return ReturnString;
}

std::string UICalcServer::GetLoginInterface()
{
	std::string ReturnString;
	ReturnString =
		"CALC :: Input Your ID & Password to Login \n";
	return ReturnString;
}

std::string UICalcServer::GetCalcInterface()
{
	std::string ReturnString;
	ReturnString =
		"CALC :: Input Integer To Calculate Pow of 2 \n";
	return std::string();
}

std::string UICalcServer::GetSignUpInterface()
{
	std::string ReturnString;
	ReturnString =
		"CALC :: Input Your ID & Password to Sign up \n";
	return ReturnString;
}

std::string UICalcServer::GetUserIDInterface()
{
	std::string ReturnString;
	ReturnString =
		"  UserID : ";
	return ReturnString;
}

std::string UICalcServer::GetUserPWInterface()
{
	std::string ReturnString;
	ReturnString =
		"  UserPW : ";
	return ReturnString;
}

std::string UICalcServer::GetCalcIntegerInterface()
{
	std::string ReturnString;
	ReturnString =
		"  Integer : ";
	return ReturnString;
}

std::string UICalcServer::GetCalcValueInterface()
{
	std::string ReturnString;
	ReturnString =
		"CALC :: Return Value Is ";
	return ReturnString;
}

