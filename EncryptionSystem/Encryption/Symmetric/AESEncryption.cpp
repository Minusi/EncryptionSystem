#include "AESEncryption.h"

AESEncryption::AESEncryption(const int InKeySize)
	: Super()
	, KeySize(InKeySize)
{
	InitializeEncryption();
}

void AESEncryption::InitializeEncryption()
{
	Prng.reset(new CryptoPP::AutoSeededRandomPool());


	Key.reset( new CryptoPP::SecByteBlock(KeySize));
	Prng->GenerateBlock(*Key, Key->size());

	IV.reset( new CryptoPP::byte[KeySize] );
	Prng->GenerateBlock(IV.get(), sizeof(IV.get()));

}

void AESEncryption::Encrypt(std::string & OutCipherText, std::string InPlainText)
{
	try
	{
		CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption e;
		e.SetKeyWithIV(*Key, Key->size(), IV.get());

		std::unique_ptr<CryptoPP::StringSink> strsnk
		{
			new CryptoPP::StringSink(OutCipherText)
		};

		std::unique_ptr<CryptoPP::StreamTransformationFilter> stf
		{
			new CryptoPP::StreamTransformationFilter(e, strsnk.get())
		};

		CryptoPP::StringSource ss
		(
			InPlainText,
			true,
			stf.get()
		);
	}
	catch (const CryptoPP::Exception& e)
	{
		std::cout << "AES::Error::" << e.what() << std::endl;
		exit(1);
	}
}

void AESEncryption::Decrypt(std::string & OutPlainText, std::string InCipherText)
{
	try
	{
		CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption d;
		d.SetKeyWithIV(*Key, Key->size(), IV.get());

		std::unique_ptr<CryptoPP::StringSink> strsnk
		{
			new CryptoPP::StringSink(OutPlainText)
		};

		std::unique_ptr<CryptoPP::StreamTransformationFilter> stf
		{
			new CryptoPP::StreamTransformationFilter(d, stf.get())
		};

		CryptoPP::StringSource ss
		(
			InCipherText,
			true,
			stf.get()
		);
	}
	catch (const CryptoPP::Exception& e)
	{
		std::cout << "AES::ERROR::" << e.what() << std::endl;
		exit(1);
	}
}

AESEncryption* GlobalAES = new AESEncryption();