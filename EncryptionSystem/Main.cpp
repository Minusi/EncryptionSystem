#include <iostream>

#include "Client/CalcClient.h"
#include "Server/CalcServer.h"

#include "NetBase.h"

int main()
{
	bool IsServer;

	std::cout << "SYSTEM::Client(0), Server(1) : ";
	std::cin >> IsServer;

	if (IsServer)
	{
		CalcServer();
	}
	else
	{
		CalcClient();
	}

	std::cout << "APP::Exit" << std::endl;

	return 0;
}














//
//
//// g++ -g3 -ggdb -O0 -I. -I/usr/include/cryptopp dh-agree.cpp -o dh-agree.exe -lcryptopp -lpthread
//// g++ -g -O2 -I. -I/usr/include/cryptopp dh-agree.cpp -o dh-agree.exe -lcryptopp -lpthread
//
//#include <iostream>
//using std::cout;
//using std::cerr;
//using std::endl;
//
//#include <string>
//using std::string;
//
//#include <stdexcept>
//using std::runtime_error;
//
//#include "cryptopp700\osrng.h"
//
//using CryptoPP::AutoSeededRandomPool;
//
//#include "cryptopp700\integer.h"
//using CryptoPP::Integer;
//
//#include "cryptopp700\nbtheory.h"
//using CryptoPP::ModularExponentiation;
//
//#include "cryptopp700\dh.h"
//using CryptoPP::DH;
//
//#include "cryptopp700\secblock.h"
//using CryptoPP::SecByteBlock;
//
//#include <cryptopp700\hex.h>
//using CryptoPP::HexEncoder;
//
//#include <cryptopp700\filters.h>
//using CryptoPP::StringSink;
//
//#include "Encryption\CryptoUtil.h"
//
//int main(int argc, char** argv)
//{
//	try
//	{
//		// RFC 5114, 1024-bit MODP Group with 160-bit Prime Order Subgroup
//		// http://tools.ietf.org/html/rfc5114#section-2.1
//		Integer p("0xB10B8F96A080E01DDE92DE5EAE5D54EC52C99FBCFB06A3C6"
//			"9A6A9DCA52D23B616073E28675A23D189838EF1E2EE652C0"
//			"13ECB4AEA906112324975C3CD49B83BFACCBDD7D90C4BD70"
//			"98488E9C219A73724EFFD6FAE5644738FAA31A4FF55BCCC0"
//			"A151AF5F0DC8B4BD45BF37DF365C1A65E68CFDA76D4DA708"
//			"DF1FB2BC2E4A4371");
//
//		Integer g("0xA4D1CBD5C3FD34126765A442EFB99905F8104DD258AC507F"
//			"D6406CFF14266D31266FEA1E5C41564B777E690F5504F213"
//			"160217B4B01B886A5E91547F9E2749F4D7FBD7D3B9A92EE1"
//			"909D0D2263F80A76A6A24C087A091F531DBF0A0169B6A28A"
//			"D662A4D18E73AFA32D779D5918D08BC8858F4DCEF97C2A24"
//			"855E6EEB22B3B2E5");
//
//		Integer q("0xF518AA8781A8DF278ABA4E7D64B7CB9D49462353");
//
//		// Schnorr Group primes are of the form p = rq + 1, p and q prime. They
//		// provide a subgroup order. In the case of 1024-bit MODP Group, the
//		// security level is 80 bits (based on the 160-bit prime order subgroup).		
//
//		// For a compare/contrast of using the maximum security level, see
//		// dh-agree.zip. Also see http://www.cryptopp.com/wiki/Diffie-Hellman
//		// and http://www.cryptopp.com/wiki/Security_level .
//
//		DH dhA, dhB;
//		AutoSeededRandomPool rndA, rndB;
//
//		dhA.AccessGroupParameters().Initialize(p, q, g);
//		dhB.AccessGroupParameters().Initialize(p, q, g);
//
//		if (!dhA.GetGroupParameters().ValidateGroup(rndA, 3) ||
//			!dhB.GetGroupParameters().ValidateGroup(rndB, 3))
//			throw runtime_error("Failed to validate prime and generator");
//
//		size_t count = 0;
//
//		p = dhA.GetGroupParameters().GetModulus();
//		q = dhA.GetGroupParameters().GetSubgroupOrder();
//		g = dhA.GetGroupParameters().GetGenerator();
//
//		// http://groups.google.com/group/sci.crypt/browse_thread/thread/7dc7eeb04a09f0ce
//		Integer v = ModularExponentiation(g, q, p);
//		if (v != Integer::One())
//			throw runtime_error("Failed to verify order of the subgroup");
//
//		//////////////////////////////////////////////////////////////
//
//		SecByteBlock privA(dhA.PrivateKeyLength());
//		SecByteBlock pubA(dhA.PublicKeyLength());
//		dhA.GenerateKeyPair(rndA, privA, pubA);
//
//		SecByteBlock privB(dhB.PrivateKeyLength());
//		SecByteBlock pubB(dhB.PublicKeyLength());
//		dhB.GenerateKeyPair(rndB, privB, pubB);
//
//		std::cout <<
//			"privA : " << privA.size() << "\t" << CryptoUtil::HexaizeString(CryptoUtil::StringnizeBlock(privA)) << std::endl <<
//			"pubA  : " << pubA.size() << "\t" << CryptoUtil::HexaizeString(CryptoUtil::StringnizeBlock(pubA)) << std::endl <<
//			"privB : " << privB.size() << "\t" << CryptoUtil::HexaizeString(CryptoUtil::StringnizeBlock(privB)) << std::endl <<
//			"pubB  : " << pubB.size() << "\t" << CryptoUtil::HexaizeString(CryptoUtil::StringnizeBlock(pubB)) << std::endl << std::endl;
//
//
//		//////////////////////////////////////////////////////////////
//
//		if (dhA.AgreedValueLength() != dhB.AgreedValueLength())
//			throw runtime_error("Shared secret size mismatch");
//
//		SecByteBlock sharedA(dhA.AgreedValueLength()), sharedB(dhB.AgreedValueLength());
//
//		if (!dhA.Agree(sharedA, privA, pubB))
//			throw runtime_error("Failed to reach shared secret (1A)");
//
//		if (!dhB.Agree(sharedB, privB, pubA))
//			throw runtime_error("Failed to reach shared secret (B)");
//
//		std::cout << std::endl << std::endl <<
//			"dhA.AgreedValueLength() : " << dhA.AgreedValueLength() << std::endl <<
//			"dhB.AgreedValueLength() : " << dhB.AgreedValueLength() << std::endl << std::endl << std::endl;
//
//
//		count = std::min(dhA.AgreedValueLength(), dhB.AgreedValueLength());
//		if (!count || 0 != memcmp(sharedA.BytePtr(), sharedB.BytePtr(), count))
//			throw runtime_error("Failed to reach shared secret");
//
//		//////////////////////////////////////////////////////////////
//
//		Integer a, b;
//
//		a.Decode(sharedA.BytePtr(), sharedA.SizeInBytes());
//		cout << "Shared secret (A): " << std::hex << a << endl;
//
//		b.Decode(sharedB.BytePtr(), sharedB.SizeInBytes());
//		cout << "Shared secret (B): " << std::hex << b << endl;
//	}
//
//	catch (const CryptoPP::Exception& e)
//	{
//		cerr << e.what() << endl;
//		return -2;
//	}
//
//	catch (const std::exception& e)
//	{
//		cerr << e.what() << endl;
//		return -1;
//	}
//
//	return 0;
//}
//
