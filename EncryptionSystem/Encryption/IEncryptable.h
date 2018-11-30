#pragma once

#include <combaseapi.h>
#include <string>



/*
 * 암호화 및 복호화 가능한 클래스입니다.
 * 이것을 상속하는 클래스는 암, 복호화에 필요한 초기화와
 * 암, 복호화 기능을 구현해야합니다.
 */
interface IEncryptable
{
protected:
	// 암호화에 필요한 기초 파라미터들을 초기화합니다.
	virtual void InitializeEncryption() = 0;

public:
	// 주어진 평문을 암호화합니다,
	virtual void Encrypt(std::string& OutCipherText, std::string InPlainText) = 0;
	
	// 주어진 비문을 복호화합니다.
	virtual void Decrypt(std::string& OutPlainText, std::string InCipherText) = 0;
 };

