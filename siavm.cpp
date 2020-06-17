#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cstring>

#include "ALU.h"
#include "Memory.h"
#include "Registers.h"

// CPU Helper Functions

void SetControlSignals();
void Int2Char(integer, unsigned char[]);
void Char2Int(unsigned char[], integer&);
void ImmediateGenerator();
void LoadAssembly(const char[], Memory&);

int programCounter;
int nextInstruction;

struct FE_DE {
	bool enable = false;
	InstructionReg instr;
	int programCounter = 0;
	int nextInstruction = 0;
	unsigned int opcode = 0;
};

struct DE_EX {
	bool enable = false;
	InstructionReg instr;
	int programCounter = 0;
	int nextInstruction = 0;
	unsigned int opcode = 0;
	bool branch = false;
	bool branchType = false;
	bool branchTaken = false;
	bool memRead = false;
	bool memWrite = false;
	bool memToReg = false;
	bool aluSrc = false;
	bool regWrite = false;
	bool halt = false;
	bool stk = false;
	bool interrupt = false;
	bool jump = false;
	bool call = false;
	int aluSignal = 0;
	int stkSignal = 0;
	int interruptCode = 0;
	integer immediateData = 0;
	IntegerReg op1;
	IntegerReg op2;
	IntegerReg memWriteReg;
	int writeReg = 0;
};

struct EX_ST {
	bool enable = false;
	InstructionReg instr;
	int programCounter = 0;
	int nextInstruction = 0;
	unsigned int opcode = 0;
	bool branch = false;
	bool branchType = false;
	bool branchTaken = false;
	bool memRead = false;
	bool memWrite = false;
	bool memToReg = false;
	bool aluSrc = false;
	bool regWrite = false;
	bool halt = false;
	bool stk = false;
	bool interrupt = false;
	bool jump = false;
	bool call = false;
	int aluSignal = 0;
	int stkSignal = 0;
	int interruptCode = 0;
	integer immediateData = 0;
	IntegerReg op1;
	IntegerReg op2;
	IntegerReg aluResult;
	bool aluZero = false;
	int memAddress = 0;
	IntegerReg memWriteReg;
	IntegerReg memReadReg;
	IntegerReg regWriteData;
	int branchTarget = 0;
	int jumpTarget = 0;
	int writeReg = 0;
};

FE_DE fe_de;
DE_EX de_ex;
EX_ST ex_st;

void forward_1_2() {
	de_ex.programCounter = fe_de.programCounter;
	de_ex.nextInstruction = fe_de.nextInstruction;
	de_ex.instr = fe_de.instr;
	de_ex.opcode = fe_de.opcode;
	de_ex.enable = true;
	fe_de.enable = false;
}

void forward_2_3() {
	ex_st.programCounter = de_ex.programCounter;
	ex_st.nextInstruction = de_ex.nextInstruction;
	ex_st.writeReg = de_ex.writeReg;
	ex_st.aluSignal = de_ex.aluSignal;
	ex_st.branch = de_ex.branch;
	ex_st.branchType = de_ex.branchType;
	ex_st.memRead = de_ex.memRead;
	ex_st.memWrite = de_ex.memWrite;
	ex_st.memToReg = de_ex.memToReg;
	ex_st.aluSrc = de_ex.aluSrc;
	ex_st.regWrite = de_ex.regWrite;
	ex_st.halt = de_ex.halt;
	ex_st.stk = de_ex.stk;
	ex_st.stkSignal = de_ex.stkSignal;
	ex_st.interrupt = de_ex.interrupt;
	ex_st.jump = de_ex.jump;
	ex_st.call = de_ex.call;
	ex_st.immediateData = de_ex.immediateData;
	ex_st.enable = true;
	de_ex.enable = false;
}

// CPU Hardware

RegisterFile registers;
Memory systemMemory(MEM_SIZE);
ALU mainALU;

// CPU Stage Functions

void Fetch();
void Decode();
void Execute();
void Store();

int main(int argc, char* argv[])
{

	nextInstruction = 0;
	LoadAssembly(argv[1], systemMemory);
	fe_de.enable = false;
	de_ex.enable = false;
	ex_st.enable = false;
	while (true)
	{
		Fetch();
		Execute();
		Decode();
		if (de_ex.halt)
			break;
		Store();
	}
}

// CPU Main Functions

void Fetch()
{
	unsigned char buffer[4];
	unsigned char c_opcode;
	programCounter = nextInstruction;
	systemMemory.Read(programCounter, &c_opcode, 1);
	fe_de.opcode = (unsigned int)c_opcode;
	fe_de.opcode = fe_de.opcode >> 4;
	int instructionBytes = 2;
	if (fe_de.opcode >= 10 && fe_de.opcode <= 13)
	{
		instructionBytes = 4;
	}
	systemMemory.Read(programCounter, buffer, instructionBytes);
	nextInstruction = programCounter + instructionBytes;
	fe_de.programCounter = programCounter;
	fe_de.instr.Write(buffer, instructionBytes);
	fe_de.enable = true;
}

void Decode()
{
	if (fe_de.enable == false)
	{
		return;
	}
	unsigned char buffer[4];
	int bytes = 0;
	fe_de.instr.Read(buffer, bytes);
	SetControlSignals();
	if (de_ex.halt)
		return;
	int readRegister1 = (int)(buffer[0] & 0x0F);
	int readRegister2 = (int)(buffer[1] >> 4);
	int writeRegister = (int)(buffer[1] & 0x0F);
	if (fe_de.opcode == 9)
		writeRegister = readRegister1;
	if (de_ex.stk)
	{
		int temp = readRegister1;
		if (de_ex.stkSignal == 2)
			writeRegister = readRegister1;
		readRegister1 = STK_PTR_REG;
		readRegister2 = temp;
	}
	if (de_ex.memRead || de_ex.memWrite)
	{
		int temp = readRegister1;
		readRegister1 = readRegister2;
		if (de_ex.memRead)
		{
			writeRegister = temp;
		}
		if (de_ex.memWrite)
		{
			readRegister2 = temp;
		}
	}
	if (de_ex.call)
	{
		readRegister1 = STK_PTR_REG;
	}
	ImmediateGenerator();
	integer regRead1;
	integer regRead2;
	registers.ReadRegisters(readRegister1, readRegister2, regRead1, regRead2);
	if (de_ex.stk && de_ex.stkSignal == 0)
	{
		int memAddress = regRead1;
		unsigned char data[4];
		systemMemory.Read(memAddress, data, 4);
		integer value;
		Char2Int(data, value);
		memAddress += 4;
		registers.WriteRegister(STK_PTR_REG, memAddress);
		nextInstruction = value;
		fe_de.enable = false;
		de_ex.enable = false;
		ex_st.enable = false;
		return;
	}
	de_ex.op1.Write(regRead1);
	de_ex.op2.Write(de_ex.aluSrc ? de_ex.immediateData : regRead2);
	de_ex.memWriteReg.Write(regRead2);
	de_ex.writeReg = writeRegister;
	forward_1_2();
}

void Execute()
{
	if (de_ex.enable == false)
		return;
	int memAddress;
	if (de_ex.interrupt)
	{
		switch (de_ex.interruptCode)
		{
		case 0:
			registers.print();
			break;
		case 1:
			systemMemory.print();
			break;
		default:
			break;
		}
		forward_2_3();
		return;
	}
	if (de_ex.stk)
	{
		de_ex.op1.Read(memAddress);
		switch (de_ex.stkSignal)
		{
		case 0:
		case 2:
		{
			unsigned char data[4];
			systemMemory.Read(memAddress, data, 4);
			integer value;
			Char2Int(data, value);
			ex_st.memReadReg.Write(value);
			ex_st.op1.Write(memAddress);
			break;
		}
		case 1:
		{
			unsigned char data[4];
			integer value;
			de_ex.memWriteReg.Read(value);
			Int2Char(value, data);
			systemMemory.Write(memAddress - 4, data, 4);
			ex_st.op1.Write(memAddress);
			break;
		}
		}
		forward_2_3();
		return;
	}
	if (de_ex.jump)
	{
		ex_st.jumpTarget = de_ex.immediateData << 1;
		if (de_ex.call)
		{
			de_ex.op1.Read(memAddress);
			unsigned char data[4];
			Int2Char(de_ex.programCounter + 4, data);
			systemMemory.Write(memAddress - 4, data, 4);
			memAddress -= 4;
			registers.WriteRegister(STK_PTR_REG, memAddress);
			fe_de.enable = false;
			de_ex.enable = false;
			ex_st.enable = false;
		}
		nextInstruction = ex_st.jumpTarget;
		return;
	}
	integer aluOp1;
	integer aluOp2;
	integer aluOut;
	de_ex.op1.Read(aluOp1);
	de_ex.op2.Read(aluOp2);
	ex_st.op1.Write(aluOp1);
	ex_st.op2.Write(aluOp2);
	mainALU.Execute(aluOp1, aluOp2, aluOut, ex_st.aluZero, de_ex.aluSignal);
	ex_st.aluResult.Write(aluOut);
	if (de_ex.memWrite)
	{
		ex_st.aluResult.Read(memAddress);
		unsigned char writeData[4];
		integer data;
		de_ex.memWriteReg.Read(data);
		Int2Char(data, writeData);
		systemMemory.Write(memAddress, writeData, 4);
		forward_2_3();
		return;
	}
	if (de_ex.memRead)
	{
		ex_st.aluResult.Read(memAddress);
		unsigned char readData[4];
		systemMemory.Read(memAddress, readData, 4);
		integer data;
		Char2Int(readData, data);
		ex_st.memReadReg.Write(data);
		forward_2_3();
		return;
	}
	if (de_ex.branch)
	{
		ex_st.branchTarget = de_ex.programCounter + (de_ex.immediateData << 1);
		if (de_ex.branchType)
		{
			if (ex_st.aluResult.data < 0)
				ex_st.branchTaken = true;
		}
		else
		{
			if (ex_st.aluZero)
				ex_st.branchTaken = true;
		}
		if (ex_st.branchTaken)
		{
			fe_de.enable = false;
			de_ex.enable = false;
			ex_st.enable = false;
			nextInstruction = ex_st.branchTarget;
		}
		return;
	}
	forward_2_3();
}

void Store()
{
	if (ex_st.enable == false)
		return;
	IntegerReg regWriteData;
	if (ex_st.stk)
	{
		int stkAddress;
		ex_st.op1.Read(stkAddress);
		switch (ex_st.stkSignal)
		{
		case 0:
			stkAddress += 4;
			ex_st.memReadReg.Read(ex_st.nextInstruction);
			registers.WriteRegister(STK_PTR_REG, stkAddress);
			nextInstruction = ex_st.nextInstruction;
			return;
		case 1:
			stkAddress -= 4;
			registers.WriteRegister(STK_PTR_REG, stkAddress);
			break;
		case 2:
			integer data;
			ex_st.memReadReg.Read(data);
			registers.WriteRegister(ex_st.writeReg, data);
			stkAddress += 4;
			registers.WriteRegister(STK_PTR_REG, stkAddress);
			break;
		}
	}
	if (ex_st.jump)
	{
		ex_st.nextInstruction = ex_st.jumpTarget;

	}
	if (ex_st.memToReg)
	{
		integer writeData;
		ex_st.memReadReg.Read(writeData);
		regWriteData.Write(writeData);
	}
	else
	{
		integer writeData;
		ex_st.aluResult.Read(writeData);
		regWriteData.Write(writeData);
	}
	if (ex_st.regWrite)
	{
		integer writeData;
		regWriteData.Read(writeData);
		registers.WriteRegister(ex_st.writeReg, writeData);
	}
	ex_st.enable = false;
}

void SetControlSignals()
{
	de_ex.aluSignal = 1;

	de_ex.branch = false;
	de_ex.branchType = false;
	de_ex.branchTaken = false;
	de_ex.memRead = false;
	de_ex.memWrite = false;
	de_ex.memToReg = false;
	de_ex.aluSrc = false;
	de_ex.regWrite = false;
	de_ex.halt = false;
	de_ex.stk = false;
	de_ex.interrupt = false;
	de_ex.jump = false;
	de_ex.call = false;

	char* instructionBytes = fe_de.instr.bytes_;

	if (fe_de.opcode == 0)
	{
		de_ex.halt = true;
		return;
	}
	if (fe_de.opcode >= 1 && fe_de.opcode <= 6)	// 3R
	{
		de_ex.aluSignal = (int)(instructionBytes[0] >> 4);
		de_ex.regWrite = true;
		return;
	}
	if (fe_de.opcode == 7)	// STK
	{
		de_ex.stk = true;
		de_ex.stkSignal = ((instructionBytes[1] >> 6) & 0x03);
		return;
	}
	if (fe_de.opcode == 8)	// INT
	{
		de_ex.interrupt = true;
		de_ex.interruptCode = (int)instructionBytes[1];
		return;
	}
	if (fe_de.opcode == 9)	// ADDI
	{
		de_ex.aluSignal = 1;
		de_ex.regWrite = true;
		de_ex.aluSrc = true;
		return;
	}
	if (fe_de.opcode == 10 || fe_de.opcode == 11)	// BR
	{
		de_ex.aluSignal = 5;
		de_ex.branch = true;
		de_ex.aluSrc = false;
		if (fe_de.opcode == 11)
			de_ex.branchType = true;
		return;
	}
	if (fe_de.opcode == 12 || fe_de.opcode == 13)	// JMP
	{
		de_ex.jump = true;
		if (fe_de.opcode == 13)
			de_ex.call = true;
		return;
	}
	if (fe_de.opcode == 14)	// LOAD
	{
		de_ex.aluSignal = 1;
		de_ex.aluSrc = true;
		de_ex.memToReg = true;
		de_ex.regWrite = true;
		de_ex.memRead = true;
	}
	if (fe_de.opcode == 15)	// STORE
	{
		de_ex.aluSignal = 1;
		de_ex.aluSrc = true;
		de_ex.memWrite = true;
	}
}

void ImmediateGenerator()
{
	bool negative = false;
	de_ex.immediateData = 0;
	if (fe_de.opcode < 9)
		return;
	unsigned char buffer[4];
	int bytes = 0;
	fe_de.instr.Read(buffer, bytes);
	if (fe_de.opcode == 9)	// ADDI
	{
		char s_buffer = buffer[1];
		de_ex.immediateData = (integer)s_buffer;
		return;
	}
	if (fe_de.opcode == 10 || fe_de.opcode == 11)	// BR
	{
		integer temp = buffer[1] & 0x01;
		if (temp > 8)
			negative = true;
		temp = (integer)((buffer[1] & 0x01) << 16);
		temp += (integer)(buffer[2] << 8);
		temp += (integer)(buffer[3]);
		if (negative)
			de_ex.immediateData = -temp;
		else
			de_ex.immediateData = temp;
		return;
	}
	if (fe_de.opcode == 12 || fe_de.opcode == 13) // JMP
	{
		de_ex.immediateData = (integer)((buffer[0] & 0x01) << 24);
		de_ex.immediateData += (integer)(buffer[1] << 16);
		de_ex.immediateData += (integer)(buffer[2] << 8);
		de_ex.immediateData += (integer)(buffer[3]);
		return;
	}
	if (fe_de.opcode == 14 || fe_de.opcode == 15)	// ST LD
	{
		integer temp = (integer)(buffer[1] & 0x03);
		de_ex.immediateData = temp << 1;
	}
}

void LoadAssembly(const char filename[], Memory& memory)
{
	int mem_ptr = 0;
	std::ifstream assembly_file(filename, std::ios::in | std::ios::binary);
	if (!assembly_file.is_open())
	{
		std::cerr << "Could not open asm file\n";
		exit(1);
	}

	char buffer[2];
	while (assembly_file.read(buffer, 2))
	{
		unsigned char u_buff[2];
		for (int i = 0; i < 2; ++i)
			u_buff[i] = (unsigned)buffer[i];
		memory.Write(mem_ptr, u_buff, 2);
		mem_ptr += 2;
	}
	assembly_file.close();
}

void Int2Char(integer data, unsigned char bytes[])
{
	int BYTES = sizeof(integer);
	for (int i = 0; i < BYTES; ++i)
	{
		bytes[i] = '\0';
	}
	for (int i = 0; i < BYTES; ++i)
	{
		bytes[3 - i] = (data >> (i * 8));
	}
}

void Char2Int(unsigned char bytes[], integer& data)
{
	data = int((unsigned char)(bytes[0]) << 24 |
		(unsigned char)(bytes[1]) << 16 |
		(unsigned char)(bytes[2]) << 8 |
		(unsigned char)(bytes[3]));
}
