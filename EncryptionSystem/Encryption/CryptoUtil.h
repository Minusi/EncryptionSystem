#pragma once

#include <string>
#include <strstream>

#include <cryptopp700\modes.h>
#include <cryptopp700\filters.h>
#include <cryptopp700\hex.h>
#include <cryptopp700\cryptlib.h>
#include <cryptopp700\queue.h>
#include <cryptopp700\rsa.h>

#include "Debug\Debug.h"
#include "Debug\Macro.h"



namespace CryptoUtil
{
	// 문자열을 Integer화합니다. 그 이외의 동작에 대해서는
	// 올바른 동작을 보증할 수 없습니다. 모드에 따라서 순수
	// 문자열을 Integer로 변환하는 기능과 헥사화된 문자열을
	// Integer로 변환하는 기능을 출력할 수 있습니다.
	CryptoPP::Integer IntegerizeString(std::string InString, bool bHexInput = false);

	// Integer를 문자열화합니다. 그 이외의 동작에 대해서는
	// 올바른 동작을 보증할 수 없습니다. 모드에 따라서 순수 
	// 문자열과 헥사화된 문자열을 출력할 수 있습니다.
	std::string StringnizeInteger(CryptoPP::Integer& InInteger, bool bHexResult = false);



	// 문자열을 SecByteBlock화합니다. 그 이외의 동작에 대해서는
	// 올바른 동작을 보증할 수 없습니다.
	CryptoPP::SecByteBlock BlockizeString(std::string InString);

	// SecByteBlock을 문자열화합니다. 그 이외의 동작에 대해서는
	// 올바른 동작을 보증할 수 없습니다.
	std::string StringnizeBlock(CryptoPP::SecByteBlock& InBlock);



	// 보통의 문자열을 헥사화합니다. 그 이외의 동작에 대해서는
	// 올바른 동작을 보증할 수 없습니다.
	std::string HexaizeString(const std::string InString);

	// 헥사화된 문자열을 보통의 문자열로 복구합니다. 그 이외의 동작
	// 에 대해서는 올바른 동작을 보증할 수 없습니다.
	std::string StringnizeHex(const std::string InHexString);
}