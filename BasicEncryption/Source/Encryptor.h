#pragma once

#include <string>

#include "IEncryptor.h"
#include "ICryptoState.h"



class Encryptor : public IEncryptor
{
public:
	// 기본적으로 AES-CBC 기반의 암호 프로세스를 수행할 생성자입니다.
	Encryptor();

	// 사용자가 정의한 기반의 암호 프로세스를 수행할 생성자입니다.
	Encryptor(std::string InAlgorithm, std::string InBlockMode, bool InTargetIsData = true);

	// 복사 생성자입니다.
	Encryptor(const Encryptor& InEncryptor) = delete;

	virtual ~Encryptor();
		
public:
	// 암호 기계에 암호 상태를 등록합니다.
	void RegisterConfiguration(std::string InAlgorithm, std::string InBlockMode, bool InTargetIsData);

	// IEncryptor를 통해 상속됨
	virtual void Execute() override;

private:
	// 암호 알고리즘과 블록 암호 운용 모드에 대한 상태
	ICryptoState * CryptoState;

	// 암호 프로세스를 진행할 타겟이 Data인지 File인지 여부
	bool bTargetIsData;
};