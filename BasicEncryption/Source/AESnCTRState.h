#pragma once

#include <secblock.h>
#include <aes.h>
#include <modes.h>
#include <filters.h>

#include <base64.h>

#include "ICryptoState.h"



class AESnCTRState : public ICryptoState
{
public:
	// 기본 생성자입니다.
	AESnCTRState();

	virtual void ExecuteWithData() override;
	virtual void ExecuteWithFile() override;


private:
	// 암호화를 수행할 키입니다.
	CryptoPP::SecByteBlock Key;

	// CTR모드에 필요한 카운터입니다.
	byte CTR[CryptoPP::AES::DEFAULT_BLOCKSIZE];
};