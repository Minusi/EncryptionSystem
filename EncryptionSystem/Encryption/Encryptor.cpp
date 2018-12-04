#include "Encryptor.h"



Encryptor::Encryptor(const SOCKET* InSocket)
	: PassSocket( InSocket )
	, PassNetMode( EncryptionBase::NetMode::NONE)
	, EncryptionMode( EMode::NONE )
{
}

void Encryptor::ChangeModule(EMode InMode)
{
	EncryptionMode = InMode;

	// 비암호화 모듈일 때 현재 암호를 하지 않는다는 경고를 출력합니다.
	if (EncryptionMode == EMode::NONE)
	{
		if (EncryptionModule == nullptr)
		{
			std::cout << "ENCRYPT::ALARM::Duplicated NONE >> " <<
				"Already None Encryption Module" << std::endl;
			system("pause");
		}
		else
		{
			EncryptionModule.reset();
		}
	}

	

	// 사용할 알고리즘 모드가 AES일 경우를 처리합니다.
	else if (EncryptionMode == EMode::AES)
	{
		if (IsSocketValid() == false)
		{
			std::cout << "ENCRYPT::ERROR::SOCKET Is Not Valid In " << __func__ << std::endl;
			exit(1);
		}

		EncryptionModule.reset(new AESEncryption(true));
	}


	// 사용할 알고리즘 모드가 DH 키교환일 경우를 처리합니다.
	else if (EncryptionMode == EMode::DH)
	{
		if (IsSocketValid() == false)
		{
			std::cout << "ENCRYPT::ERROR::SOCKET Is Not Valid In " << __func__ << std::endl;
			exit(1);
		}

		INetDelegateComponent* NetDeleParam = new NetAvailable<5>(PassSocket);
		EncryptionModule.reset(new DHEncryption{ NetDeleParam, PassNetMode, 256 });
	}



	// 사용할 알고리즘 모드가 RSA일 경우를 처리합니다.
	else if (EncryptionMode == EMode::RSA)
	{
		if (IsSocketValid() == false)
		{
			MAC_ERR_MSG_WITH_PAD("Socket Is Not Valid", 1);
			exit(1);
		}

		INetDelegateComponent* NetDeleParam = new NetAvailable<5>(PassSocket);
		EncryptionModule.reset(new RSAEncryption{ NetDeleParam, PassNetMode});
	}



	// 사용할 알고리즘 모드가 OPENSSL일 경우를 처리합니다.
	else if (EncryptionMode == EMode::OPENSSL)
	{
		// TODO : OPENSSL 구현할 것
	}
}

void Encryptor::CoInitNetworkMode(EncryptionBase::NetMode InNetMode)
{
	PassNetMode = InNetMode;
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
	MAC_PRNT_MSG_WITH_PAD("EncryptionModule->Encrypt(T,U)", 1);
	EncryptionModule->Encrypt(OutCipherText, InPlainText);
}

void Encryptor::ToPlain(std::string & OutPlainText, std::string InCipherText)
{
	EncryptionModule->Decrypt(OutPlainText, InCipherText);
}