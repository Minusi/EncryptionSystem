#include "UserInfo.h"



UserInfo::UserInfo() 
	: Value{ 0 }
{
}

UserInfo::UserInfo(std::string InID, std::string InPW, int Value)
	: ID(InID)
	, PW(InPW)
	, Value(0)
{
}

bool UserInfo::IsValid()
{
	return ID.empty();
}

std::string UserInfo::GetID() const
{
	return ID;
}

void UserInfo::SetID(std::string InID)
{
	ID = InID;
}

std::string UserInfo::GetPW() const
{
	return PW;
}

void UserInfo::SetPW(std::string InPW)
{
	PW = InPW;
}

int UserInfo::GetValue() const
{
	return Value;
}

void UserInfo::SetValue(std::string InValue)
{
}
