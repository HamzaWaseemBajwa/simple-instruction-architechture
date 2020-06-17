#include "Registers.h"

IntegerReg::IntegerReg()
{
	data = 0;
}

void IntegerReg::Write(integer value)
{
	data = value;
}

void IntegerReg::Read(integer& value)
{
	value = data;
}

void InstructionReg::Write(unsigned  char* inst, int size)
{
	n_bytes_ = size;
	for (int i = 0; i < size; ++i)
	{
		bytes_[i] = inst[i];
	}
}

void InstructionReg::Read(unsigned char* inst, int& size)
{
	size = n_bytes_;
	for (int i = 0; i < size; ++i)
	{
		inst[i] = bytes_[i];
	}
}

RegisterFile::RegisterFile()
{
	reg_ = new IntegerReg[SIZE_];
	for (int i = 0; i < SIZE_; ++i)
	{
		reg_[i].Write(0);
	}
	reg_[STK_PTR_REG_].Write(MEM_SIZE);
}

RegisterFile::~RegisterFile()
{
	delete[] reg_;
}

void RegisterFile::print() const
{
	std::cout << "=== Registers =====\n";
	for (int i = 0; i < SIZE_; ++i)
	{
		std::cout << 'r' << i << "\t:\t" << reg_[i].data << '\n';
	}
	std::cout << "====================\n";
}

void RegisterFile::WriteRegister(const int rd, const int32_t data)
{
	reg_[rd].Write(data);
}

void RegisterFile::ReadRegisters(const int r1, const int r2, int32_t& dr1, int32_t& dr2) const
{
	if (r1 < SIZE_)
		reg_[r1].Read(dr1);
	if (r2 < SIZE_)
		reg_[r2].Read(dr2);
}
