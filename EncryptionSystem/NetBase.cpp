#include "NetBase.h"

#include <iostream>

#include "Debug\Debug.h"



NetAvailable::NetAvailable(const SOCKET * InSocket)
	: DeleSocket{ InSocket }
{
}

bool NetAvailable::RequireCommunication()
{
	return true;
}

void NetAvailable::Send(std::string InBuffer)
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
	std::cout << CLASS_TAG << CON_DEBUG_TAG <<
		"Buffer >> " << InBuffer.c_str() << " in " << __func__ << std::endl;
#endif // CONSOLE_DEBUG
}

const char* NetAvailable::Recv()
{
	if (recv(*DeleSocket, PrivateBuffer, sizeof(PrivateBuffer), 0) == SOCKET_ERROR)
	{
		std::cout << "NETCOMP::ERROR::RECV Fail in " << __func__ << std::endl;
		exit(1);
	}

#ifdef CONSOLE_DEBUG
	std::cout << CLASS_TAG << CON_DEBUG_TAG <<
		"Buffer >> " << PrivateBuffer << " in " << __func__ << std::endl;
#endif // CONSOLE_DEBUG

	return PrivateBuffer;
}

const SOCKET* NetAvailable::GetSocket() const
{
	return DeleSocket;
}

void NetAvailable::SetSocket(SOCKET * InSocket)
{
	DeleSocket = InSocket;
}

bool NoNetAvailable::RequireCommunication()
{
	return false;
}

