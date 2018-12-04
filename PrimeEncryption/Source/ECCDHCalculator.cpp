#include "ECCDHCalculator.h"

#include <cmath>
#include <iostream>
#include <algorithm>



NECCDHCalculator::NECCDHCalculator(int InA, int InMod, int InSecVal) 
	: coefA(InA), mod(InMod), secVal(InSecVal)
{
}



void NECCDHCalculator::CalculateECCDH(int InX, int InY, int& OutX, int& OutY)
{
	int TempY = static_cast<int>(std::pow(InY, 2)) % mod;
	int TempX = static_cast<int>(std::pow(InX, 3)) + (coefA * InX) % mod;

	// 타원함수에서 상수계수 b를 구하는 과정
	for (int i = 0; i < mod; ++i)
	{
		//디버그 코드
		//std::cout << "i=" << i << std::endl;
		//std::cout << (TempX + i) % mod << std::endl;
		//std::cout << TempY << std::endl << std::endl;

		if ((TempX + i) % mod == TempY)
		{
			constB = i;

			std::cout << "constB::const B value is "<< i << std::endl << std::endl;
			break;
		}
	}

	int x1 = InX,	y1 = InY;
	int x2 = InX,	y2 = InY;
	int x3 = 0,	y3 = 0;

	int m;


	for (int i = 2; i <= secVal; ++i)
	{
		std::cout << i - 1 << "번째 루프 : \t";
		if ((x1 == x2) && (y1 == y2))
		{
			std::cout << "(x1,y1) == (x2,y2)" << std::endl;
			// (3x^2 + a)
			int tempR1 = (3 * x1 * x1) + coefA;

			// 유효하지 않은 초기값 주입
			int tempR2 = 0;

			// (2y)^-1
			for (int k = 1; ; ++k)
			{
				if (((y1 * 2 * k) % mod) == 1)
				{
					tempR2 = k;
					break;
				}
			}
			// m = (3x^2 + a)(2y^-1) mod p
			m = (tempR1 * tempR2) % mod;
		}
		else
		{
			std::cout << "(x1,y1) != (x2,y2)" << std::endl;
			// (y2 - y1)
			int tempR1 = (y2 - y1);

			// 유효하지 않은 초기값 주입
			int tempR2 = 0;

			// (x2 - x1)^-1
			for (int k = 1; ; ++k)
			{
				if ((((x2 - x1) * k) % mod) == 1)
				{
					tempR2 = k;
					break;
				}
			}

			// m = (y2 - y1)(x2 - x1)^-1 mod p
			m = PositiveModulo(tempR1 * tempR2) % mod;			
		}
		std::cout << "m  =  " << m << " \t";

		int subx3 = ((m*m) - x1 - x2);
		x3 = PositiveModulo(subx3) % mod;
		std::cout << "x3 =  " << x3 << " \t";
		
		int suby3 = ((m * (x1 - x3)) - y1);
		y3 = PositiveModulo(suby3) % mod;
		std::cout << "y3 =  " << y3 << std::endl << std::endl;

		// 결과값 x3, y3에 대한 유효성 검사
		if (x3 < 0)
		{
			std::cout << "ERROR::Negative X occured in" << i << std::endl;
		}
		else if (y3 < 0)
		{
			std::cout << "ERROR::Negative Y occured in" << i << std::endl;

		}

		x2 = x3;
		y2 = y3;
	}

	OutX = x3;
	OutY = y3;
}

int NECCDHCalculator::PositiveModulo(int InNeg)
{
	int ConvertPos;

	if (InNeg < 0)
	{
		for (int a = 1; ; ++a)
		{
			if (std::abs(InNeg) < (mod * a))
			{
				//std::cout << std::abs(InNeg) << std::endl;
				//std::cout << (mod * a) << std::endl;
				//std::cout << "Modulo exit! " << a << std::endl;
				ConvertPos = (mod * a) + InNeg;
				break;
			}
		}
	}
	else if (InNeg > 0)
	{
		//std::cout << "InNeg is not Neg!" << std::endl;
		ConvertPos = InNeg;
	}

	return ConvertPos;
}
