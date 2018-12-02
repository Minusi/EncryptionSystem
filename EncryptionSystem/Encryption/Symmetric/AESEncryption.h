#pragma once

#include <memory>
#include <string>

#include <cryptopp700\aes.h>
#include <cryptopp700\osrng.h>
#include <cryptopp700\modes.h>

#include "Encryption\EncryptionBase.h"



class AESEncryption : public EncryptionBase
{
private:
	using Super = EncryptionBase;

public:
	// AESEncryption 생성자입니다.
	AESEncryption(const int InKeySize = CryptoPP::AES::DEFAULT_KEYLENGTH);


private:
	// EncryptionBase을(를) 통해 상속됨
	virtual void InitializeEncryption() override;

public:
	// EncryptionBase을(를) 통해 상속됨
	virtual void Encrypt(std::string & OutCipherText, std::string InPlainText) override;
	virtual void Decrypt(std::string & OutPlainText, std::string InCipherText) override;



private:
	int KeySize;

	std::unique_ptr<CryptoPP::AutoSeededRandomPool> Prng;
	std::unique_ptr<CryptoPP::SecByteBlock> Key;
	std::unique_ptr<CryptoPP::byte[]> IV;
};

extern AESEncryption* GlobalAES;