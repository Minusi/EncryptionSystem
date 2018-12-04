#include "Encryptor.h"

#include "AESnCBCState.h"
#include "DESnCBCState.h"
#include "TDESnCBCState.h"

#include "AESnECBState.h"
#include "DESnECBState.h"
#include "TDESnECBState.h"

#include "AESnCTRState.h"
#include "DESnCTRState.h"
#include "TDESnCTRState.h"



Encryptor::Encryptor()
{
	RegisterConfiguration("AES", "CBC", true);
}

Encryptor::Encryptor(std::string InAlgorithm, std::string InBlockMode, bool InTargetIsData)
{
	RegisterConfiguration(InAlgorithm, InBlockMode, InTargetIsData);
}

Encryptor::~Encryptor()
{
	delete CryptoState;
}



void Encryptor::RegisterConfiguration(std::string InAlgorithm, std::string InBlockMode, bool InTargetIsData)
{
	bTargetIsData = InTargetIsData;

	if (InBlockMode == "ECB" || InBlockMode == "ecb")
	{
		if (InAlgorithm == "DES" || InAlgorithm == "des")
		{
			CryptoState = static_cast<ICryptoState*>(new DESnECBState());
		}
		else if (InAlgorithm == "3DES" || InAlgorithm == "3des")
		{
			CryptoState = static_cast<ICryptoState*>(new TDESnECBState());
		}
		else if (InAlgorithm == "AES" || InAlgorithm == "aes")
		{
			CryptoState = static_cast<ICryptoState*>(new AESnECBState());
		}
	}
	else if (InBlockMode == "CBC" || InBlockMode == "cbc")
	{
		if (InAlgorithm == "DES" || InAlgorithm == "des")
		{
			CryptoState = static_cast<ICryptoState*>(new DESnCBCState());
		}
		else if (InAlgorithm == "3DES" || InAlgorithm == "3des")
		{
			CryptoState = static_cast<ICryptoState*>(new TDESnCBCState());
		}
		else if (InAlgorithm == "AES" || InAlgorithm == "aes")
		{
			CryptoState = static_cast<ICryptoState*>(new AESnCBCState());
		}
	}
	else if (InBlockMode == "CTR" || InBlockMode == "ctr")
	{
		if (InAlgorithm == "DES" || InAlgorithm == "des")
		{
			CryptoState = static_cast<ICryptoState*>(new DESnCTRState());
		}
		else if (InAlgorithm == "3DES" || InAlgorithm == "3des")
		{
			CryptoState = static_cast<ICryptoState*>(new TDESnCTRState());
		}
		else if (InAlgorithm == "AES" || InAlgorithm == "aes")
		{
			CryptoState = static_cast<ICryptoState*>(new AESnCTRState());
		}
	}
	else
	{
		std::cerr << "ENCRYPTOR:: 허가되지 않은 모드 접근입니다." << std::endl;
	}
}



void Encryptor::Execute()
{
	if (CryptoState == nullptr)
	{
		std::cerr << "ENCRYPTOR:: 암호 모듈의 설정이 완료되지 않았습니다" << std::endl;
	}

	switch (bTargetIsData)
	{
	case true:
		CryptoState->ExecuteWithData();
		break;
	case false:
		CryptoState->ExecuteWithFile();
		break;
	}
}

