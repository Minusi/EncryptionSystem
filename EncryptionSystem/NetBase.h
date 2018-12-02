#pragma once
/*
 * 참고로 WinSock2.h는 왠지는 모르겠지만 이미 포함되어 있음.
 */

#include <combaseapi.h>
#include <memory>
#include <string>



//////////////////////////////////////////////////////////////
// DEFINE SECTION
//////////////////////////////////////////////////////////////

constexpr int BUFFERSIZE = 256;



//////////////////////////////////////////////////////////////
// TYPEDEF SECTION
//////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////
// CLASS SECTION
//////////////////////////////////////////////////////////////

/*
 * INetDelegateComponent는 소켓을 가지지 않아 네트워크 통신이
 * 불가능한 클래스에 대해 네트워크 소켓과 버퍼를 전달받아
 * 네트워크 통신을 가능하게 하는 인터페이스입니다.
 *
 *
 */
interface INetDelegateComponent
{
public:
	// 네트워크를 필요로 하는지 반환합니다.
	virtual bool RequireCommunication() = 0;

	 //소켓을 받아와 통신을 수행합니다.
	virtual void Send(std::string InBuffer)
	{
		return;
	}

	virtual const char* Recv()
	{
		return nullptr;
	}

	virtual const SOCKET* GetSocket() const
	{
		return nullptr;
	}

	virtual void SetSocket(SOCKET* InSocket)
	{
		return;
	}
};



class NetAvailable : public INetDelegateComponent
{

public:
	NetAvailable(const SOCKET* InSocket);

	// INetworkComponent을(를) 통해 상속됨
	virtual bool RequireCommunication() override;
	virtual void Send(std::string InBuffer);
	virtual const char* Recv();



	virtual const SOCKET* GetSocket() const;		// SOCKET 액세서 (변경 불가)
	virtual void SetSocket(SOCKET* InSocket);		// SOCKET 뮤테이터

private:
	// 상대방의 소켓 정보입니다.
	const SOCKET * DeleSocket;
		
	// 네트워크 컴포넌트의 허가가 있을 시 할당되는 전용 버퍼입니다.
	char PrivateBuffer[BUFFERSIZE];
};



class NoNetAvailable : public INetDelegateComponent
{
public:
	// INetworkComponent을(를) 통해 상속됨
	virtual bool RequireCommunication() override;
};