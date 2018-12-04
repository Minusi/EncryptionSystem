#pragma once
/*
 * 참고로 WinSock2.h는 왠지는 모르겠지만 이미 포함되어 있음.
 */

#include <combaseapi.h>
#include <memory>
#include <string>
#include <iostream>

#include "Debug\Debug.h"



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


template< unsigned int N = 1>
class NetAvailable : public INetDelegateComponent
{

public:
	NetAvailable(const SOCKET * InSocket)
		: DeleSocket{ InSocket }
	{
#ifdef CONSOLE_DEBUG
		std::cout << "----------------------------------------------------------------------" << std::endl;
		std::cout << CLASS_TAG << FUNC_TAG <<
			"Buffer Size : " << sizeof(PrivateBuffer) << std::endl;
		std::cout << "----------------------------------------------------------------------" << std::endl;
#endif // CONSOLE_DEBUG

	}

	// INetworkComponent을(를) 통해 상속됨
	bool RequireCommunication() override
	{
		return true;
	}
	


	void Send(std::string InBuffer)
	{
		// 문자열이 비어있으면 전송하지 않습니다.
		if (InBuffer.empty())
		{
			return;
		}

		// 오버플로우 발생시 전송하지 않습니다.
		if (InBuffer.length() >= sizeof(PrivateBuffer))
		{
			return;
		}

		strcpy_s(PrivateBuffer, InBuffer.c_str());
		if (send(*DeleSocket, PrivateBuffer, sizeof(PrivateBuffer), 0) == SOCKET_ERROR)
		{
			std::cout << "NETCOMP::ERROR::SEND Fail in " << __func__ << std::endl;
			exit(1);
		}


#ifdef CONSOLE_DEBUG
		CON_NEWLINE;
		std::cout << CLASS_TAG << CON_DEBUG_TAG <<
			"Buffer >> " << InBuffer.c_str() << " in " << __func__ << std::endl;
#endif // CONSOLE_DEBUG
	}



	const char* Recv()
	{
		if (recv(*DeleSocket, PrivateBuffer, sizeof(PrivateBuffer), 0) == SOCKET_ERROR)
		{
			std::cout << "NETCOMP::ERROR::RECV Fail in " << __func__ << std::endl;
			exit(1);
		}

#ifdef CONSOLE_DEBUG
		CON_NEWLINE;
		std::cout << CLASS_TAG << CON_DEBUG_TAG <<
			"Buffer >> " << PrivateBuffer << " in " << __func__ << std::endl;
#endif // CONSOLE_DEBUG

		return PrivateBuffer;
	}



	const SOCKET* GetSocket() const
	{
		return DeleSocket;
	}

	void SetSocket(SOCKET * InSocket)
	{
		DeleSocket = InSocket;
	}



private:
	// 상대방의 소켓 정보입니다.
	const SOCKET * DeleSocket;
		
	// 네트워크 컴포넌트의 허가가 있을 시 할당되는 전용 버퍼입니다.
	char PrivateBuffer[BUFFERSIZE * N];
};





class NoNetAvailable : public INetDelegateComponent
{
public:
	// INetworkComponent을(를) 통해 상속됨
	virtual bool RequireCommunication() override;
};