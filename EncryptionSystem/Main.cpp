#include <iostream>

#include "Client/CalcClient.h"
#include "Server/CalcServer.h"


void main()
{
	bool IsServer;

	std::cout << " 서버로 사용할 것입니까? " << std::endl;
	std::cin >> IsServer;

	if (IsServer)
	{
		CalcServer calcServer = CalcServer();
	}
	else
	{
		CalcClient calcClient = CalcClient();
	}
	
	std::cout << "Application::Return" << std::endl;
	return;
}