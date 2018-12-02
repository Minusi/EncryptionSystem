#include "DHEncryption.h"

#include <Debug\Debug.h>



DHEncryption::DHEncryption
(
	INetDelegateComponent* InNetDeleComp,
	NetMode InMode,
	int InKeySize
)	
	: Super(InNetDeleComp)
	, NetworkMode(InMode)
	, KeySize(InKeySize)
{
	InitializeEncryption();
	ExchangeKeyThroughNetwork();
}

DHEncryption::~DHEncryption()
{
}

void DHEncryption::ExchangeKeyThroughNetwork()
{
	// 네트워크 컴포넌트에 대한 유효성 검사를 실시합니다.
	if (NetComponent == nullptr || NetComponent->RequireCommunication() == false)
	{
		std::cout << "DH::ERROR::DH Must Need Communication Through Network in " << 
			__func__ << std::endl;
		exit(1);
	}

	// 네트워크 모드에 따른 키 교환을 수행합니다.
	if ( NetworkMode == NetMode::NONE)
	{
		std::cout << "DH::ERROR::NetMode has NetMode::NONE Value in" << __func__ << std::endl;
		exit(1);
	}
	else if (NetworkMode == NetMode::HOST)
	{
		ExchangeKeyInHostMode();
	}
	else if (NetworkMode == NetMode::GUSET)
	{
		ExchangeKeyInGuestMode();
	}
	else
	{
		std::cout << "DH::ERROR::NetMode has Invalid Value in " << __func__ << std::endl;
		exit(1);
	}
}

void DHEncryption::ExchangeKeyInHostMode()
{
	// 소켓에 대한 유효성 검사를 실시합니다.
	const SOCKET* TempSocket = NetComponent->GetSocket();
	if ( TempSocket == nullptr || *TempSocket == SOCKET_ERROR )
	{
		std::cout << "DH::ERROR::Socket Does Not Exist in " << __func__ << std::endl;
		exit(1);
	}

	const char* RecvedBuf;


	
	/////////////////////////////////////////////////////////////////////////////////

	// 게스트에게 자신의 공개키를 보내겠다고 전송합니다.
	NetComponent.get()->Send(Message::Host::SENDHOSTKEY);

	// 게스트가 알았다는 신호를 보내지 않으면 에러를 발생시키고 종료합니다.
	RecvedBuf = NetComponent.get()->Recv();
	if (strcmp(RecvedBuf, Message::Common::ACCEPT.c_str()) != 0)
	{
		std::cout << "DH::ERROR::GUEST DH Send Invalid Value in " << __func__ << std::endl;
		exit(1);
	}



	/////////////////////////////////////////////////////////////////////////////////

	// 게스트에게 공개키를 전송하기 위해 SecByteBlock에서 String으로 변환합니다.
	std::string SecToStr(reinterpret_cast<const char*>(&PublicKey[0]), PublicKey.size());

	// 게스트에게 자신의 공개키를 전송합니다.
	NetComponent.get()->Send(SecToStr);

	// 게스트가 키를 받았다고 얘기하지 않으면 에러를 발생시키고 종료합니다.
	RecvedBuf = NetComponent.get()->Recv();
	if (strcmp(RecvedBuf, Message::Common::RECVKEY.c_str()) != 0)
	{
		std::cout << "DH::ERROR::GUEST DH Has Invalid Our Public Key in " << __func__ << std::endl;
		exit(1);
	}



	/////////////////////////////////////////////////////////////////////////////////

	// 게스트가 자신의 공개키를 보내겠다는 메시지를 송신하지 않았으면
	// 에러를 발생시키고 종료합니다.
	RecvedBuf = NetComponent.get()->Recv();
	if (strcmp(RecvedBuf, Message::Guest::SENDGUESTKEY.c_str()) != 0)
	{
		std::cout << "DH::ERROR::Guest DH Send Invalid Value in " << __func__ << std::endl;
		exit(1);
	}

	// 게스트에게 알겠다고 전송합니다.
	NetComponent.get()->Send(Message::Common::ACCEPT);



	/////////////////////////////////////////////////////////////////////////////////

	// 게스트의 공개키를 수신합니다.
	// 이후 비밀 공유 키 생성을 위해 변환 과정을 거칩니다.
	RecvedBuf = NetComponent.get()->Recv();
	std::string RecvedStr = RecvedBuf;
	CryptoPP::SecByteBlock GuestPublicKey
	(
		reinterpret_cast<const byte*>(&RecvedStr[0]), RecvedStr.size()
	);

	// 게스트의 공개키로 비밀 공유 키를 생성합니다.
	CreateSharedPrivateKey(&GuestPublicKey);

	// 게스트에게 키를 수신했다고 전송합니다.
	NetComponent.get()->Send(Message::Common::RECVKEY);
}

void DHEncryption::ExchangeKeyInGuestMode()
{
	// 소켓에 대한 유효성 검사를 실시합니다.
	const SOCKET* TempSocket = NetComponent->GetSocket();
	if (TempSocket == nullptr || *TempSocket == SOCKET_ERROR)
	{
		std::cout << "DH::ERROR::Socket Does Not Exist in " << __func__ << std::endl;
		exit(1);
	}

	const char* RecvedBuf;



	/////////////////////////////////////////////////////////////////////////////////

	// 호스트가 자신의 공개키를 보내겠다는 메시지를 송신하지 않았으면
	// 에러를 발생시키고 종료합니다.
	RecvedBuf = NetComponent.get()->Recv();
	if (strcmp(RecvedBuf, Message::Host::SENDHOSTKEY.c_str()) != 0)
	{
		std::cout << "DH::ERROR::Host DH Send Invalid Value in " << __func__ << std::endl;
		exit(1);
	}

	// 호스트에게 알겠다고 전송합니다.
	NetComponent.get()->Send(Message::Common::ACCEPT);



	/////////////////////////////////////////////////////////////////////////////////

	// 호스트의 공개키를 수신합니다.
	// 이후 비밀 공유 키 생성을 위해 변환 과정을 거칩니다.
	RecvedBuf = NetComponent.get()->Recv();
	std::string RecvedStr = RecvedBuf;
	CryptoPP::SecByteBlock HostPublicKey
	(
		reinterpret_cast<const byte*>(&RecvedStr[0]), RecvedStr.size()
	);

	// 호스트의 공개키로 비밀 공유 키를 생성합니다.
	CreateSharedPrivateKey(&HostPublicKey);

	// 호스트에게 키를 수신했다고 전송합니다.
	NetComponent.get()->Send(Message::Common::RECVKEY);



	/////////////////////////////////////////////////////////////////////////////////

	// 게스트에게 자신의 공개키를 보내겠다고 전송합니다.
	NetComponent.get()->Send(Message::Host::SENDHOSTKEY);

	// 게스트가 알았다는 신호를 보내지 않으면 에러를 발생시키고 종료합니다.
	RecvedBuf = NetComponent.get()->Recv();
	if (strcmp(RecvedBuf, Message::Common::ACCEPT.c_str()) != 0)
	{
		std::cout << "DH::ERROR::GUEST DH Send Invalid Value in " << __func__ << std::endl;
		exit(1);
	}



	/////////////////////////////////////////////////////////////////////////////////

	// 호스트에게 공개키를 전송하기 위해 SecByteBlock에서 String으로 변환합니다.
	std::string SecToStr(reinterpret_cast<const char*>(&PublicKey[0]), PublicKey.size());

	// 호스트에게 자신의 공개키를 전송합니다.
	NetComponent.get()->Send(SecToStr.c_str());

	// 호스트가 키를 받았다고 얘기하지 않으면 에러를 발생시키고 종료합니다.
	RecvedBuf = NetComponent.get()->Recv();
	if (strcmp(RecvedBuf, Message::Common::RECVKEY.c_str()) != 0)
	{
		std::cout << "DH::ERROR::GUEST DH Has Invalid Our Public Key in " << __func__ << std::endl;
		exit(1);
	}
}

void DHEncryption::CreateSharedPrivateKey(const CryptoPP::SecByteBlock* CounterPublicKey)
{
	// 공개 키가 유효하지 않으면 에러와 함꼐 종료합니다.
	if (CounterPublicKey == nullptr)
	{
		std::cout << "DH::ERROR::Counter Public Key Is Not Valid in" << __func__ << std::endl;
		exit(1);
	}

	// 상대 공개 키가 비어있으면 에러와 함꼐 종료합니다.
	if (CounterPublicKey->empty())
	{
		std::cout << "DH::ERROR::Counter Public Key Is Empty in" << __func__ << std::endl;
		exit(1);
	}

	// 공유 비밀 키를 생성합니다.
	CryptoPP::SecByteBlock TempSharedKey(DHMethod.AgreedValueLength());
	SharedKey.Assign(TempSharedKey);

	// 공유 비밀 키를 검증합니다.
	if (DHMethod.Agree(SharedKey, PrivateKey, *CounterPublicKey) == false)
	{
		std::cout << "DH::ERROR::Failed To Reach Shared Secret in " << __func__ << std::endl;
		exit(1);
	}

#ifdef CONSOLE_DEBUG
	CryptoPP::Integer DecodedInt;
	DecodedInt.Decode(SharedKey.BytePtr(), SharedKey.SizeInBytes());

	CON_NEWLINE;
	std::cout << CLASS_TAG << CON_DEBUG_TAG <<
		"SharedKey : \t" << std::hex << DecodedInt << std::endl;
	CON_NEWLINE;
#endif // CONSOLE_DEBUG

}

void DHEncryption::InitializeEncryption()
{
	//////////////////////////////////////////////////////////////////////////////
	// 도메인 매개 변수를 생성합니다.
	CryptoPP::PrimeAndGenerator pg;
	pg.Generate(1, Prng, KeySize, KeySize-1);

	DomainParamSet.p = pg.Prime();
	DomainParamSet.q = pg.SubPrime();
	DomainParamSet.g = pg.Generator();

	DHMethod.AccessGroupParameters().Initialize
	(
		DomainParamSet.p, 
		DomainParamSet.q, 
		DomainParamSet.g
	);

	/////////////////////////////////////////////////////////////////////////////
	// 도메인 매개 변수 p와 g에 대한 유효성 검사를 실시합니다.
	if (DHMethod.GetGroupParameters().ValidateGroup(Prng, 3) == false)
	{
		std::cout << "DH::Error::Failed To Valiate Prime And Generator" << std::endl;
		exit(1);
	}

	CryptoPP::Integer v = CryptoPP::ModularExponentiation
	(
		DomainParamSet.p, 
		DomainParamSet.q, 
		DomainParamSet.p
	);
	if (v != CryptoPP::Integer::One())
	{
		std::cout << "DH::Error::Failed To Verify Order Of The Subgroup";
		exit(1);
	}

	//////////////////////////////////////////////////////////////////////////////
	// 비대칭 키를 생성합니다.
	PrivateKey = CryptoPP::SecByteBlock(DHMethod.PrivateKeyLength());
	PublicKey = CryptoPP::SecByteBlock(DHMethod.PublicKeyLength());
	DHMethod.GenerateKeyPair(Prng, PrivateKey, PublicKey);

	ExchangeKeyThroughNetwork();
}

void DHEncryption::Encrypt(std::string & OutCipherText, std::string InPlainText)
{
}

void DHEncryption::Decrypt(std::string & OutPlainText, std::string InCipherText)
{
}

DHEncryption::DomainParams::DomainParams
(
	CryptoPP::Integer Inp, 
	CryptoPP::Integer Inq, 
	CryptoPP::Integer Ing
)
	: p(Inp)
	, q(Inq)
	, g(Ing)
{
}
