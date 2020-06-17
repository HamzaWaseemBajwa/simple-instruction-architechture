#pragma once
#include <cstdint>
class ALU
{
public:
	void Execute(int32_t op1, int32_t op2, int32_t& result, bool& zero, int signal);

private:
	void Adder(int32_t op1, int32_t op2, int32_t& result, bool& zero, bool mode = true);
	void Multiplier(int32_t op1, int32_t op2, int32_t& result);
	void Divider(int32_t op1, int32_t op2, int32_t& result);
	void LogicalUnit(int32_t op1, int32_t op2, int32_t& result, bool mode = true);
};

