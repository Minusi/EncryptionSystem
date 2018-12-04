#pragma once

#include <string>
#include <memory>
#include <iostream>

#include "EncryptionBase.h"
#include "DH\DHEncryption.h"
#include "RSA\RSAEncryption.h"
#include "Symmetric\AESEncryption.h"

#include "NetBase.h"
#include "Debug\Macro.h"



/*
 * Encryptor 클래스는 암호화를 수행하기 위한 클래스입니다.
 * 암호화를 위한 각종 설정들을 제공하고, 암호화 기능을 제공합니다.
 *
 * 좀 더 책임범위에 대해 자세히 얘기하자면 다음과 같습니다:
 * Encryptor 클래스는 네트워크 호스트와 암호 모듈 사이를 중재해주는
 * 중재자 클래스입니다. 각각의 암호 모듈은 인자를 충분히 받으면,
 * 암호화 및 복호화에 필요한 기능을 수행할 수 있습니다. 대개 암호적
 * 기능의 경우 EncryptionBase 상속 계층에서 자급자족할 수 있지만
 * 네트워크 기능의 경우 Server나 Client 클래스에서 가지고 있는 자원을
 * 참조해야 하는 경우가 많습니다. 이런 인자들을 Encryptor 클래스에서
 * 공급해주고 반대로 Server나 Client에서 필요로 하는 구체적인 암호화
 * 알고리즘을 제공해줍니다.
 */
class Encryptor
{
public:
	/*
	 * Encyptor::EMode 클래스는 Encryptor의 모듈을
	 * 변경하기 위해 사용하는 열거형 클래스입니다.
	 * 현재 다음 멤버 함수에서 사용됩니다.
	 *
	 * ChangeModule()
	 */
	enum class EMode
	{
		NONE, AES, DH, RSA, OPENSSL
	};
public:
	// Encryptor 생성자입니다.
	// 지정한 모드로 암호 모듈을 생성하고 초기화를 수행합니다.
	Encryptor(const SOCKET* InSocket = nullptr);

	// Encryptor 소멸자입니다.
	~Encryptor() = default;

	// 입력한 모드를 가진 암호 모듈로 교체합니다.
	void ChangeModule(EMode InMode = EMode::NONE);

	// 암호 모듈에 네트워크 모드를 부여합니다.
	// 암호 모듈의 모든 초기화는 생성자의 호출과 
	// 함께 진행되므로 네트워크 기능도 생성자 매개변수에
	// 포함되어야 하므로, 암호 모듈의 생성 이전에
	// 호출해야 합니다.
	void CoInitNetworkMode(EncryptionBase::NetMode InNetMode);



	// EncryptionBase 클래스의 Encrypt 함수에 대한 래퍼 함수입니다.
	void ToCipher(std::string& OutCipherText, std::string InPlainText);

	// EncryptionBase 클래스의 Decrypt 함수에 대한 래퍼 함수입니다.
	void ToPlain(std::string& OutPlainText, std::string InCipherText);

private:
	// 소켓이 유효한지 확인합니다.
	bool IsSocketValid() const;



private:
	// 네트워크 호스트로부터 전달받은 소켓 인자입니다.
	// 암호자의 생성과 함께 초기화되며, 이후 변경불가입니다.
	const SOCKET* PassSocket;

	// 네트워크 지원 암호화 모듈에 전달할 네트워크 모드입니다.
	// 네트워크를 사용하는 암호 모듈의 생성 이전에 호출되어 
	// 암호 모듈의 생성자의 매개변수에 함께 삽입되어야 합니다.
	EncryptionBase::NetMode PassNetMode;

	// 현재 사용중인 암호화 알고리즘 모드입니다.
	EMode EncryptionMode;

	// 현재 등록된 암,복호화 기능을 수행하는 알고리즘입니다.
	std::unique_ptr<EncryptionBase> EncryptionModule;
};

