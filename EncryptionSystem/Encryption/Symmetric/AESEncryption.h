#pragma once

#include <memory>
#include <string>

#include <cryptopp700\aes.h>
#include <cryptopp700\osrng.h>
#include <cryptopp700\modes.h>

#include "Encryption\EncryptionBase.h"
#include "Encryption\CryptoUtil.h"

#include "Debug\Macro.h"


// AES Encrypt 함수에서 제대로된 동작을 하지 않아 Wiki를 참조하여 그대로
// 복사한 기능을 사용하기 위한 매크로 정의자입니다. 현재는 원인을 발견하
// 여 더이상 사용하지 않습니다. 이유는 StringSource에서의 스마트 포인터에
// 대한 무리한 남용입니다.
#ifndef AES_TEST
//#define AES_TEST
#endif



/*
 * AESEncryption 클래스는 AES 암호화/복호화 알고리즘을 제공하는
 * 클래스입니다. CBC 모드와 연동하여 구현되어 있으며, 따라서 키
 * 와 초기벡터를 이용하여 내부적으로 초기화를 수행합니다.
 */
class AESEncryption : public EncryptionBase
{
private:
	using Super = EncryptionBase;

public:
	// AESEncryption 생성자입니다.
	AESEncryption
	(
		const bool bManualKeyGen = false,
		const std::string InManualKey = std::string(),
		const int InKeySize = CryptoPP::AES::DEFAULT_KEYLENGTH
	);

	// EncryptionBase을(를) 통해 상속됨
	virtual void InitializeEncryption() override;

	// EncryptionBase을(를) 통해 상속됨
	// 평문을 헥사코드된 암호문으로 암호화합니다.
	virtual void Encrypt(std::string & OutCipherText, std::string InPlainText) override;
	
	// EncryptionBase을(를) 통해 상속됨
	// 헥사코드된 암호문을 평문으로 복호화합니다.
	virtual void Decrypt(std::string & OutPlainText, std::string InCipherText) override;



private:
	// 수동으로 키와 IV를 생성합니다.
	void GenerateKeyManually();



private:
	const int KeySize;
	
	// 수동적으로 키를 생성할 것인지를 결정하는 플래그입니다.
	const bool bUseManualKey;

	// 수동적으로 키를 생성할 때 키와 IV에 저장할 값입니다.
	std::string CacheKeyStr;

	std::unique_ptr<CryptoPP::AutoSeededRandomPool> Prng;
	std::unique_ptr<CryptoPP::SecByteBlock> Key;
	std::unique_ptr<CryptoPP::byte[]> IV;


#ifdef AES_TEST
	CryptoPP::AutoSeededRandomPool testprng;
	CryptoPP::SecByteBlock testkey;
	CryptoPP::byte testIV[CryptoPP::AES::BLOCKSIZE];
#endif // AES_TEST

};
