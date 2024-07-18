#pragma once

#include <stdbool.h>

void halfAdder(bool a, bool b, bool* carry, bool* sum);
void fullAdder(bool a, bool b, bool c, bool* carry, bool* sum);

void halfAdder(bool a, bool b, bool* carry, bool* sum)
{
	*carry = (a & b);
	*sum = (a ^ b);
}

void fullAdder(bool a, bool b, bool c, bool* carry, bool* sum)
{
	bool tmpCarry;
	bool tmpSum;
	halfAdder(a, b, &tmpCarry, &tmpSum);

	bool tmpCarry2;

	halfAdder(tmpSum, c, &tmpCarry2, sum);
	*carry = (tmpCarry | tmpCarry2);
}