﻿#include "AESnECBState.h"

#include <osrng.h>
#include <hex.h>
#include <base64.h>

#include "GlobalConfig.h"


AESnECBState::AESnECBState() : Key(CryptoPP::SecByteBlock(0x00, CryptoPP::AES::DEFAULT_KEYLENGTH))
{
	//초기화 프로세스
	{
		CryptoPP::AutoSeededRandomPool Prng;

		Prng.GenerateBlock(Key, sizeof(Key));
		std::cout << sizeof(Key) << std::endl;
	}
}



void AESnECBState::ExecuteWithData()
{
	std::cout << "ENCRYPTOR:: AES-ECB 방식의 암호화 프로세스 진행중..." << std::endl;

	// 암호화 프로세스
	try
	{

		CryptoPP::ECB_Mode<CryptoPP::AES>::Encryption CBCEncryption;
		CBCEncryption.SetKey(Key, Key.size());

		CryptoPP::StringSource Source
		(
			PlainText,
			true,
			new CryptoPP::StreamTransformationFilter
			(
				CBCEncryption, new CryptoPP::StringSink(CipherText)
			)
		);
	}
	catch (const CryptoPP::Exception& e)
	{
		std::cerr << e.what() << std::endl;
		exit(1);
	}


	// 복호화 프로세스
	try
	{
		CryptoPP::ECB_Mode<CryptoPP::AES>::Decryption CBCDecryption;
		CBCDecryption.SetKey(Key, Key.size());

		CryptoPP::StringSource Source
		(
			CipherText,
			true,
			new CryptoPP::StreamTransformationFilter
			(
				CBCDecryption, new CryptoPP::StringSink(DecryptedText)
			)
		);
	}
	catch (const CryptoPP::Exception& e)
	{
		std::cerr << e.what() << std::endl;
		exit(1);
	}

	// 헥사코드 인코딩
	{
		CryptoPP::StringSource Source
		(
			CipherText,
			true,
			new CryptoPP::HexEncoder(new CryptoPP::StringSink(HexCipherText))
		);
	}

	// Base64 인코딩
	{
		CryptoPP::StringSource Source
		(
			CipherText,
			true,
			new CryptoPP::Base64Encoder(new CryptoPP::StringSink(Base64CipherText))
		);
	}
}

void AESnECBState::ExecuteWithFile()
{
	std::cout << "ENCRYPTOR:: AES-ECB 방식의 암호화 프로세스 진행중..." << std::endl;
	std::cout << "ENCRYPTOR:: 프로세스의 타겟 확인 >> 파일 " << std::endl;

	std::string InputFilePath = DefaultTargetFile;
	std::string CipherFilePath = DefaultTargetPath + AppendAES + AppendECB + AppendNo + CipherFileName;
	std::string DecryptedFilePath = DefaultTargetPath + AppendAES + AppendECB + AppendNo + DecryptedFileName;

	// 암호화 프로세스
	try
	{
		CryptoPP::ECB_Mode<CryptoPP::AES>::Encryption CBCEncryption;
		CBCEncryption.SetKey(Key, Key.size());

		CryptoPP::FileSource Source
		(
			InputFilePath.c_str(),
			true,
			new CryptoPP::StreamTransformationFilter
			(
				CBCEncryption, new CryptoPP::FileSink(CipherFilePath.c_str())
			)
		);
	}
	catch (const CryptoPP::Exception& e)
	{
		std::cout << "ENCRYPTOR:: 암호화 프로세스에서의 예외 발생" << std::endl;

		std::cerr << e.what() << std::endl;
		exit(1);
	}


	// 복호화 프로세스
	try
	{
		CryptoPP::ECB_Mode<CryptoPP::AES>::Decryption CBCDecryption;
		CBCDecryption.SetKey(Key, Key.size());

		CryptoPP::FileSource Source
		(
			CipherFilePath.c_str(),
			true,
			new CryptoPP::StreamTransformationFilter
			(
				CBCDecryption, new CryptoPP::FileSink(DecryptedFilePath.c_str())
			)
		);
	}
	catch (const CryptoPP::Exception& e)
	{
		std::cout << "ENCRYPTOR:: 복호화 프로세스에서의 예외 발생" << std::endl;
		std::cerr << e.what() << std::endl;
		exit(1);
	}
}
