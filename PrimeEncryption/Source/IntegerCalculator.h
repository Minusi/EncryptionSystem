#pragma once

#include "infint\InfInt.h"



class NIntegerCalculator
{
private:
	int p;
	InfInt g;
	int n;

	// 세터 함수를 생성하지 마십시오.
	int x;

public:
	NIntegerCalculator() noexcept;
	NIntegerCalculator(int InG, int InP) noexcept;
	NIntegerCalculator(int InG, int InZ, int InP, char InChar) noexcept;

	void CalculateX();

	void CalculateN();

	int GetP() const;
	InfInt GetG() const;
	int GetX() const;
	int GetN() const;

	void SetP(int const InP);
	void SetG(InfInt const InG);
	void SetN(int const InN);
	void SetX(int const InX);
};