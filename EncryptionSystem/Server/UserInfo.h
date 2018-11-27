#pragma once

#include <string>



class UserInfo
{
public:
	// UserInfo의 생성자입니다.
	UserInfo();

	// UserInfo의 오버로딩 생성자입니다.
	UserInfo(std::string InID, std::string InPW, int InValue);

	// 유저 정보가 유효한지 확인합니다.
	bool IsValid();



	std::string GetID() const;							// ID 액세서
	void SetID(std::string InID);						// ID 뮤테이터

	std::string GetPW() const;							// PW 액세서
	void SetPW(std::string InPW);						// PW 뮤테이터

	int GetValue() const;								// Value 액세서
	void SetValue(std::string InValue);					// Value 뮤테이터

private:
	std::string ID;
	std::string PW;
	int Value;
};