#include "EncryptionBase.h"

#include <iostream>

#include "Debug\Debug.h"



EncryptionBase::EncryptionBase()
	: NetComponent{ new NoNetAvailable() }
{
}

void EncryptionBase::InitializeNetDelegate()
{
#ifdef CONSOLE_DEBUG
	std::cout << CLASS_TAG << CON_ALARM_TAG <<
		__func__ << " Call" << std::endl;
#endif // CONSOLE_DEBUG

	return;
}

void EncryptionBase::InitializeEncryption()
{
#ifdef CONSOLE_DEBUG
	std::cout << CLASS_TAG << CON_ALARM_TAG << 
		__func__ << " Call" << std::endl;
#endif // CONSOLE_DEBUG

	return;
}

void EncryptionBase::Encrypt(std::string & OutCipherText, std::string InPlainText)
{
#ifdef CONSOLE_DEBUG
	std::cout << CLASS_TAG << CON_ALARM_TAG <<
		__func__ << " Call" << std::endl;
#endif // CONSOLE_DEBUG

	return;
}

void EncryptionBase::Decrypt(std::string &OutPlainText, std::string InCipherText)
{
#ifdef CONSOLE_DEBUG
	std::cout << CLASS_TAG << CON_ALARM_TAG <<
		__func__ << " Call" << std::endl;
#endif // CONSOLE_DEBUG

	return;
}
