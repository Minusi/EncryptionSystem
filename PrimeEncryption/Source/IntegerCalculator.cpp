#include "IntegerCalculator.h"

#include <iostream>
#include <fstream>
#include <cmath>

#include <time.h>


NIntegerCalculator::NIntegerCalculator() noexcept : g(-1), n(-1), p(-1), x(-1)
{
}

NIntegerCalculator::NIntegerCalculator(int InG, int InP) noexcept : x(-1), n(-1)
{
	SetG(InG);
	SetP(InP);
}

NIntegerCalculator::NIntegerCalculator(int InG, int InZ, int InP, char InChar) noexcept
{

	SetG(InG);
	SetP(InP);

	if (InChar == 'n')
	{
		SetN(InZ);
		std::cout << "g=" << g << "\tn=" << n << "\tp=" << p << std::endl << std::endl;
	}
	else if (InChar == 'x')
	{
		SetX(InZ);
		std::cout << "x=" << x << "\tg=" << g << "\tp=" << p << std::endl << std::endl;
	}
	else
	{
		std::cout << "ERROR::" << InChar << "is not valid argument" << std::endl;
	}
}


int externi = 1;
void NIntegerCalculator::CalculateX()
{
	if (n < 0)
	{
		std::cout << "ERROR::N value is not valid" << std::endl;
		return;
	}

	std::ofstream fout;
	fout.open("C:\\Users\\Utsuho\\Desktop\\General\\CppSpace\\NetworkEncryption\\Binary\\Count.txt", std::ios::app);

	if (fout.is_open() == false)
	{
		std::cout << "ERROR::File cannot open" << std::endl;
		return;
	}

	clock_t begin, end;

	begin = clock();

	InfInt gPowN = 1;
	InfInt temp = g;

	for (int i = 1; i <= n; ++i)
	{
		gPowN = gPowN * temp;
	}
	x = (gPowN % p).toInt();
	
	end = clock();

	std::cout << "CALCX:: " << g << "^" << n << " mod " << p << " = " << x << std::endl;
	std::cout << "CALCX:: X value = " << x << std::endl << std::endl;

	fout << externi << "\t" << ((double)(end - begin) / (double)CLOCKS_PER_SEC) << std::endl;
	externi++;
}

void NIntegerCalculator::CalculateN()
{
	if (x < 0)
	{
		std::cout << "ERROR::X value is not valid" << std::endl;
		return;
	}

	std::ofstream fout;
	fout.open("C:\\Users\\Utsuho\\Desktop\\General\\CppSpace\\NetworkEncryption\\Binary\\Count.txt");

	if (fout.is_open() == false)
	{
		std::cout << "ERROR::File cannot open" << std::endl;
		return;
	}

	clock_t begin, end, success;

	n = 1;

	begin = clock();

	InfInt base = g;
	InfInt temp = 1;
	while (n < 1000)
	{
		temp = temp * base;

		std::cout << "n=" << n << "\t";
		std::cout << temp << " % " << p << " == " << (temp % p) << std::endl;

		end = clock();
		fout << n << "\t" << ((double)(end - begin) / (double)CLOCKS_PER_SEC) << std::endl;

		if ( (temp % p) == x)
		{
			success = clock();

			std::cout << "CALCX::N is " << n << std::endl;
			std::cout << "CALCX::Time delays " << \
				((double)(success - begin) / (double)CLOCKS_PER_SEC) << " seconds" << std::endl;

			return;
		}
		++n;
	}

	std::cout << "CALCX::Root value cannot found in equation" << std::endl;
	return;
}



int NIntegerCalculator::GetP() const
{
	return p;
}

InfInt NIntegerCalculator::GetG() const
{
	return g;
}

int NIntegerCalculator::GetX() const
{
	return x;
}

int NIntegerCalculator::GetN() const
{
	return n;
}

void NIntegerCalculator::SetP(int const InP)
{
	if (p <= 0)
	{
		p = 1;
	}

	p = InP;
}

void NIntegerCalculator::SetG(InfInt const InG)
{
	if (g <= 0)
	{
		g = 1;
	}

	g = InG;
}

void NIntegerCalculator::SetN(int const InN)
{
	if (n <= 0)
	{
		n = 1;
	}
	n = InN;
}

void NIntegerCalculator::SetX(int const InX)
{
	if (x <= 0)
	{
		x = 1;
	}
	x = InX;
}
