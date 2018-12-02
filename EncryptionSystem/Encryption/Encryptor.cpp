#include "Encryptor.h"

#include <iostream>



Encryptor::Encryptor(const SOCKET* InSocket, EMode InMode)
	: PassSocket( InSocket )
{
	ChangeModule(InMode);
}

void Encryptor::ChangeModule(EMode InMode)
{
	EncryptionMode = InMode;

	// 비암호화 모듈일 때 현재 암호를 하지 않는다는 경고를 출력합니다.
	if (EncryptionMode == EMode::NONE)
	{
		if (EncryptionModule == nullptr)
		{
			std::cout << "ENCRYPT::ALARM::NONE Value Detected" << std::endl;
			system("pause");
		}
		else
		{
			EncryptionModule.reset();
		}
	}

	if (EncryptionMode == EMode::AES)
	{
		EncryptionModule.reset(GlobalAES);
	}
	else if (EncryptionMode == EMode::DH)
	{
		if (IsSocketValid() == false)
		{
			std::cout << "ENCRYPT::ERROR::SOCKET Is Not Valid In " << __func__ << std::endl;
			exit(1);
		}

		INetDelegateComponent* NetDeleParam = new NetAvailable(PassSocket);
		EncryptionModule.reset(new DHEncryption{ NetDeleParam });
	}
	else if (EncryptionMode == EMode::RSA)
	{
		EncryptionModule.reset(new RSAEncryption());
	}
}

bool Encryptor::IsSocketValid() const
{
	if (PassSocket == nullptr)
	{
		return false;
	}

	return true;
}

void Encryptor::ToCipher(std::string & OutCipherText, std::string InPlainText)
{
	EncryptionModule->Encrypt(OutCipherText, InPlainText);
}

void Encryptor::ToPlain(std::string & OutPlainText, std::string InCipherText)
{
	EncryptionModule->Decrypt(OutPlainText, InCipherText);
}
