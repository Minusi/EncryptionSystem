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
	//EncryptionBase의 생성자입니다.
	EncryptionBase();

protected:
	virtual void InitializeNetDelegate();
	
	// IEncryptable을(를) 통해 상속됨
	virtual void InitializeEncryption() override;
	virtual void Encrypt(std::string & OutCipherText, std::string InPlainText) override;
	virtual void Decrypt(std::string &OutPlainText, std::string InCipherText);

private:
	// 암호 모듈의 네트워크 통신을 지원하기 위한 컴포넌트입니다.
	std::unique_ptr<INetDelegateComponent> NetComponent;

	// 서버로부터 받아올 상대방의 소켓 정보입니다.
	std::shared_ptr<SOCKET> SockPtr;

	// 네트워크 컴포넌트의 허가가 있을 시 할당되는 전용 버퍼입니다.
	std::unique_ptr<NetBuffer> PrivateBuffer;
};

