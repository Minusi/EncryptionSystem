#pragma once

#include <infint\InfInt.h>



class NECCDHCalculator
{
private:
	// 타원함수 1차 계수
	int coefA;
	
	// 타원함수 상수 계수
	int constB;

	// 타원함수 모듈러 상수
	int mod;

	// 비밀키A
	int secVal;

public:
	NECCDHCalculator(int InA, int InMod, int InSecVal);

	void CalculateECCDH(int InX, int InY, int& OutX, int& OutY);
	
	int PositiveModulo(int InNeg);
};