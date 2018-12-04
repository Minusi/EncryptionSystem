#include "DHEncryption.h"

#include "Debug\Debug.h"



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

	PreExchangePrimes();
	InitializeEncryption();
	ExchangeKeyThroughNetwork();

	std::string SharedStr(reinterpret_cast<const char*>(&SharedKey[0]), SharedKey.size());
	SubModule.reset(new AESEncryption(true, SharedStr, SharedStr.size()));

}





void DHEncryption::PreExchangePrimes()
{
	/////////////////////////////////////////////////////////////////////////////
	// 소켓에 대한 유효성 검사를 실시합니다.
	const SOCKET* TempSocket = NetComponent->GetSocket();
	if (TempSocket == nullptr || *TempSocket == SOCKET_ERROR)
	{
		std::cout << "DH::ERROR::Socket Does Not Exist in " << __func__ << std::endl;
		exit(1);
	}

	// 네트워크 컴포넌트에 대한 유효성 검사를 실시합니다.
	if (NetComponent == nullptr || NetComponent->RequireCommunication() == false)
	{
		std::cout << "DH::ERROR::DH Must Need Communication Through Network in " <<
			__func__ << std::endl;
		exit(1);
	}



	// 네트워크 모드에 따른 사전 교환을 수행합니다.
	if (NetworkMode == NetMode::HOST)
	{
		PreExchangePrimesInHostMode();
	}
	else if (NetworkMode == NetMode::GUEST)
	{

		PreExchangePrimesInGuestMode();
	}
	else
	{
		std::cout << "DH::ERROR::NetMode has Invalid Value in " << __func__ << std::endl;
		exit(1);
	}
}

void DHEncryption::PreExchangePrimesInHostMode()
{
	//////////////////////////////////////////////////////////////////////////////
	// 도메인 매개 변수를 생성합니다.
	//CryptoPP::PrimeAndGenerator pg;
	//pg.Generate(1, Prng, KeySize, KeySize - 1);

	//DomainParamSet.p = pg.Prime();
	//DomainParamSet.q = pg.SubPrime();
	//DomainParamSet.g = pg.Generator();

	//DHMethod.AccessGroupParameters().Initialize
	//(
	//	DomainParamSet.p,
	//	DomainParamSet.q,
	//	DomainParamSet.g
	//);
	
	// 대안 도메인 매개 변수 생성입니다.
	DHMethod.AccessGroupParameters().GenerateRandomWithKeySize(Prng, KeySize);

	DomainParamSet.p = DHMethod.GetGroupParameters().GetModulus();
	DomainParamSet.q = (DomainParamSet.p - 1) / 2;
	DomainParamSet.g = DHMethod.GetGroupParameters().GetGenerator();

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
		DomainParamSet.g,
		DomainParamSet.q,
		DomainParamSet.p
	);
	if (v != CryptoPP::Integer::One())
	{
		std::cout << "DH::Error::Failed To Verify Order Of The Subgroup";
		exit(1);
	}

#ifdef CONSOLE_DEBUG
	CON_NEWLINE;
	CON_NEWLINE;
	std::cout << "----------------------------------------------------------------------" << std::endl;
	std::cout << CLASS_TAG << FUNC_TAG << "P && Q && R Values\n" <<
		"\tP : " << DomainParamSet.p << std::endl <<
		"\tG : " << DomainParamSet.g << std::endl <<
		"\tQ : " << DomainParamSet.q << std::endl;
	std::cout << "----------------------------------------------------------------------" << std::endl;
	CON_NEWLINE;
	CON_NEWLINE;
#endif // CONSOLE_DEBUG



	const char* RecvedBuf;

	/////////////////////////////////////////////////////////////////////////////////
	// 게스트에게 소수들을 보내겠다고 전송합니다.
	NetComponent.get()->Send(Message::Host::SENDPRIMES);

	// 게스트가 ACCEPT 메시지를 보내지 않으면 에러를 발생시키고 종료합니다.
	// TODO : 게스트가 향후 어떤 사유에 의해 REJECT 할 수 있을 때 이 곳에
	// 그 기능을 추가해야 합니다.
	RecvedBuf = NetComponent.get()->Recv();
	if (strcmp(RecvedBuf, Message::Common::ACCEPT.c_str()) != 0)
	{
		std::cout << "DH::ERROR::GUEST DH Send Invalid Message in " << __func__ << std::endl;
		exit(1);
	}



	// 전송하기 위해 소수들을 헥사 문자열화한 후, 전송합니다.
	std::string PHexString = CryptoUtil::StringnizeInteger(DomainParamSet.p, true);
	NetComponent->Send(PHexString);

	std::string QHexString = CryptoUtil::StringnizeInteger(DomainParamSet.q, true);
	NetComponent->Send(QHexString);

	std::string GHexString = CryptoUtil::StringnizeInteger(DomainParamSet.g, true);
	NetComponent->Send(GHexString);



	// 게스트가 보내온 메시지를 분석합니다. 게스트가 올바른 소수들을 받았다는
	// 메시지를 제외한 모든 메시지에 대해서 에러를 발생시키고 종료합니다.
	RecvedBuf = NetComponent.get()->Recv();
	if (strcmp(RecvedBuf, Message::Guest::INVALIDPRIMES.c_str()) == 0)
	{
		std::cout << "DH::ERROR::GUEST DH Has Invalid Primes in " << __func__ << std::endl;
		exit(1);
	}
	else if (strcmp(RecvedBuf, Message::Guest::RECVPRIMES.c_str()) == 0)
	{
		return;
	}
	else
	{
		std::cout << "DH::ERROR::GUEST DH Send Invalid Message in " << __func__ << std::endl;
		exit(1);
	}
}

void DHEncryption::PreExchangePrimesInGuestMode()
{
	const char* RecvedBuf;

	/////////////////////////////////////////////////////////////////////////////////
	// 호스트로부터 소수들을 보내겠다는 메시지를 받고, 그 이외의 경우에 대해서
	// 에러를 발생시키고 종료합니다.
	RecvedBuf = NetComponent.get()->Recv();
	if (strcmp(RecvedBuf, Message::Host::SENDPRIMES.c_str()) != 0)
	{
		std::cout << "DH::ERROR::HOST DH Send Invalid Message in " << __func__ << std::endl;
		std::cout << "DH::ERROR::REQUIRED MESSAGE >> " << Message::Host::SENDPRIMES << std::endl;
		exit(1);
	}

	// 호스트에게 알겠다고 전송합니다.
	// TODO : 후에 게스트가 호스트의 요구를 거절할 수 있다면
	// 이 부분을 확장시켜야 합니다.
	NetComponent.get()->Send(Message::Common::ACCEPT);

	// 호스트로부터 P 소수를 전송받습니다.
	RecvedBuf = NetComponent->Recv();
	DomainParamSet.p = CryptoUtil::IntegerizeString(RecvedBuf, true);

	// 호스트로부터 Q 소수를 전송받습니다.
	RecvedBuf = NetComponent->Recv();
	DomainParamSet.q = CryptoUtil::IntegerizeString(RecvedBuf, true);

	// 호스트로부터 G 소수를 전송받습니다.
	RecvedBuf = NetComponent->Recv();
	DomainParamSet.g = CryptoUtil::IntegerizeString(RecvedBuf, true);

	// 받은 도메인 변수들로 DH를 초기화합니다.
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
		NetComponent->Send(Message::Guest::INVALIDPRIMES);
		std::cout << "DH::Error::Failed To Valiate Prime And Generator" << std::endl;
		exit(1);
	}

	DomainParamSet.p = DHMethod.GetGroupParameters().GetModulus();
	DomainParamSet.q = DHMethod.GetGroupParameters().GetSubgroupOrder();
	DomainParamSet.g = DHMethod.GetGroupParameters().GetGenerator();

	CryptoPP::Integer v = CryptoPP::ModularExponentiation
	(
		DomainParamSet.g,
		DomainParamSet.q,
		DomainParamSet.p
	);

	if (v != CryptoPP::Integer::One())
	{
		NetComponent->Send(Message::Guest::INVALIDPRIMES);
		std::cout << "DH::Error::Failed To Verify Order Of The Subgroup";
		exit(1);
	}


	// 소수들이 유효성 검사를 통과하였다면, 호스트에게 알려
	// 다음 과정을 진행할 수 있도록 합니다.
	NetComponent->Send(Message::Guest::RECVPRIMES);


#ifdef CONSOLE_DEBUG
	CON_NEWLINE;
	CON_NEWLINE;
	std::cout << "----------------------------------------------------------------------" << std::endl;
	std::cout << CLASS_TAG << FUNC_TAG << "P && Q && R Values\n" <<
		"\tP : " << DomainParamSet.p << std::endl <<
		"\tG : " << DomainParamSet.g << std::endl <<
		"\tQ : " << DomainParamSet.q << std::endl;
	std::cout << "----------------------------------------------------------------------" << std::endl;
	CON_NEWLINE;
	CON_NEWLINE;
#endif // CONSOLE_DEBUG
}





void DHEncryption::InitializeEncryption()
{
	//////////////////////////////////////////////////////////////////////////////
	// 비대칭 키를 생성합니다.
	PrivateKey = CryptoPP::SecByteBlock(DHMethod.PrivateKeyLength());
	PublicKey = CryptoPP::SecByteBlock(DHMethod.PublicKeyLength());

#ifdef CONSOLE_DEBUG
	std::cout << "----------------------------------------------------------------------" << std::endl;
	std::cout << "Before GenerateKeyPair" << std::endl;
	std::cout << "PrivateKey points : " << (unsigned int)PrivateKey.end() << std::endl;
	std::cout << "PublicKey points : " << (unsigned int)PublicKey.end() << std::endl;
	std::cout << "----------------------------------------------------------------------" << std::endl;
#endif // CONSOLE_DEBUG

	DHMethod.GenerateKeyPair(Prng, PrivateKey, PublicKey);
	Sleep(1000);

#ifdef CONSOLE_DEBUG
	std::cout << "----------------------------------------------------------------------" << std::endl;
	std::cout << "After GenerateKeyPair" << std::endl;
	std::cout << "PrivateKey points : " << (unsigned int)PrivateKey.end() << std::endl;
	std::cout << "PublicKey points : " << (unsigned int)PublicKey.end() << std::endl;
	std::cout << "----------------------------------------------------------------------" << std::endl;
#endif // CONSOLE_DEBUG


#ifdef CONSOLE_DEBUG
	CON_NEWLINE;
	CON_NEWLINE;
	std::cout << "----------------------------------------------------------------------" << std::endl;
	std::cout << CLASS_TAG << FUNC_TAG << "Private & Public Key" << std::endl << 
		"PrivateKey (" << DHMethod.PrivateKeyLength() << ") : " << std::hex << CryptoPP::Integer(PrivateKey, PrivateKey.size()) << std::endl <<
		"PublicKey  (" << DHMethod.PublicKeyLength()  << ") : " << std::hex << CryptoPP::Integer(PublicKey, PublicKey.size()) << std::endl;
	std::cout << "----------------------------------------------------------------------" << std::endl;
	CON_NEWLINE;
	CON_NEWLINE;
#endif // CONSOLE_DEBUG

#ifdef CONSOLE_DEBUG
	std::cout << "----------------------------------------------------------------------" << std::endl;
	std::cout << "After GenerateKeyPair" << std::endl;
	std::cout << "PrivateKey points : " << (unsigned int)PrivateKey.end() << std::endl;
	std::cout << "PublicKey points : " << (unsigned int)PublicKey.end() << std::endl;
	std::cout << "----------------------------------------------------------------------" << std::endl;
#endif // CONSOLE_DEBUG

}





void DHEncryption::ExchangeKeyThroughNetwork()
{
#ifdef CONSOLE_DEBUG
	CON_NEWLINE;
	CON_NEWLINE;
	std::cout << "----------------------------------------------------------------------" << std::endl;
	std::cout << CLASS_TAG << FUNC_TAG << "Private & Public Key" << std::endl <<
		"PrivateKey (" << DHMethod.PrivateKeyLength() << ") : " << std::hex << CryptoPP::Integer(PrivateKey, PrivateKey.size()) << std::endl <<
		"PublicKey  (" << DHMethod.PublicKeyLength() << ") : " << std::hex << CryptoPP::Integer(PublicKey, PublicKey.size()) << std::endl;
	std::cout << "----------------------------------------------------------------------" << std::endl;
	CON_NEWLINE;
	CON_NEWLINE;
#endif // CONSOLE_DEBUG

	// 소켓에 대한 유효성 검사를 실시합니다.
	const SOCKET* TempSocket = NetComponent->GetSocket();
	if (TempSocket == nullptr || *TempSocket == SOCKET_ERROR)
	{
		std::cout << "DH::ERROR::Socket Does Not Exist in " << __func__ << std::endl;
		exit(1);
	}

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
	else if (NetworkMode == NetMode::GUEST)
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
	const char* RecvedBuf;

	/////////////////////////////////////////////////////////////////////////////////
	// 게스트에게 자신의 공개키를 보내겠다고 전송합니다.
	NetComponent->Send(Message::Host::SENDHOSTKEY);

	// 게스트가 알았다는 신호를 보내지 않으면 에러를 발생시키고 종료합니다.
	RecvedBuf = NetComponent->Recv();
	if (strcmp(RecvedBuf, Message::Common::ACCEPT.c_str()) != 0)
	{
		std::cout << "DH::ERROR::GUEST DH Send Invalid Value in " << __func__ << std::endl;
		exit(1);
	}



	/////////////////////////////////////////////////////////////////////////////////
	// 게스트에게 공개키를 전송하기 위해 SecByteBlock에서 String으로 변환합니다.
	std::string PublicStr = CryptoUtil::StringnizeBlock(PublicKey);
	std::string HexStr = CryptoUtil::HexaizeString(PublicStr);
	std::transform(HexStr.begin(), HexStr.end(), HexStr.begin(), ::tolower);

	// 게스트에게 자신의 공개키를 전송합니다.
	NetComponent.get()->Send(HexStr);



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
	RecvedBuf = NetComponent->Recv();
	std::string RecoveredFromHex = CryptoUtil::StringnizeHex(RecvedBuf);
	CryptoPP::SecByteBlock GuestPublicKey = CryptoUtil::BlockizeString(RecoveredFromHex);


	// 게스트의 공개키로 비밀 공유 키를 생성합니다.
	CreateSharedPrivateKey(GuestPublicKey);

	// 게스트에게 키를 수신했다고 전송합니다.

	NetComponent.get()->Send(Message::Common::RECVKEY);

#ifdef CONSOLE_DEBUG
	std::cout << "END OF " << __func__ << std::endl;
#endif // CONSOLE_DEBUG
}






void DHEncryption::ExchangeKeyInGuestMode()
{
	const char* RecvedBuf;

	/////////////////////////////////////////////////////////////////////////////////
	// 호스트가 자신의 공개키를 보내겠다는 메시지를 송신하지 않았으면
	// 에러를 발생시키고 종료합니다.
	RecvedBuf = NetComponent->Recv();
	if (strcmp(RecvedBuf, Message::Host::SENDHOSTKEY.c_str()) != 0)
	{
		std::cout << "DH::ERROR::Host DH Send Invalid Value in " << __func__ << std::endl;
		exit(1);
	}

	// 호스트에게 알겠다고 전송합니다.
	NetComponent->Send(Message::Common::ACCEPT);





	/////////////////////////////////////////////////////////////////////////////////
	// 호스트의 공개키를 수신합니다.
	// 이후 비밀 공유 키 생성을 위해 변환 과정을 거칩니다.
	RecvedBuf = NetComponent->Recv();
	std::string RecoveredFromHex = CryptoUtil::StringnizeHex(RecvedBuf);
	CryptoPP::SecByteBlock HostPublicKey = CryptoUtil::BlockizeString(RecoveredFromHex);



	// 호스트의 공개키로 비밀 공유 키를 생성합니다.
	CreateSharedPrivateKey(HostPublicKey);

	// 호스트에게 키를 수신했다고 전송합니다.
	NetComponent.get()->Send(Message::Common::RECVKEY);



	/////////////////////////////////////////////////////////////////////////////////
	// 호스트에게 자신의 공개키를 보내겠다고 전송합니다.
	NetComponent.get()->Send(Message::Guest::SENDGUESTKEY);

	// 게스트가 알았다는 신호를 보내지 않으면 에러를 발생시키고 종료합니다.
	RecvedBuf = NetComponent->Recv();
	if (strcmp(RecvedBuf, Message::Common::ACCEPT.c_str()) != 0)
	{
		std::cout << "DH::ERROR::GUEST DH Send Invalid Message in " << __func__ << std::endl;
		exit(1);
	}



	/////////////////////////////////////////////////////////////////////////////////
	// 호스트에게 공개키를 전송하기 위해 SecByteBlock에서 String으로 변환합니다.
	std::string PublicStr = CryptoUtil::StringnizeBlock(PublicKey);
	std::string HexStr = CryptoUtil::HexaizeString(PublicStr);
	std::transform(HexStr.begin(), HexStr.end(), HexStr.begin(), ::tolower);


	// 호스트에게 자신의 공개키를 전송합니다.
	NetComponent.get()->Send(HexStr);



	// 호스트가 키를 받았다고 얘기하지 않으면 에러를 발생시키고 종료합니다.
	RecvedBuf = NetComponent->Recv();
	if (strcmp(RecvedBuf, Message::Common::RECVKEY.c_str()) != 0)
	{
		std::cout << "DH::ERROR::GUEST DH Has Invalid Our Public Key in " << __func__ << std::endl;
		exit(1);
	}

#ifdef CONSOLE_DEBUG
	std::cout << "END OF " << __func__ << std::endl;
#endif // CONSOLE_DEBUG
}





void DHEncryption::CreateSharedPrivateKey(CryptoPP::SecByteBlock CounterPublicKey)
{
	try
	{
		//// 공개 키가 유효하지 않으면 에러와 함꼐 종료합니다.
		//if (CounterPublicKey == nullptr)
		//{
		//	std::cout << "DH::ERROR::Counter Public Key Is Not Valid in" << __func__ << std::endl;
		//	exit(1);
		//}

		// 상대 공개 키가 비어있으면 에러와 함꼐 종료합니다.
		if (CounterPublicKey.empty())
		{
			std::cout << "DH::ERROR::Counter Public Key Is Empty in" << __func__ << std::endl;
			exit(1);
		}

		// 공유 비밀 키를 생성합니다.
		SharedKey = CryptoPP::SecByteBlock(DHMethod.AgreedValueLength());


#ifdef CONSOLE_DEBUG
		CON_NEWLINE;
		CON_NEWLINE;
		std::cout << "----------------------------------------------------------------------" << std::endl;
		std::cout << CLASS_TAG << FUNC_TAG << "DHMethod.AgreedValueLength() : " << DHMethod.AgreedValueLength() << std::endl;
		std::cout << "----------------------------------------------------------------------" << std::endl;
		std::cout << "SharedKey        (" << SharedKey.SizeInBytes()        << ") : " << CryptoUtil::HexaizeString(CryptoUtil::StringnizeBlock(SharedKey)) << std::endl;
		std::cout << "PrivateKey       (" << DHMethod.PrivateKeyLength()    << ") : " << CryptoUtil::HexaizeString(CryptoUtil::StringnizeBlock(PrivateKey)) << std::endl;
		std::cout << "PublicKey        (" << DHMethod.PublicKeyLength()     << ") : " << CryptoUtil::HexaizeString(CryptoUtil::StringnizeBlock(PublicKey)) << std::endl;
		std::cout << "CounterPublicKey (" << CounterPublicKey.SizeInBytes() << ") : " << CryptoUtil::HexaizeString(CryptoUtil::StringnizeBlock(CounterPublicKey)) << std::endl;
		std::cout << "----------------------------------------------------------------------" << std::endl;
		CON_NEWLINE;
		CON_NEWLINE;
#endif // CONSOLE_DEBUG


		

		// 공유 비밀 키를 검증합니다.
		if (DHMethod.Agree(SharedKey, PrivateKey, CounterPublicKey) == false)
		{
			std::cout << "DH::ERROR::Failed To Reach Shared Secret in " << __func__ << std::endl;
			exit(1);
		}

#ifdef CONSOLE_DEBUG
		CryptoPP::Integer DecodedInt;
		DecodedInt.Decode(SharedKey.BytePtr(), SharedKey.SizeInBytes());

		CON_NEWLINE;
		CON_NEWLINE;
		std::cout << "----------------------------------------------------------------------" << std::endl;
		std::cout << CLASS_TAG << CON_DEBUG_TAG << std::endl <<
			"SharedKey : " << std::hex << DecodedInt << std::endl;
		std::cout << "----------------------------------------------------------------------" << std::endl;
		CON_NEWLINE;
		CON_NEWLINE;
#endif // CONSOLE_DEBUG
	}
	catch (const CryptoPP::Exception& e)
	{
		std::cerr << e.what() << std::endl;
		exit(1);
	}
}





void DHEncryption::Encrypt(std::string & OutCipherText, std::string InPlainText)
{
	MAC_PRNT_MSG_WITH_PAD("PROCESSING", 1);
	if (SubModule == nullptr)
	{
		std::cout << "DH::ERROR::SubModule Does Not Initiailized in	" << __func__ << std::endl;
		exit(1);
	}
	SubModule->Encrypt(OutCipherText, InPlainText);
	
	MAC_PRNT_MSG_WITH_PAD("PROCESS END", 1);

}

void DHEncryption::Decrypt(std::string & OutPlainText, std::string InCipherText)
{
	MAC_PRNT_MSG_WITH_PAD("PROCESSING", 1);
	if (SubModule == nullptr)
	{
		std::cout << "DH::ERROR::SubModule Does Not Initiailized in " << __func__ << std::endl;
		exit(1);
	}
	SubModule->Decrypt(OutPlainText, InCipherText);

	MAC_PRNT_MSG_WITH_PAD("PROCESS END", 1);

}