#include "UserDB.h"

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
	std::string OutID,
	std::string OutPW
)
{
	std::istringstream iss(InLine);
	Attribute AttributeIterator;

	// 구분자를 통해 애트리뷰트를 가져옵니다.
	std::getline(iss, AttributeIterator, Delimeter);

	// AttributeName의 공백을 제거합니다.
	auto It = std::remove(AttributeIterator.begin(), AttributeIterator.end(), ' ');
	AttributeIterator.erase(It, AttributeIterator.end());

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
			std::cout << "ERROR::Invalid Attribute Type in" << __func__ << std::endl;
		}
		return;
	}
	else
	{
		std::cout << "ERROR::Invalid Attribute Type in" << __func__ << std::endl;
		return;
	}
}


/////////////////////////////////////////////////////////////////////////////////////////


UserDB::UserDB() 
{
	UserDB("CalcUserDatabase.txt");
}

UserDB::UserDB(std::string InPath, int Index) 
	: DatabasePath(InPath)
{

	// 파일로부터 읽어올 버퍼입니다.
	static int const MAXSIZE = 1024;
	char Buffer[MAXSIZE];						

	// ID와 PW를 담는 묶음 문자열입니다.
	std::string UserInfoBundle;
	char LineDelimeter = '#';

	std::string ResultID;
	std::string ResultPW;

	// 파일 입력 스트림을 생성합니다.
	std::ifstream FileStream(DatabasePath);		
	while (FileStream.getline(Buffer, MAXSIZE))
	{
		UserInfoBundle = Buffer;
		UserInfoBundle.append(std::to_string(LineDelimeter));

		if (FileStream.getline(Buffer, MAXSIZE))
		{
			// PW 애트리뷰트 및 값을 추가합니다.
			UserInfoBundle.append(Buffer);

			// 번들 DB 파일로부터 ID와 PW를 받아옵니다.
			Parser.ParseUserInfo(UserInfoBundle, LineDelimeter, ResultID, ResultPW);

			// 메모리에 올립니다.
			UsersInfo.insert(ResultID, ResultPW);
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
	UsersInfo.insert(InID, InPW);
}

void UserDB::RemoveIDAndPW(std::string InID)
{
	// 있는 아이디인지 검사합니다.
	bool bExistID = ExistID(InID);
	if (bExistID == false)
	{
		false;
	}

	// 없으면 제거합니다.
	UsersInfo.erase(InID);
}

bool UserDB::FindIDAndPW(std::string InID, std::string InPW)
{
	if (UsersInfo.at(InID) == InPW)
	{
		// TODO : 18년 11월 29일 자료구조를 도와주다 깜빡하다.
	}
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