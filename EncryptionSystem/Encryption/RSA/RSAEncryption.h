#pragma once

#include <memory>
#include <iostream>
#include <sstream>
#include <cassert>
#include <map>

#include <cryptopp700\rsa.h>
#include <cryptopp700\osrng.h>
#include <cryptopp700\hex.h>

#include "Encryption\EncryptionBase.h"
#include "Encryption\CryptoUtil.h"

#include "Debug\Debug.h"
#include "Debug\Macro.h"



/*
 * RSAEncryption 클래스는 RSA 암호 알고리즘을 사용하여
 * 주어진 평문을 암호화하고 비문을 복호화하는 클래스입니다.
 */
class RSAEncryption : public EncryptionBase
{
private:
	constexpr static int KeyBuffer = 92;

	using Super = EncryptionBase;

	class Message
	{
	public:
		Message() = delete;

		/*
		* 키 교환 모드에서 HOST이든 GUEST이든 관계없이
		* 사용되는 공통 메시지 컨테이너입니다.
		*/
		class Common
		{
		public:
			Common() = delete;

			static std::string const ACCEPT;
			static std::string const RECVKEY;
			static std::string const INVALIDKEY;
		};

		/*
		* 키 교환 모드가 HOST인 머신이 사용하는
		* 메시지 컨테이너입니다.
		*/
		class Host
		{
		public:
			Host() = delete;

			static std::string const SENDHOSTKEY;
		};

		/*
		* 키 교환 모드가 GUEST인 머신이 사용하는
		* 메시지 컨테이너입니다.
		*/
		class Guest
		{
		public:
			Guest() = delete;

			static std::string const SENDGUESTKEY;
		};
	};

public:
	//RSAEncryption 생성자입니다.
	RSAEncryption
	(
		INetDelegateComponent* InNetDeleComp = nullptr,
		NetMode InMode = NetMode::NONE, 
		int InKeySize = 128
	);

	//RSAEncryption 소멸자입니다.
	~RSAEncryption() = default;

	// IEncryptable을(를) 통해 상속됨
	//
	virtual void InitializeEncryption() override;


	// IEncryptable을(를) 통해 상속됨
	virtual void Encrypt(std::string & OutCipherText, std::string InPlainText) override;
	virtual void Decrypt(std::string & OutPlainText, std::string InCipherText) override;



private:
	// 공개키 공유를 시작합니다. 이 작업이 수행하고 나면
	// 호스트와 게스트는 각각 상대방의 공개키를 소유하고
	// 있어야 합니다.
	void ExchangeKeyThroughNetwork();

	// 네트워크 호스트 모드로 공개키를 교환합니다. 
	void ExchangeKeyInHostMode();

	// 네트워크 게스트 모드로 공개키를 교환합니다.
	void ExchangeKeyInGuestMode();



private:
	// 공개키 교환에 수행되는 네트워크 모드를 설정합니다.
	NetMode NetworkMode;



	// 키 길이입니다.
	const int KeySize;
	
	// 시드 기반 난수 생성기입니다.
	CryptoPP::AutoSeededRandomPool Prng;

	// RSA 복호화에 사용될 개인키입니다.
	std::unique_ptr<CryptoPP::RSA::PrivateKey> PrivateKey;

	// RSA 암호화에 사용될 공개키 컨테이너입니다.
	// 문자열과 공개키 목록으로 구성됩니다. 여기서는 서버-클라
	// 이언트의 1대1 구조이므로, 자기 자신의 공개키 "SELF"와
	// 상대방의 공개키 "COUNTER"로 구성됩니다.
	std::unique_ptr<std::map<std::string,CryptoPP::RSA::PublicKey>> PublicKeyMap;
};

