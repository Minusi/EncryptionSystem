#pragma once


/*	IEncryptor는 일련의 암호화 프로세스를 실행하기 위한 인터페이스입니다.
 *	이 암호화 프로세스는 일반적으로 다음과 같은 과정으로 수행되길 기대합니다:
 *		- 초기화
 *		- 암호화
 *		- 복호화
 */
class IEncryptor
{
public:
	// 정규화된 암호화 프로세스를 수행합니다.
	virtual void Execute() = 0;
};