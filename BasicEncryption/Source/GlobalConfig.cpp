#include "GlobalConfig.h"



std::string PlainText = "abcdefghijklmnopqrstuvwxyz";
std::string CipherText;
std::string Base64CipherText;
std::string HexCipherText;
std::string DecryptedText;



std::string DefaultTargetPath = "C:\\Users\\Utsuho\\Desktop\\General\\CppSpace\\NetworkEncryption\\BasicEncryption\\Data\\";
std::string DefaultTargetFile = 
	//"C:\\Users\\Utsuho\\Desktop\\General\\CppSpace\\NetworkEncryption\\BasicEncryption\\Data\\AlphabetTextFile.txt";
	"C:\\Users\\Utsuho\\Desktop\\General\\CppSpace\\NetworkEncryption\\BasicEncryption\\Data\\GrayscaleImageFile.bmp";

// 암호화 방식에 따른 파일이름 추가
std::string AppendAES = "AES_";
std::string AppendDES = "DES_";
std::string AppendTDES = "3DES_";

// 암호 블록 운용 모드에 따른 파일이름 추가
std::string AppendCBC = "CBC_";
std::string AppendECB = "ECB_";
std::string AppendCTR = "CTR_";

// 인코딩 방식에 따른 파일 이름 추가
std::string AppendNo = "Bin_";
std::string AppendBase64 = "Base64_";
std::string AppendHex = "Hex_";

// 암, 복호화에 따른 파일이름 추가
std::string CipherFileName = 
	//"CipherTextFile.txt";
	"CipherImageFile.bmp";
std::string DecryptedFileName = 
	//"DecryptedTextFile.txt";
	"DecryptedImageFile.bmp";