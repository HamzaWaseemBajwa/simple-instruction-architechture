#include "Memory.h"
#include "Memory.h"

Memory::Memory(int max_size)
{
	MAX_SIZE_ = max_size;
	bytes_ = new unsigned char[MAX_SIZE_];
	for (int i = 0; i < MAX_SIZE_; ++i)
	{
		bytes_[i] = '\0';
	}
}

Memory::~Memory()
{
	delete[] bytes_;
}

void Memory::Write(const int address, const unsigned char* data, const int size)
{
	for (int i = 0; i < size; ++i)
	{
		bytes_[address + i] = data[i];
	}
}

void Memory::Read(const int address, unsigned char* data, const int size) const
{
	for (int i = 0; i < size; ++i)
	{
		data[i] = bytes_[address + i];
	}
}

void Memory::print() const
{
	std::cout << "========= Memory ===========\n";
	for (int i = 0; i < MAX_SIZE_; ++i)
	{
		std::cout << std::setfill('0') << std::setw(2) << std::hex << (0xff & (unsigned int)bytes_[i]) << ' ';
		if (i % 10 == 9)
			std::cout << '\n';
	}
	std::cout << "\n===========================\n";
	std::cout << std::dec;
}
