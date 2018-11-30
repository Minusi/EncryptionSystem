#include "RSAEncryption.h"

#include <sstream>
#include <iostream>
#include <cassert>



RSAEncryption::RSAEncryption()
{
	InitializeEncryption();
}


RSAEncryption::~RSAEncryption()
{
}

void RSAEncryption::InitializeEncryption()
{
	////////////////////////////////////////////////
	// 키를 생성합니다.
	try
	{
		CryptoPP::InvertibleRSAFunction Params;
		Params.GenerateRandomWithKeySize(Rng, 1536);

		PrivateKeyPtr.reset(new CryptoPP::RSA::PrivateKey(Params));
		PublicKeyPtr.reset(new CryptoPP::RSA::PublicKey(Params));
	}
	catch (CryptoPP::Exception& e)
	{
		e.what();
	}
}

void RSAEncryption::Encrypt(std::string & OutCipherText, std::string InPlainText)
{
	////////////////////////////////////////////////
	// 암호화합니다.
	try
	{
		CryptoPP::RSAES_OAEP_SHA_Encryptor Encryptor(*PublicKeyPtr);

		CryptoPP::StringSource EncryptStringSource(InPlainText, true,
			new CryptoPP::PK_EncryptorFilter(Rng, Encryptor,
				new CryptoPP::StringSink(OutCipherText)
			) // PK_EncryptorFilter
		); // StringSource

	}
	catch (CryptoPP::Exception& e)
	{
		e.what();
	}
}

void RSAEncryption::Decrypt(std::string & OutPlainText, std::string InCipherText)
{
	////////////////////////////////////////////////
	// 복호화합니다.
	try
	{
		CryptoPP::RSAES_OAEP_SHA_Decryptor Decryptor(*PrivateKeyPtr);

		CryptoPP::StringSource DecryptStringSource(InCipherText, true,
			new CryptoPP::PK_DecryptorFilter(Rng, Decryptor,
				new CryptoPP::StringSink(OutPlainText)
			) // PK_DecryptorFilter
		); // StringSource
	}
	catch (CryptoPP::Exception& e)
	{
		e.what();
	}
}