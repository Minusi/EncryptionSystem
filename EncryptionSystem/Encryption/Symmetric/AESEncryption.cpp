#include "AESEncryption.h"

#include <cryptopp700\hex.h>

#include "Debug\Debug.h"



AESEncryption::AESEncryption
(
	const bool bManualKeyGen,
	const std::string InManualKey,
	const int InKeySize)
	: Super()
	, KeySize(InKeySize)
	, bUseManualKey(bManualKeyGen)
	, CacheKeyStr(InManualKey)
{
	if (bUseManualKey == true && CacheKeyStr.empty())
	{
		CacheKeyStr = "essasenhaehfraca";
	}
	InitializeEncryption();
}

void AESEncryption::InitializeEncryption()
{
#ifdef AES_TEST
	testkey = CryptoPP::SecByteBlock(CryptoPP::AES::DEFAULT_KEYLENGTH);
	testprng.GenerateBlock(testkey, testkey.size());
	testprng.GenerateBlock(testIV, sizeof(testIV));
#else

	// 랜덤 풀과 키 및 초기벡터에 대해 초기화를 수행합니다.
	Prng.reset(new CryptoPP::AutoSeededRandomPool());
	Key.reset(new CryptoPP::SecByteBlock(KeySize));

	constexpr int IVSize = CryptoPP::AES::BLOCKSIZE;
	IV = std::make_unique<byte[]>(IVSize);

	// Key와 IV 블록에 대해 0으로 초기화를 수행합니다.
	std::fill(Key->begin(), Key->end(), 0);
	//memcpy(IV.get(), 0, sizeof(IVSize));

	if (bUseManualKey == false)
	{
		Prng->GenerateBlock(*Key, Key->size());
		Prng->GenerateBlock(IV.get(), sizeof(IVSize));
	}
	else
	{
		GenerateKeyManually();
	}

	CryptoPP::SecByteBlock DbgBlock = *Key;
	CryptoPP::SecByteBlock DbgBlock2((IV.get(), sizeof(IVSize)));
	MAC_PRNT_MSG_WITH_PAD
	(
		"KEY : " << CryptoUtil::HexaizeString(CryptoUtil::StringnizeBlock(DbgBlock)) <<
		"\t" << Key->size() << std::endl <<
		"IV  : " << CryptoUtil::HexaizeString(CryptoUtil::StringnizeBlock(DbgBlock2)) <<
		"\t" << sizeof(IVSize),
		1
	);

#endif // AES_TEST
}

void AESEncryption::GenerateKeyManually()
{
#ifdef CONSOLE_DEBUG
	std::cout << CLASS_TAG << FUNC_TAG << std::endl;
#endif // CONSOLE_DEBUG

	
	// 수동으로 Key를 생성합니다.
	std::string ManualIV = CacheKeyStr;
	for (int i = 0; i < static_cast<int>(KeySize); ++i)
	{
		(*Key)[i] = static_cast<CryptoPP::byte>(CacheKeyStr[i]);
	}

	// 수동으로 IV를 생성합니다.
	memcpy(IV.get(), CacheKeyStr.data(), CacheKeyStr.size());

#ifdef CONSOLE_DEBUG
	std::string DebugString
	(
		reinterpret_cast<const char*>(&(*Key)[0]), Key->size()
	);

	MAC_PRNT_MSG_WITH_PAD(std::endl << "DebugString : " << CryptoUtil::HexaizeString(DebugString), 1);
#endif // CONSOLE_DEBUG

}



void AESEncryption::Encrypt(std::string & OutCipherText, std::string InPlainText)
{
	try
	{
#ifdef AES_TEST
		// Test 변수에 대한 AES-CBC 암호화 실시
		CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption e;
		e.SetKeyWithIV(testkey, testkey.size(), testIV);

		CryptoPP::StringSource ss
		(
			InPlainText,
			true,
			new CryptoPP::StreamTransformationFilter(e,
				new CryptoPP::StringSink(OutCipherText))
		);


#ifdef CONSOLE_DEBUG
		std::string HexString;
		CryptoPP::StringSource hexss
		(
			OutCipherText,
			true,
			new CryptoPP::HexEncoder((new CryptoPP::StringSink(HexString)))
		);
		std::cout << "Plain  : " << InPlainText << std::endl;
		std::cout << "Cipher : " << HexString << "\t" << HexString.size() << std::endl;
#endif // CONSOLE_DEBUG



#else

		std::string BinaryCipherText;
		CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption e;
		e.SetKeyWithIV(*Key, Key->size(), IV.get());

		CryptoPP::StringSource BinarySource
		(
			InPlainText,
			true,
			new CryptoPP::StreamTransformationFilter(e,
				new CryptoPP::StringSink(BinaryCipherText))
		);

		CryptoPP::StringSource HexSource
		(
			BinaryCipherText,
			true,
			new CryptoPP::HexEncoder(new CryptoPP::StringSink(OutCipherText))
		);	

#ifdef CONSOLE_DEBUG
		CON_NEWLINE;
		std::cout << CLASS_TAG << FUNC_TAG <<
			"HexSource >> " << OutCipherText << "\t" << OutCipherText.size() << std::endl;
#endif // CONSOLE_DEBUG
#endif // AES_TEST
	}
	catch (const CryptoPP::Exception& e)
	{
		std::cerr << e.what() << std::endl;
		exit(1);
	}

}

void AESEncryption::Decrypt(std::string & OutPlainText, std::string InCipherText)
{
	try
	{
#ifdef CONSOLE_DEBUG
		CON_NEWLINE;
		std::cout << CLASS_TAG << FUNC_TAG <<
			"HexCipher >> " << InCipherText << "\t" << InCipherText.size() << std::endl;
#endif // CONSOLE_DEBUG

		std::string BinaryCipherText;
		CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption d;
		d.SetKeyWithIV(*Key, Key->size(), IV.get());

		CryptoPP::StringSource BinarySource
		(
			InCipherText,
			true,
			new CryptoPP::HexDecoder((new CryptoPP::StringSink(BinaryCipherText)))
		);

		CryptoPP::StringSource ss
		(
			BinaryCipherText,
			true,
			new CryptoPP::StreamTransformationFilter(d,
				new CryptoPP::StringSink(OutPlainText))
		);


#ifdef CONSOLE_DEBUG
		CON_NEWLINE;
		std::cout << CLASS_TAG << FUNC_TAG <<
			"PlainText >> " << OutPlainText << "\t" << OutPlainText.size() << std::endl;
#endif // CONSOLE_DEBUG
	}
	catch (const CryptoPP::Exception& e)
	{
		std::cout << "AES::ERROR::" << e.what() << std::endl;
		exit(1);
	}
}