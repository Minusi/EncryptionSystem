#pragma once
/*
 * 참고로 WinSock2.h는 왠지는 모르겠지만 이미 포함되어 있음.
 */

#include <combaseapi.h>
#include <memory>



//////////////////////////////////////////////////////////////
// DEFINE SECTION
//////////////////////////////////////////////////////////////

#define BUFFERSIZE 256



//////////////////////////////////////////////////////////////
// TYPEDEF SECTION
//////////////////////////////////////////////////////////////

typedef char NetBuffer[256];



//////////////////////////////////////////////////////////////
// CLASS SECTION
//////////////////////////////////////////////////////////////
interface INetDelegateComponent
{
public:
	// 네트워크를 필요로 하는지 반환합니다.
	virtual bool RequireCommunication() = 0;

	 //소켓을 받아와 통신을 수행합니다.
	virtual void Send(SOCKET InSocket, const char* InBuffer)
	{
		return;
	}

	virtual void Recv(SOCKET InSocket,const char* OutBuffer)
	{
		return;
	}
};



class NetAvailble : public INetDelegateComponent
{
	// INetworkComponent을(를) 통해 상속됨
	virtual bool RequireCommunication() override;
	virtual void Send(SOCKET InSocket, const char* InBuffer) override;
	virtual void Recv(SOCKET InSocket, const char* OutBuffer) override;
};



class NoNetAvailable : public INetDelegateComponent
{
	// INetworkComponent을(를) 통해 상속됨
	virtual bool RequireCommunication() override;
};