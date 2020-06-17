#include "globals.h"
#include <iostream>

#pragma once
struct IntegerReg
{
	integer data;

	IntegerReg();

	void Write(integer value);

	void Read(integer& value);
};

#pragma once
struct InstructionReg
{
	char bytes_[4];
	int n_bytes_;

	void Write(unsigned char* inst, int size);

	void Read(unsigned char* inst, int& size);

	void operator=(const InstructionReg& rhs)
	{
		this->n_bytes_ = rhs.n_bytes_;
		for (int i = 0; i < rhs.n_bytes_; ++i)
		{
			this->bytes_[i] = rhs.bytes_[i];
		}
	}
};

#pragma once
class RegisterFile
{
public:
	RegisterFile();

	~RegisterFile();

	void print() const;

	void WriteRegister(const int rd, const int32_t data);

	void ReadRegisters(const int r1, const int r2, int32_t& dr1, int32_t& dr2) const;

private:
	IntegerReg* reg_;
	const int SIZE_ = REG_FILE_SIZE;
	const int STK_PTR_REG_ = STK_PTR_REG;
};