#pragma once

#include <secblock.h>
#include <des.h>
#include <modes.h>
#include <filters.h>

#include <base64.h>

#include "ICryptoState.h"



class TDESnCBCState : public ICryptoState
{
public:
	// 기본 생성자입니다.
	TDESnCBCState();

	virtual void ExecuteWithData() override;
	virtual void ExecuteWithFile() override;


private:
	// 암호화를 수행할 키입니다.
	CryptoPP::SecByteBlock Key;

	// CBC모드에 필요한 초기 벡터입니다.
	byte IV[CryptoPP::DES_EDE3::DEFAULT_BLOCKSIZE];
};