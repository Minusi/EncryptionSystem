#include "AESnCBCState.h"

#include <osrng.h>
#include <hex.h>
#include <base64.h>

#include "GlobalConfig.h"

AESnCBCState::AESnCBCState() : Key(CryptoPP::SecByteBlock(CryptoPP::AES::DEFAULT_KEYLENGTH))
{
	//초기화 프로세스
	{
		CryptoPP::AutoSeededRandomPool Prng;

		Prng.GenerateBlock(Key, sizeof(Key));
		Prng.GenerateBlock(IV, sizeof(IV));
	}
}



void AESnCBCState::ExecuteWithData()
{
	std::cout << "ENCRYPTOR:: AES-CBC 방식의 암호화 프로세스 진행중..." << std::endl;
	std::cout << "ENCRYPTOR:: 프로세스의 타겟 확인 >> 파일 " << std::endl;

	// 암호화 프로세스
	try
	{

		CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption CBCEncryption;
		CBCEncryption.SetKeyWithIV(Key, Key.size(), IV);

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
	catch(const CryptoPP::Exception& e)
	{
		std::cerr << e.what() << std::endl;
		exit(1);
	}


	// 복호화 프로세스
	try
	{
		CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption CBCDecryption;
		CBCDecryption.SetKeyWithIV(Key, Key.size(), IV);

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

void AESnCBCState::ExecuteWithFile()
{
	std::cout << "ENCRYPTOR:: AES-CBC 방식의 암호화 프로세스 진행중..." << std::endl;
	std::cout << "ENCRYPTOR:: 프로세스의 타겟 확인 >> 데이터 " << std::endl;

	std::string InputFilePath = DefaultTargetFile;
	std::string CipherFilePath = DefaultTargetPath + AppendAES + AppendCBC + AppendNo + CipherFileName;
	std::string DecryptedFilePath = DefaultTargetPath + AppendAES + AppendCBC + AppendNo + DecryptedFileName;

	std::ifstream InputFileStream(InputFilePath, std::ios::binary | std::ios::in);
	std::ifstream CipherFileStream(CipherFilePath, std::ios::binary | std::ios::out);
	std::ifstream DecryptedFileStream(DecryptedFilePath, std::ios::binary | std::ios::out);

	// 암호화 프로세스
	try
	{
		CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption CBCEncryption;
		CBCEncryption.SetKeyWithIV(Key, Key.size(), IV);

		CryptoPP::FileSource Source
		(
			InputFileStream,
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
		CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption CBCDecryption;
		CBCDecryption.SetKeyWithIV(Key, Key.size(), IV);

		CryptoPP::FileSource Source
		(
			CipherFileStream,
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