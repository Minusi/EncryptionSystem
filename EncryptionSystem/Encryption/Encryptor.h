#pragma once

#include <string>
#include <memory>

#include "IEncryptable.h"
#include "DH\DHEncryption.h"
#include "RSA\RSAEncryption.h"



/*
 * Encryptor 클래스는 암호화를 수행하기 위한 클래스입니다.
 * 암호화를 위한 각종 설정들을 제공하고, 암호화 기능을 제공합니다.
 */
class Encryptor
{
public:
	/*
	 * Encyptor::Mode 클래스는 Encryptor의 모듈을
	 * 변경하기 위해 사용하는 열거형 클래스입니다.
	 * 현재 다음 멤버 함수에서 사용됩니다.
	 *
	 * ChangeModule()
	 */
	enum class Mode
	{
		NONE, AES, DH, RSA
	};
public:
	// Encryptor 생성자입니다.
	Encryptor();

	// Encryptor 소멸자입니다.
	~Encryptor();

	// 입력한 모드를 가진 암호 모듈로 교체합니다.
	void ChangeModule(Mode InMode);

private:
	std::unique_ptr<IEncryptable> EncryptModule;
};

