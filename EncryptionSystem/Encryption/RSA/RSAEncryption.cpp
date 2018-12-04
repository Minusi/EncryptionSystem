#include "RSAEncryption.h"



RSAEncryption::RSAEncryption
(
	INetDelegateComponent* InNetDeleComp,
	NetMode InMode,
	int InKeySize
)	
	: Super(InNetDeleComp)
	, NetworkMode(InMode)
	, KeySize(InKeySize)
{
	MAC_PRNT_MSG_WITH_PAD("", 1);

	InitializeEncryption();
	ExchangeKeyThroughNetwork();
}





void RSAEncryption::InitializeEncryption()
{
	MAC_PRNT_MSG_WITH_PAD
	(
		MAC_CLASS << MAC_FUNC << MAC_LINENUM,
		2
	);


	////////////////////////////////////////////////////////////////////////////////
	// 키를 생성합니다.
	try
	{
		// 공개키 맵 컨테이너를 초기화합니다.
		PublicKeyMap.reset(new std::map<std::string, CryptoPP::RSA::PublicKey>());
	

		CryptoPP::InvertibleRSAFunction Params;
		Params.GenerateRandomWithKeySize(Prng, KeySize);

		// 자신의 공개키를 생성하여 공개키 맵 컨테이너에 삽입합니다.
		PrivateKey.reset(new CryptoPP::RSA::PrivateKey(Params));
		PublicKeyMap->insert
		(
			std::make_pair<std::string, CryptoPP::RSA::PublicKey>
			(
				std::string("SELF"), 
				CryptoPP::RSA::PublicKey(Params)
			)
		);


		// 디버그를 찍기 위한 작업입니다.
		CryptoPP::Integer e = PublicKeyMap->at("SELF").GetPublicExponent();
		CryptoPP::Integer n = PublicKeyMap->at("SELF").GetModulus();
		CryptoPP::Integer d = PrivateKey->GetPrivateExponent();
		MAC_PRNT_MSG_WITH_PAD
		(
			"SelfPubKey.e : " << e << std::endl <<
			"SelfPubKey.n : " << n << std::endl <<
			"SelfPrivKey.d: " << d,
			1
			
		);
	}
	catch (CryptoPP::Exception& e)
	{
		MAC_ERR_MSG_WITH_PAD(e.what(), 1);
		exit(1);
	}
}





void RSAEncryption::ExchangeKeyThroughNetwork()
{	
	MAC_PRNT_MSG_WITH_PAD
	(
		"",
		2
	);

	// 소켓에 대한 유효성 검사를 실시합니다.
	const SOCKET* TempSocket = NetComponent->GetSocket();
	if (TempSocket == nullptr || *TempSocket == SOCKET_ERROR)
	{
		MAC_ERR_MSG("Socket Does Not Exist in ");
		exit(1);
	}

	// 네트워크 컴포넌트에 대한 유효성 검사를 실시합니다.
	if (NetComponent == nullptr || NetComponent->RequireCommunication() == false)
	{
		MAC_ERR_MSG_WITH_PAD("DH Must Need Communication Through Network in ", 1);
		exit(1);
	}



	// 네트워크 모드에 따른 키 교환을 수행합니다.
	if (NetworkMode == NetMode::NONE)
	{
		MAC_ERR_MSG_WITH_PAD("NetMode has NetMode::NONE Value in ", 1);
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
		MAC_ERR_MSG_WITH_PAD("NetMode has Invalid Value in ", 1);
		exit(1);
	}
}




void RSAEncryption::ExchangeKeyInHostMode()
{
	MAC_PRNT_MSG_WITH_PAD
	(
		"",
		2
	);

	const char* RecvedBuf;

	// 호스트에서 게스트로 공개키를 넘기는 과정
	{
		MAC_PRNT_MSG_WITH_PAD("Host->Guest", 1);

		/******************************************************************************/
		// 게스트에게 자신의 공개키를 보내겠다고 전송합니다.
		NetComponent->Send(Message::Host::SENDHOSTKEY);

		// 게스트가 ACCEPT 메시지를 보내지 않으면 에러를 발생시키고 종료합니다.
		RecvedBuf = NetComponent->Recv();
		if (strcmp(RecvedBuf, Message::Common::ACCEPT.c_str()) != 0)
		{
			MAC_ERR_MSG_WITH_PAD("GUEST RSA Send Invalid Message", 1);
			exit(1);
		}





		/******************************************************************************/
		// 게스트에게 자신의 공개키를 가공하여 전송합니다.
		// 먼저, 자신의 공개키를 가져옵니다.
		auto Result = PublicKeyMap->find("SELF");
		if (Result == PublicKeyMap->end())
		{
			MAC_ERR_MSG_WITH_PAD("Self Public Key Does Not Established", 1);
			exit(1);
		}
		CryptoPP::RSA::PublicKey SelfPubKey = Result->second;

		// 이후, 공개키를 전송하기 위해 문자열로 변환하는 과정을 거칩니다.
		std::string HexStr;
		CryptoPP::ByteQueue Queue;
		SelfPubKey.Save(Queue);
		
		
		// 키 정보를 담을 버퍼를 선언하고 저장합니다.
		CryptoPP::byte PublicKeyBuffer[KeyBuffer];

		std::cout << "HIHI\t" << sizeof(PublicKeyBuffer) << std::endl;
		size_t Size = Queue.Get((CryptoPP::byte*)&PublicKeyBuffer, sizeof(PublicKeyBuffer));


		CryptoPP::SecByteBlock PublicKeyBlock{ PublicKeyBuffer, sizeof(PublicKeyBuffer) };
		HexStr = CryptoUtil::HexaizeString(CryptoUtil::StringnizeBlock(PublicKeyBlock));

		// 문자열에 쓰레기값을 들어가지 않게 하기 위해 resize합니다.
		HexStr.resize(HexStr.length());

		// 게스트에게 우리의 공개키를 전송합니다.
		NetComponent->Send(HexStr);

		std::cout << "HIHI\t" << HexStr.size() << std::endl;

		// 게스트에게 키 사이즈를 전송합니다.
		std::string SizeStr(std::to_string(Size));
		NetComponent->Send(SizeStr);



		// 게스트가 키를 제대로 받지 않았거나 유효하지 않은 메시지를 수신하면
		// 에러를 발생시키고 종료합니다.
		RecvedBuf = NetComponent->Recv();
		if (strcmp(RecvedBuf, Message::Common::RECVKEY.c_str()) == 0)
		{
		}
		else if (strcmp(RecvedBuf, Message::Common::INVALIDKEY.c_str()) == 0)
		{
			MAC_ERR_MSG_WITH_PAD("GUEST RSA Received Invalid Our Public Key ", 1);
			exit(1);
		}
		else
		{
			MAC_ERR_MSG_WITH_PAD("GUEST RSA Send Invalid Message", 1);
			exit(1);
		}
	}



	// 게스트가 호스트에게 공개키를 넘기는 과정
	{
		MAC_PRNT_MSG_WITH_PAD("Guest->Host", 1);


		/******************************************************************************/
		// 게스트로부터 공개키 전송 메시지를 수신하고, 다른 메시지일 때 에러를 발생
		// 시키고 종료합니다.
		RecvedBuf = NetComponent->Recv();
		if (strcmp(RecvedBuf, Message::Guest::SENDGUESTKEY.c_str()) != 0)
		{
			MAC_ERR_MSG_WITH_PAD("GUEST RSA Send Invalid", 1);
			exit(1);
		}

		// 게스트에게 ACCEPT 메시지를 전송합니다.
		NetComponent->Send(Message::Common::ACCEPT);



		/******************************************************************************/
		// 게스트로부터 가공된(헥사화된) 공개키를 수신합니다.
		std::string RecvedStr;
		CryptoPP::SecByteBlock GuestKeyBlock;
		CryptoPP::ByteQueue GuestQueue;
		RecvedStr = NetComponent->Recv();

		// 헥사화된 문자열을 바이너리로 바꾸고 다시 SecByteBlock으로 전환합니다.
		GuestKeyBlock = CryptoUtil::BlockizeString(CryptoUtil::StringnizeHex(RecvedStr));

		MAC_PRNT_START
			MAC_CLASS << MAC_FUNC << "RecvedStr : " << RecvedStr << MAC_LINENUM
			MAC_PRNT_END;






		// 게스트로부터 키 크기를 수신합니다.
		RecvedStr = NetComponent->Recv();
		size_t HostKeySize = static_cast<size_t>(std::stoi(RecvedStr));
		MAC_PRNT_START
			MAC_CLASS << MAC_FUNC << "HostKeySize : " << HostKeySize << MAC_LINENUM
			MAC_PRNT_END;


		// 공개키와 키 크기 정보를 이용해서 게스트의 공개키를 만듭니다.
		CryptoPP::RSA::PublicKey GuestPubKey;
		GuestQueue.Put2(GuestKeyBlock, HostKeySize, 0, true);
		GuestPubKey.Load(GuestQueue);

		// 호스트 공개키를 공개키 맵 컨테이너에 등록합니다.
		PublicKeyMap->insert(std::pair<std::string, CryptoPP::RSA::PublicKey>("COUNTER", GuestPubKey));

		// 공개키에 대한 정보를 파싱합니다.
		CryptoPP::Integer e = GuestPubKey.GetPublicExponent();
		CryptoPP::Integer n = GuestPubKey.GetModulus();

		MAC_PRNT_MSG_WITH_PAD
		(
			std::endl <<
			"HostPubKey.e : " << e << std::endl <<
			"HostPubKey.n : " << n,
			1
		);

		// 호스트에게 공개키를 받았음을 전송합니다.
		NetComponent->Send(Message::Common::RECVKEY);

	}



#ifdef CONSOLE_DEBUG
	std::cout << CLASS_TAG << FUNC_TAG << "END" << std::endl;
#endif // CONSOLE_DEBUG

}



void RSAEncryption::ExchangeKeyInGuestMode()
{
	MAC_PRNT_MSG_WITH_PAD
	(
		"",
		2
	);

	const char* RecvedBuf;

	// 호스트가 게스트에게 공개키를 넘기는 과정
	{
		MAC_PRNT_MSG_WITH_PAD("Host->Guest", 1);


		/******************************************************************************/
		// 호스트로부터 공개키 전송 메시지를 수신하고, 다른 메시지일 때 에러를 발생
		// 시키고 종료합니다.
		RecvedBuf = NetComponent->Recv();
		if (strcmp(RecvedBuf, Message::Host::SENDHOSTKEY.c_str()) != 0)
		{
			MAC_ERR_MSG_WITH_PAD("HOST RSA Send Invalid Message", 1);
			exit(1);
		}

		// 호스트에게 ACCEPT 메시지를 전송합니다.
		NetComponent->Send(Message::Common::ACCEPT);



		/******************************************************************************/
		// 호스트로부터 가공된(헥사화된) 공개키를 수신합니다.
		std::string RecvedStr;
		CryptoPP::SecByteBlock HostKeyBlock;
		CryptoPP::ByteQueue HostQueue;
		RecvedStr = NetComponent->Recv();
		
		// 헥사화된 문자열을 바이너리로 바꾸고 다시 SecByteBlock으로 전환합니다.
		HostKeyBlock = CryptoUtil::BlockizeString(CryptoUtil::StringnizeHex(RecvedStr));

		MAC_PRNT_START
			MAC_CLASS << MAC_FUNC << "RecvedStr : " << RecvedStr << MAC_LINENUM
		MAC_PRNT_END;




		// 호스트로부터 키 크기를 수신합니다.
		RecvedStr = NetComponent->Recv();
		size_t HostKeySize = static_cast<size_t>(std::stoi(RecvedStr));
		MAC_PRNT_START 
			MAC_CLASS <<MAC_FUNC << "HostKeySize : " << HostKeySize << MAC_LINENUM
		MAC_PRNT_END;
		

		// 공개키와 키 크기 정보를 이용해서 호스트의 공개키를 만듭니다.
		CryptoPP::RSA::PublicKey HostPubKey;
		HostQueue.Put2(HostKeyBlock, HostKeySize, 0, true);
		HostPubKey.Load(HostQueue);

		// 호스트 공개키를 공개키 맵 컨테이너에 등록합니다.
		PublicKeyMap->insert(std::pair<std::string, CryptoPP::RSA::PublicKey>("COUNTER", HostPubKey));

		// 공개키에 대한 정보를 파싱합니다.
		CryptoPP::Integer e = HostPubKey.GetPublicExponent();
		CryptoPP::Integer n = HostPubKey.GetModulus();
		MAC_PRNT_MSG_WITH_PAD
		(
			"HostPubKey.e : " << e << std::endl <<
			"HostPubKey.n : " << n,
			1
		);

		// 호스트에게 공개키를 받았음을 전송합니다.
		NetComponent->Send(Message::Common::RECVKEY);
	}





	// 게스트가 호스트에게 공개키를 넘기는 과정
	{
		MAC_PRNT_MSG_WITH_PAD("Guest->Host", 1);


		/******************************************************************************/
		// 호스트에게 자신의 공개키를 보내겠다고 전송합니다.
		NetComponent->Send(Message::Guest::SENDGUESTKEY);

		// 호스트가 ACCEPT 메시지를 보내지 않으면 에러를 발생시키고 종료합니다.
		RecvedBuf = NetComponent->Recv();
		if (strcmp(RecvedBuf, Message::Common::ACCEPT.c_str()) != 0)
		{
			MAC_ERR_MSG_WITH_PAD("HOST RSA Send Invalid Message", 1);
			exit(1);
		}





		/******************************************************************************/
		// 호스트에게 자신의 공개키를 가공하여 전송합니다.
		// 먼저, 자신의 공개키를 가져옵니다.
		auto Result = PublicKeyMap->find("SELF");
		if (Result == PublicKeyMap->end())
		{
			MAC_ERR_MSG_WITH_PAD("Self Public Key Does Not Established", 1);
			exit(1);
		}
		CryptoPP::RSA::PublicKey SelfPubKey = Result->second;

		// 이후, 공개키를 전송하기 위해 문자열로 변환하는 과정을 거칩니다.
		std::string HexStr;
		CryptoPP::ByteQueue Queue;
		SelfPubKey.Save(Queue);

		// 키 정보를 담을 버퍼를 선언하고 저장합니다.
		CryptoPP::byte PublicKeyBuffer[KeyBuffer];
		size_t Size = Queue.Get((CryptoPP::byte*)&PublicKeyBuffer, sizeof(PublicKeyBuffer));

		CryptoPP::SecByteBlock PublicKeyBlock{ PublicKeyBuffer, sizeof(PublicKeyBuffer) };
		HexStr = CryptoUtil::HexaizeString(CryptoUtil::StringnizeBlock(PublicKeyBlock));


		// 문자열에 쓰레기값을 들어가지 않게 하기 위해 resize합니다.
		HexStr.resize(HexStr.size());

		// 호스트에게 우리의 공개키를 전송합니다.
		NetComponent->Send(HexStr);

		// 호스트에게 키 사이즈를 전송합니다.
		std::string SizeStr(std::to_string(Size));
		NetComponent->Send(SizeStr);



		// 호스트가 키를 제대로 받지 않았거나 유효하지 않은 메시지를 수신하면
		// 에러를 발생시키고 종료합니다.
		RecvedBuf = NetComponent->Recv();
		if (strcmp(RecvedBuf, Message::Common::RECVKEY.c_str()) == 0)
		{
		}
		else if (strcmp(RecvedBuf, Message::Common::INVALIDKEY.c_str()) == 0)
		{
			MAC_ERR_MSG_WITH_PAD("HOST RSA Received Invalid Our Public Key ", 1);
			exit(1);
		}
		else
		{
			MAC_ERR_MSG_WITH_PAD("HOST RSA Send Invalid Message", 1);
			exit(1);
		}
	}


#ifdef CONSOLE_DEBUG
	std::cout << CLASS_TAG << FUNC_TAG << "END" << std::endl;
#endif // CONSOLE_DEBUG
}





void RSAEncryption::Encrypt(std::string & OutCipherText, std::string InPlainText)
{
	MAC_PRNT_MSG_WITH_PAD("PROCESSING", 1);

	////////////////////////////////////////////////////////////////////////////////
	// 암호화합니다.
	try
	{
		std::string BinaryCipherText;
		CryptoPP::RSAES_OAEP_SHA_Encryptor e(PublicKeyMap->at("COUNTER"));

		std::cout << MAC_FUNC << "1" << std::endl;

		// 공개키를 사용하여 평문을 암호문으로 만듭니다.
		CryptoPP::StringSource BinarySource
		(
			InPlainText, 
			true,
			new CryptoPP::PK_EncryptorFilter
			(
				Prng, 
				e,
				new CryptoPP::StringSink(BinaryCipherText)
			) // PK_EncryptorFilter
		); // StringSource

		std::cout << MAC_FUNC << "2" << std::endl;

		// 바이너리화된 암호문을 헥사화합니다.
		CryptoPP::StringSource HexSrouce
		(
			BinaryCipherText,
			true,
			new CryptoPP::HexEncoder
			(
				new CryptoPP::StringSink(OutCipherText)
			)
		);
		
		std::cout << MAC_FUNC << "3" << std::endl;
	}
	catch (CryptoPP::Exception& e)
	{
		MAC_ERR_MSG_WITH_PAD(e.what(), 1);
	}

	MAC_PRNT_MSG_WITH_PAD("PROCESS END", 1);
}



void RSAEncryption::Decrypt(std::string & OutPlainText, std::string InCipherText)
{
	////////////////////////////////////////////////
	// 복호화합니다.
	try
	{
		CryptoPP::RSAES_OAEP_SHA_Decryptor Decryptor(*PrivateKey);

		CryptoPP::StringSource DecryptStringSource(InCipherText, true,
			new CryptoPP::PK_DecryptorFilter(Prng, Decryptor,
				new CryptoPP::StringSink(OutPlainText)
			) // PK_DecryptorFilter
		); // StringSource
	}
	catch (CryptoPP::Exception& e)
	{
		MAC_ERR_MSG_WITH_PAD(e.what(), 1);
	}
}
