#pragma once



/*	ICryptoState 클래스는 각각의 암호 알고리즘과 블록 운용 방식을
 *	나타내는 상태에 대한 인터페이스 클래스입니다.
 *  모든 암호화 상태는 이 인터페이스를 상속하여 Execute 함수를 통해서
 *	그 사양을 구현해야 합니다.
 */
class ICryptoState
{
public:
	typedef unsigned char byte;

	// 데이터에 대해 정해진 암호화 알고리즘과 블록 운용 방식으로 암,복호화를 수행합니다.
	virtual void ExecuteWithData() = 0;

	// 파일에 대해 정해진 암호화 알고리즘과 블록 운용 방식으로 암,복호화를 수행합니다.
	virtual void ExecuteWithFile() = 0;
};