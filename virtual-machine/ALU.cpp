#include "ALU.h"

void ALU::Execute(int32_t op1, int32_t op2, int32_t& result, bool& zero, int signal)
{
	switch (signal)
	{
	case 1:	// ADD
		Adder(op1, op2, result, zero);
		break;
	case 5:	// SUB
		Adder(op1, op2, result, zero, false);
		break;
	case 2:	// AND
		LogicalUnit(op1, op2, result);
		break;
	case 6:	// OR
		LogicalUnit(op1, op2, result, false);
		break;
	case 4:	// MULT
		Multiplier(op1, op2, result);
		break;
	case 3:	// DIV
		Divider(op1, op2, result);
		break;
	}
	zero = (result == 0);
}

void ALU::Adder(int32_t op1, int32_t op2, int32_t& result, bool& zero, bool mode)
{
	if (mode)
	{
		result = op1 + op2;
	}
	else
	{
		result = op1 - op2;
	}
}

void ALU::Multiplier(int32_t op1, int32_t op2, int32_t& result)
{
	result = op1 * op2;
}

void ALU::Divider(int32_t op1, int32_t op2, int32_t& result)
{
	result = op1 / op2;
}

void ALU::LogicalUnit(int32_t op1, int32_t op2, int32_t& result, bool mode)
{
	if (mode)
	{
		result = op1 & op2;
	}
	else
	{
		result = op1 | op2;
	}
}
