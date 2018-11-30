#include "NetBase.h"

#include <iostream>


bool NetAvailble::RequireCommunication()
{
	return true;
}

void NetAvailble::Send(SOCKET InSocket, const char* InBuffer)
{
	if (send(InSocket, InBuffer, sizeof(*InBuffer), 0) == SOCKET_ERROR)
	{
		std::cout << "NETCOMP::ERROR::SEND Fail in " << __func__ << std::endl;
		exit(1);
	}
}

void NetAvailble::Recv(SOCKET InSocket, const char* OutBuffer)
{
	std::cout << "NETCOMP::ERROR::RECV Fail in " << __func__ << std::endl;
	exit(1);
}



bool NoNetAvailable::RequireCommunication()
{
	return false;
}
