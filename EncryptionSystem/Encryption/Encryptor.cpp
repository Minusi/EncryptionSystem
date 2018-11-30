#include "Encryptor.h"

#include <iostream>



Encryptor::Encryptor()
{
}


Encryptor::~Encryptor()
{
}

void Encryptor::ChangeModule(Mode InMode)
{
	if (InMode == Mode::AES)
	{
		// TODO : AES 암호모듈 구현후 작성할 것
	}
	else if (InMode == Mode::DH)
	{
		// TODO : DH 암호모듈 구현후 작성할 것
	}
	else if (InMode == Mode::RSA)
	{
		EncryptModule.reset(new RSAEncryption());
	}
	else if (InMode == Mode::NONE)
	{
		std::cout << "ENCRYPT::ALARM::NONE Value Detected" << std::endl;
		system("pause");
	}
}
