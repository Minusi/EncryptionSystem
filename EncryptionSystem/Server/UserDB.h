#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include <set>
#include <algorithm>



/*
 * 현재 로그인한 유저에 대한 정보를 보유하는 클래스입니다.
 * 로그인한 유저는 이미 인증을 거친 상태이므로
 * 이전에 입력한 정수값과 ID만을 가집니다.
 */
class LoginUserInfo 
{
public:
	// LoginUserInfo 생성자입니다.
	LoginUserInfo();

	// LoginUserInfo 소멸자입니다.
	~LoginUserInfo();

	// ID를 반환합니다.
	std::string GetID() const;

	// 클라이언트 유저 계정과 연결을 설정합니다.
	void Connect(std::string InID);

	// 인증되었는지 여부를 반환합니다.
	bool Authorized() const;

private:
	std::string ID;
	int Value;
	bool bAuthorized;
};



/*
 * DBParser는 DB 파일을 받아서 애트리뷰트와 필드를 구분하는 클래스입니다.
 * 구분되는 애트리뷰트들은 다음과 같습니다.
 * 
 * ID : # 아이디 관련 정보입니다.
 * PW : # 패스워드 관련 정보입니다.
 */
class DBParser
{	
public:
	typedef std::string Attribute;
	typedef char AtDelimeter;

	class AttributeType sealed
	{
	public:
		AttributeType() = delete;

		static std::string const ID;
		static std::string const PW;
	};

public:
	// DBParser의 생성자입니다.
	// 애트리뷰트 셋과 구분자를 초기화합니다.
	DBParser();
	
	// 애트리뷰트 세트를 반환합니다. 이것으로 조작을 수행하지 마시오.
	// 읽기 전용입니다.
	std::set<Attribute> const GetAttributes() const;

	// 애트리뷰트 세트에 애트리뷰트를 추가합니다.
	void AddAttribute(Attribute InAttribute);

	// 애트리뷰트 세트에 저장된 애트리뷰트를 제거합니다.
	void RemoveAttribute(Attribute InAttritube);

	// 구분 글자를 변경합니다.
	void ChangeDelimeter(AtDelimeter InDelimeter);

	// 주어진 키워드로 파싱을 수행합니다.
	void ParseUserInfo
	(
		std::string InAttribute, 
		char LineDelimeter, 
		std::string& OutID, 
		std::string& OutPW
	);
	
private:
	// 애트리뷰트 세트입니다.
	std::set<Attribute> Attributes;

	// 애트리뷰트와 필드를 구분하는 구분자입니다.
	AtDelimeter Delimeter;
};



/*
 * 유저의 데이터베이스 파일을 보관하고 있는 클래스입니다.
 * 파일로부터 유저의 정보를 읽어들여야 이를 저장하는 기능을 수행합니다.
 * 이 때 파싱은 DBParser 클래스가 수행합니다.
 */
class UserDB
{
private:
	typedef std::string Path;

public:
	// UserDB의 생성자입니다.
	UserDB();

	// UserDB의 경로 지정 오버로딩 생성자입니다.
	UserDB(std::string InPath, int Index = 0);

public:
	// ID와 PW를 DB에 삽입합니다.
	void InsertIDAndPW(std::string InID, std::string InPW);

	// ID와 PW를 DB에서 제거합니다.
	void RemoveIDAndPW(std::string InID);

	// ID가 이미 DB에 있는 지 확인합니다.
	bool ExistID(std::string InSearchID) const;

	// ID와 PW가 DB에 있는 지 확인합니다.
	bool FindIDAndPW(std::string InID, std::string InPW);

	// 모든 ID와 PW를 출력합니다.
	void PrintUsersInfo() const;

private:
	// 파일로부터 읽어들여온 유저 ID와 PW에 대한 자료입니다.
	std::map<std::string, std::string> UsersInfo;

	// 데이터베이스 파일 위치입니다.
	Path DatabasePath;

	// DB파서입니다.
	DBParser Parser;
};