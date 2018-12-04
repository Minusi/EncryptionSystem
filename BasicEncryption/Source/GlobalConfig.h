#pragma once

#include <string>
#include <files.h>



extern std::string PlainText;
extern std::string CipherText;
extern std::string Base64CipherText;
extern std::string HexCipherText;
extern std::string DecryptedText;



extern std::string DefaultTargetPath;
extern std::string DefaultTargetFile;

// 암호화 방식에 따른 파일이름 추가
extern std::string AppendAES;
extern std::string AppendDES;
extern std::string AppendTDES;

// 암호 블록 운용 모드에 따른 파일이름 추가
extern std::string AppendCBC;
extern std::string AppendECB;
extern std::string AppendCTR;

// 인코딩 방식에 따른 파일 이름 추가
extern std::string AppendNo;
extern std::string AppendBase64;
extern std::string AppendHex;

// 암, 복호화에 따른 파일이름 추가
extern std::string CipherFileName;
extern std::string DecryptedFileName;