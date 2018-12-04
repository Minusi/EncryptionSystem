// Main.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include <iostream>

#include <WinSock2.h>

#include "source\integercalculator.h"
#include "Source\ECCDHCalculator.h"



int main()
{
	// g, x or n, p
	NIntegerCalculator nic(1,13, 23,'n');
	//nic.CalculateN();

	InfInt TestInt = 1;

	for (int i = 0; i < 100; ++i)
	{
		TestInt = TestInt * 11;
		nic.SetG(TestInt);
		std::cout << i << "번째 루프 계산중..." << std::endl;
		nic.CalculateX();
	}

/***********************************************************/

	//NECCDHCalculator DHC(11, 167, 31);
	//int x, y;
	//DHC.CalculateECCDH(2, 7, x, y);

	//std::cout << "x=" << x << "\ty=" << y << std::endl;

    return 0;
}