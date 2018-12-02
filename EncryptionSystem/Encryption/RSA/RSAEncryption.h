#pragma once

#include <memory>

#include <cryptopp700\rsa.h>
#include <cryptopp700\osrng.h>

#include "Encryption\EncryptionBase.h"



/*
 * RSAEncryption 클래스는 RSA 암호 알고리즘을 사용하여
 * 주어진 평문을 암호화하고 비문을 복호화하는 클래스입니다.
 */
class RSAEncryption : public EncryptionBase
{
public:
	//RSAEncryption 생성자입니다.
	RSAEncryption();

	//RSAEncryption 소멸자입니다.
	~RSAEncryption();

	// IEncryptable을(를) 통해 상속됨
	virtual void Encrypt(std::string & OutCipherText, std::string InPlainText) override;
	virtual void Decrypt(std::string & OutPlainText, std::string InCipherText) override;

private:
	// IEncryptable을(를) 통해 상속됨
	virtual void InitializeEncryption() override;

private:
	CryptoPP::AutoSeededRandomPool Rng;

	std::unique_ptr<CryptoPP::RSA::PrivateKey> PrivateKeyPtr;
	std::unique_ptr<CryptoPP::RSA::PublicKey> PublicKeyPtr;
};

