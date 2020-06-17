#pragma once
#include <iostream>
#include <iomanip>
class Memory
{
public:
	Memory(int max_size);

	~Memory();

	void Write(const int address, const unsigned char* data, const int size);

	void Read(const int address, unsigned char* data, const int size) const;

	void print() const;
private:
	unsigned char* bytes_;
	int MAX_SIZE_;
};

