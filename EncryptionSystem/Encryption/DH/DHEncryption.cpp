#include "DHEncryption.h"



DHEncryption::DHEncryption()
{

}


DHEncryption::~DHEncryption()
{
}

void DHEncryption::InitializeEncryption()
{
	//////////////////////////////////////////////////////////////////////////////
	// 도메인 매개 변수를 생성합니다.
	CryptoPP::PrimeAndGenerator pg;
	pg.Generate(1, Prng, 512, 511);

	DomainParamSet.p = pg.Prime();
	DomainParamSet.q = pg.SubPrime();
	DomainParamSet.g = pg.Generator();

	DHMethod.AccessGroupParameters().Initialize
	(
		DomainParamSet.p, 
		DomainParamSet.q, 
		DomainParamSet.g
	);
	if (DHMethod.GetGroupParameters().ValidateGroup(Prng, 3) == false)
	{
		std::cout << "DH::Error::Failed To Valiate Prime And Generator" << std::endl;
		exit(1);
	}

	CryptoPP::Integer v = CryptoPP::ModularExponentiation
	(
		DomainParamSet.p, 
		DomainParamSet.q, 
		DomainParamSet.p
	);
	if (v != CryptoPP::Integer::One())
	{
		std::cout << "DH::Error::Failed To Verify Order Of The Subgroup";
		exit(1);
	}

	//////////////////////////////////////////////////////////////////////////////
	// 비대칭 키를 생성합니다.
	StrPrivateKey = CryptoPP::SecByteBlock(DHMethod.PrivateKeyLength());
	StrPublicKey = CryptoPP::SecByteBlock(DHMethod.PublicKeyLength());
	DHMethod.GenerateKeyPair(Prng, StrPrivateKey, StrPublicKey);
}

void DHEncryption::Encrypt(std::string & OutCipherText, std::string InPlainText)
{
}

void DHEncryption::Decrypt(std::string & OutPlainText, std::string InCipherText)
{
}

DHEncryption::DomainParams::DomainParams
(
	CryptoPP::Integer Inp, 
	CryptoPP::Integer Inq, 
	CryptoPP::Integer Ing
)
	: p(Inp)
	, q(Inq)
	, g(Ing)
{
}
