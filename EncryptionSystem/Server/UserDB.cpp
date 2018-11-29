#include "UserDB.h"
#include "Debug\Debug.h"

#include <iostream>
#include <fstream>



LoginUserInfo::LoginUserInfo()
	: Value(0)
	, bAuthorized(false)
{
}

LoginUserInfo::~LoginUserInfo()
{
	bAuthorized = false;
}

std::string LoginUserInfo::GetID() const
{
	return ID;
}

void LoginUserInfo::Connect(std::string InID)
{
	ID = InID;
	bAuthorized = true;
}

bool LoginUserInfo::Authorized() const
{
	return bAuthorized;
}


////////////////////////////////////////////////////////////////////////////////////////


std::string const DBParser::AttributeType::ID = "ID";
std::string const DBParser::AttributeType::PW = "PW";

DBParser::DBParser()
{
	Attributes.insert(DBParser::AttributeType::ID);
	Attributes.insert(DBParser::AttributeType::PW);

	Delimeter = ':';
}

std::set<DBParser::Attribute> const DBParser::GetAttributes() const
{
	return Attributes;
}

void DBParser::AddAttribute(Attribute InAttribute)
{
	Attributes.insert(InAttribute);
}

void DBParser::RemoveAttribute(Attribute InAttribute)
{
	Attributes.erase(InAttribute);
}

void DBParser::ChangeDelimeter(AtDelimeter InDelimeter)
{
	Delimeter = InDelimeter;
}

void DBParser::ParseUserInfo
(
	std::string InLine, 
	char LineDelimeter,
	std::string& OutID,
	std::string& OutPW
)
{
	// Inline 파라미터의 공백을 제거합니다.
	auto It = std::remove(InLine.begin(), InLine.end(), ' ');
	InLine.erase(It, InLine.end());

	std::istringstream iss(InLine);
	Attribute AttributeIterator;

	//디버그 InLine 파라미터
	//std::cout << "DEBUG::" << InLine << " " << __func__ << std::endl;

	// 구분자를 통해 애트리뷰트를 가져옵니다.
	std::getline(iss, AttributeIterator, Delimeter);

	// ID일 떄 값을 저장합니다.
	if (AttributeIterator == AttributeType::ID)
	{
		// 연결 구분자를 통해 값을 가져옵니다.
		std::getline(iss, AttributeIterator, LineDelimeter);
		OutID = AttributeIterator;

		// 구분자를 통해 PW 애트리뷰트를 가져옵니다.
		std::getline(iss, AttributeIterator, Delimeter);

		if (AttributeIterator == AttributeType::PW)
		{
			// 구분자를 통해 PW 값을 가져옵니다.
			std::getline(iss, AttributeIterator, LineDelimeter);
			OutPW = AttributeIterator;
		}
		else
		{

#ifdef CONSOLE_DEBUG
			std::cout << CON_PARSER_TAG << CON_WARN_TAG <<
				"Attribute Type Does Not Match To PW" << std::endl;
#endif // CONSOLE_DEBUG

		}
	}
	else
	{

#ifdef CONSOLE_DEBUG
		std::cout << CON_PARSER_TAG << CON_WARN_TAG <<
			"Attribute Type Does Not Match To ID" << std::endl;
#endif // CONSOLE_DEBUG

	}
}


/////////////////////////////////////////////////////////////////////////////////////////


UserDB::UserDB() 
	: DatabasePath("CalcUserDatabase.txt")
{
#ifdef CONSOLE_DEBUG
	CON_NEWLINE;
	std::cout << CON_DB_TAG << CON_DEBUG_TAG <<
		__func__ << std::endl;
#endif // CONSOLE_DEBUG


	// 파일로부터 읽어올 버퍼입니다.
	static int const MAXSIZE = 256;
	char Buffer[MAXSIZE];

	// ID와 PW를 담는 묶음 문자열입니다.
	std::string UserInfoBundle;
	char LineDelimeter = '#';

	std::string ResultID;
	std::string ResultPW;

	// 파일 입력 스트림을 생성합니다.
	std::ifstream FileStream;
	FileStream.open("CalcUserDatabase.txt", std::ios::in);

	// 파일이 열려있지 않으면 종료합니다.
	if (FileStream.is_open() == false)
	{
		std::cout << "DBASE::ERROR::File Cannnot Open" << std::endl;
		exit(1);
	}

	while (FileStream.getline(Buffer, MAXSIZE))
	{
#ifdef CON_DB_TAG 
		std::cout << CON_DB_TAG << CON_DEBUG_TAG <<
			"Buffer >> " << Buffer << ">> in " << __func__ << std::endl;
#endif // !CON_DB_TAG


		UserInfoBundle = Buffer;
		UserInfoBundle += LineDelimeter;

		if (FileStream.getline(Buffer, MAXSIZE))
		{
#ifdef CON_DB_TAG 
			std::cout << CON_DB_TAG << CON_DEBUG_TAG <<
				"Buffer >> " << Buffer << ">> in " << __func__ << std::endl;
#endif // !CON_DB_TAG

			// PW 애트리뷰트 및 값을 추가합니다.
			UserInfoBundle.append(Buffer);

			// 번들 DB 파일로부터 ID와 PW를 받아옵니다.
			Parser.ParseUserInfo(UserInfoBundle, LineDelimeter, ResultID, ResultPW);

			// 메모리에 올립니다.
			InsertIDAndPW(ResultID, ResultPW);

			PrintUsersInfo();
		}
	}
}

void UserDB::InsertIDAndPW(std::string InID, std::string InPW)
{
	// 있는 아이디인지 검사합니다.
	bool bExistID = ExistID(InID);
	if (bExistID == false)
	{
		return;
	}

	// 없으면 삽입합니다.
	UsersInfo.insert(std::make_pair(InID, InPW));
}

void UserDB::RemoveIDAndPW(std::string InID)
{
	// 있는 아이디인지 검사합니다.
	bool bExistID = ExistID(InID);
	if (bExistID == false)
	{
		false;
	}

	// 있으면 제거합니다.
	UsersInfo.erase(InID);
}

bool UserDB::FindIDAndPW(std::string InID, std::string InPW)
{
	// 유저가 입력한 ID값으로 찾습니다.
	auto It = UsersInfo.find(InID);
	if (It != UsersInfo.end())
	{
		// 유저가 입력한 값과 DB의 PW값이 같은지 확인합니다.
		if (It->second == InPW)
		{
			return true;
		}
		else
		{

#ifdef CONSOLE_DEBUG
			std::cout << CON_DB_TAG << CON_WARN_TAG <<
				"PW Is Not Same In" << __func__ << std::endl;
#endif // CONSOLE_DEBUG
			return false;
		}
	}
	else
	{

#ifdef CONSOLE_DEBUG
		std::cout << CON_DB_TAG << CON_WARN_TAG <<
			"ID Is Not Same In" << __func__ << std::endl;
#endif // CONSOLE_DEBUG

	}
	return false;
}

void UserDB::PrintUsersInfo() const
{
#ifdef CONSOLE_DEBUG
	std::cout << CON_DB_TAG << CON_DEBUG_TAG << __func__ << std::endl;

	for (auto It = UsersInfo.begin(); It != UsersInfo.end(); ++It)
	{
		std::cout << It->first << "\t";
		std::cout << It->second << std::endl;
	}

#endif // CONSOLE_DEBUG

}

bool UserDB::ExistID(std::string InSearchID) const
{
	auto It = UsersInfo.find(InSearchID);
	if (It == UsersInfo.end() )
	{
		return true;
	}
	return false;
}