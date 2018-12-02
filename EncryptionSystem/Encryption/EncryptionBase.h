#pragma once

#include <memory>

#include "IEncryptable.h"
#include "NetBase.h"



/*
 * EncryptionBase는 암호 모듈의 기본이 되는 클래스입니다.
 * 네트워크 전송 기능과 암호화 및 복호화 기능을 구현할 수 있습니다.
 * 
 * 이 클래스 자체로는 아무런 기능을 가지지 않으므로 이 클래스를 인스턴스화
 * 하지 않는 것이 좋습니다. 특히 암호화/복호화 기능은 제공하지 않습니다.
 * 모든 암호 모듈은 이 클래스를 상속받아 작성하여야 합니다.
 */
class EncryptionBase : public IEncryptable
{
public:
	/*
	* 통신을 필요로 할 때 통신 순서를 제어하기 위한 플래그입니다.
	* NONE - 기본적으로 네트워크 모드를 지정하지 않는다는 것을 의미합니다.
	* HOST - 네트워크에서 무언가를 주도적으로 진행하겠다는 것을 의미합니다.
	*			비유를 하자면, 조별 과제에서 팀장과 같습니다. 대개 조장은
	*			같은 자료조사를 하더라도 주도적으로 정보를 수집하고 결집하는
	*			역할을 수행한다는 점을 떠올리면 될 것 같습니다.
	* GUEST - 네트워크에서 무언가를 수동적으로 진행하겠다는 것을 의미합니다.
	*			비유를 하자면, 조별 과제에서 조원과 같습니다. 조원은 조장이
	*			세운 계획을 그대로 이행한다는 점을 떠올리면 될 것 같습니다.
	*
	* 주의할 점은 두 네트워크 객체 간 통신에서 둘다 HOST이거나 둘다 GUEST여서는
	* 안된다는 점입니다. 이는 MASTER/SLAVE 구조를 떠올리시면 될 것 같습니다.
	*/
	enum class NetMode
	{
		NONE, HOST, GUSET
	};



public:
	// EncryptionBase의 생성자입니다.
	// 암호 모듈 작동에 필요한 사전 준비 사항들을 초기화합니다.
	// 네트워크 모듈과 암호 모듈에 대한 초기화가 이루어져야 합니다.
	// EncryptionBase 클래스에서는 네트워크에 대한 초기화만 이루어지고
	// 암호 모듈은 초기화를 하지 않습니다.
	EncryptionBase(INetDelegateComponent* InNetDeleComp = nullptr);

	// IEncryptable을(를) 통해 상속됨
	virtual void Encrypt(std::string& OutCipherText, std::string InPlainText) = 0;
	virtual void Decrypt(std::string& OutPlainText, std::string InCipherText) = 0;

protected:
	virtual void InitializeEncryption() = 0;

protected:
	// 암호 모듈의 네트워크 통신을 지원하기 위한 컴포넌트입니다.
	std::unique_ptr<INetDelegateComponent> NetComponent;
};

