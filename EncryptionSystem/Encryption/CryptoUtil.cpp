#include "CryptoUtil.h"




CryptoPP::Integer CryptoUtil::IntegerizeString(std::string InString, bool bHexInput)
{
	std::string Recovered;
	if (bHexInput == true)
	{
		Recovered = StringnizeHex(InString);
	}
	else
	{
		Recovered = InString;
	}


#ifdef CONSOLE_DEBUG
	CON_NEWLINE;
	CON_NEWLINE;
	std::cout << CRYPUT_TAG << FUNC_TAG << "\n" <<
		"InString : " << InString << std::endl << std::endl <<
		"Integer  : " << CryptoPP::Integer(Recovered.c_str()) << std::endl;
	CON_NEWLINE;
	CON_NEWLINE;
#endif // CONSOLE_DEBUG


	return CryptoPP::Integer(Recovered.c_str());
}

std::string CryptoUtil::StringnizeInteger(CryptoPP::Integer & InInteger, bool bHexResult)
{
	std::ostringstream Oss;
	std::string Stringnized;
	Oss << InInteger;

	if (bHexResult == true)
	{
		Stringnized = HexaizeString(Oss.str());
	}
	else
	{
		Stringnized = Oss.str();
	}


#ifdef CONSOLE_DEBUG
	CON_NEWLINE;
	CON_NEWLINE;
	std::cout << CRYPUT_TAG << FUNC_TAG << "\n" <<
		"InInteger : " << InInteger << std::endl <<
		"ResultStr : " << Stringnized << std::endl;
	CON_NEWLINE;
	CON_NEWLINE;
#endif // CONSOLE_DEBUG


	return Stringnized;
}



CryptoPP::SecByteBlock CryptoUtil::BlockizeString(std::string InString)
{
	return CryptoPP::SecByteBlock
	(
		reinterpret_cast<const CryptoPP::byte*>(&InString[0]), InString.size()
	);
}

std::string CryptoUtil::StringnizeBlock(CryptoPP::SecByteBlock& InBlock)
{
	return std::string(reinterpret_cast<const char*>(&InBlock[0]), InBlock.size());
}



std::string CryptoUtil::HexaizeString(const std::string InString)
{
	std::string HexString;
	CryptoPP::StringSource HexSource
	(
		InString,
		true,
		new CryptoPP::HexEncoder(new CryptoPP::StringSink(HexString))
	);

	return HexString;
}

std::string CryptoUtil::StringnizeHex(const std::string InHexString)
{
	std::string Stringnized;
	CryptoPP::StringSource StringSource
	(
		InHexString,
		true,
		new CryptoPP::HexDecoder(new CryptoPP::StringSink(Stringnized))
	);

	return Stringnized;
}