#pragma once

#include <secblock.h>
#include <aes.h>
#include <modes.h>
#include <filters.h>

#include <base64.h>

#include "ICryptoState.h"



class AESnECBState : public ICryptoState
{
public:
	// 기본 생성자입니다.
	AESnECBState();

	virtual void ExecuteWithData() override;
	virtual void ExecuteWithFile() override;


private:
	// 암호화를 수행할 키입니다.
	CryptoPP::SecByteBlock Key;
};