#pragma once

#include <cryptopp700\dh.h>
#include <cryptopp700\osrng.h>
#include <cryptopp700\nbtheory.h>

#include "Encryption\IEncryptable.h"



/*
 * DHEncryption 클래스는 Diffie-Hellman 알고리즘으로
 * 주어진 평문을 암호화하고, 비문을 복호화합니다.
 */
class DHEncryption : public IEncryptable
{
private:
	struct DomainParams
	{
	public:
		DomainParams
		(
			CryptoPP::Integer Inp = 0,
			CryptoPP::Integer Inq = 0, 
			CryptoPP::Integer Ing = 0
		);

		CryptoPP::Integer p;
		CryptoPP::Integer q;
		CryptoPP::Integer g;
	};
public:
	//DHEncryption 생성자입니다.
	DHEncryption();

	//DHEncryption 소멸자입니다.
	~DHEncryption();



private:
	// IEncryptable을(를) 통해 상속됨
	virtual void InitializeEncryption() override;

public:
	// IEncryptable을(를) 통해 상속됨
	virtual void Encrypt(std::string & OutCipherText, std::string InPlainText) override;
	virtual void Decrypt(std::string & OutPlainText, std::string InCipherText) override;

private:
	CryptoPP::AutoSeededRandomPool Prng;
	DomainParams DomainParamSet;
	CryptoPP::DH DHMethod;

	CryptoPP::SecByteBlock StrPrivateKey;
	CryptoPP::SecByteBlock StrPublicKey;
};

