#pragma once

#include <memory>

#include <cryptopp700\dh.h>
#include <cryptopp700\osrng.h>
#include <cryptopp700\nbtheory.h>

#include "Encryption\EncryptionBase.h"



/*
 * DHEncryption 클래스는 Diffie-Hellman 알고리즘으로
 * 주어진 평문을 암호화하고, 비문을 복호화합니다.
 */
class DHEncryption sealed : public EncryptionBase
{
private:
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

	// 라이브러리의 Prime, Generator, SubGroup 파라미터들을
	// 한데 집결시키기 위한 단순 구조체입니다.
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
	// DHEncryption 생성자입니다. EncryptionBase에서 네트워크 관련
	// 초기화를 진행해주므로 이 곳에서는 네트워크 관련 매개변수만
	// 입력해주면 알아서 상위 클래스가 초기화 해줍니다.( 상위 클래스 호출을 통해 )
	// 이 생성자에서는 상위 클래스가 제공해주지 않는 암호화 알고리즘의
	// 초기화를 수행하여야 합니다.
	DHEncryption
	(
		INetDelegateComponent* InNetDeleComp = nullptr,
		NetMode InMode = NetMode::NONE,
		int InKeySize = BUFFERSIZE
	);

	//DHEncryption 소멸자입니다.
	~DHEncryption();

	// IEncryptable을(를) 통해 상속됨
	virtual void Encrypt(std::string & OutCipherText, std::string InPlainText) override;
	virtual void Decrypt(std::string & OutPlainText, std::string InCipherText) override;



private:
	// 키 공유 프로세스를 시작합니다. 이 프로세스가 끝나
	// 면 호스트와 게스트는 서로의 공개키를 이용하여 공유
	// 된 비밀키 생성이 완료된 상태여야합니다.
	// 
	// 내부적으로 ExchangeKeyInXXXMode 
	// -> CreateSharedPrivateKey 를 호출합니다.
	void ExchangeKeyThroughNetwork();

	// 네트워크 호스트 모드로 키를 교환합니다. 
	void ExchangeKeyInHostMode();

	// 네트워크 게스트 모드로 키를 교환합니다.
	void ExchangeKeyInGuestMode();

	// 공유 비밀 키를 생성합니다.
	void CreateSharedPrivateKey(const CryptoPP::SecByteBlock* CounterPublicKey);

	// IEncryptable을(를) 통해 상속됨
	// DH 키 교환에 필요한 사전 초기화를 수행합니다.
	virtual void InitializeEncryption() override;

private:
	// 암호 교환에 수행되는 네트워크 모드를 설정합니다.
	NetMode NetworkMode;



	CryptoPP::AutoSeededRandomPool Prng;
	DomainParams DomainParamSet;
	CryptoPP::DH DHMethod;

	const int KeySize;
	CryptoPP::SecByteBlock PrivateKey;
	CryptoPP::SecByteBlock PublicKey;
	CryptoPP::SecByteBlock SharedKey;
};