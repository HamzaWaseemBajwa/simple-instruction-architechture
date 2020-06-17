#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

using namespace std;

// struct definition for storing instruction bytes and byte count
struct bin_instruction
{
	char bytes[4];
	int byte_count;

	void init() // initlizes the bytes and byte count to 0 and 
	{
		byte_count = 0;
		for (int i = 0; i < 4; ++i)
		{
			bytes[i] = '\0';
		}
	}
};

// returns an integer value indicating the format of the instruction
int get_format(string opcode)
{
	if (opcode == "add" ||
		opcode == "and" ||
		opcode == "divide" ||
		opcode == "halt" ||
		opcode == "multiply" ||
		opcode == "or" ||
		opcode == "subtract")
	{
		return 0;
	}
	if (opcode == "addimmediate")
	{
		return 1;
	}
	if (opcode == "branchifequal" ||
		opcode == "branchifless")
	{
		return 2;
	}
	if (opcode == "call" ||
		opcode == "jump")
	{
		return 3;
	}
	if (opcode == "interrupt")
	{
		return 4;
	}
	if (opcode == "load" ||
		opcode == "store")
	{
		return 5;
	}
	if (opcode == "pop" ||
		opcode == "push" ||
		opcode == "return")
	{
		return 6;
	}
	return -1;
}

// returns an 8-bit value from the register name
char get_register(string token)
{
	return (char)stoi(token.substr(1));
}

// returns an 8-bit value corresponding to the opcode
char get_bin_opcode(string opcode)
{
	if (opcode == "add")
		return 0x10;
	if (opcode == "addimmediate")
		return 0x90;
	if (opcode == "and")
		return 0x20;
	if (opcode == "branchifequal")
		return 0xa0;
	if (opcode == "branchifless")
		return 0xb0;
	if (opcode == "call")
		return 0xd0;
	if (opcode == "divide")
		return 0x30;
	if (opcode == "halt")
		return 0x00;
	if (opcode == "interrupt")
		return 0x80;
	if (opcode == "jump")
		return 0xc0;
	if (opcode == "load")
		return 0xe0;
	if (opcode == "multiply")
		return 0x40;
	if (opcode == "or")
		return 0x60;
	if (opcode == "pop")
		return 0x70;
	if (opcode == "push")
		return 0x70;
	if (opcode == "return")
		return 0x70;
	if (opcode == "store")
		return 0xf0;
	if (opcode == "subtract")
		return 0x50;
	return -1;
}

// returns the correct flag for a "stack" type instruction
char get_stack_code(string opcode)
{
	if (opcode == "pop")
		return 0x80;
	if (opcode == "push")
		return 0x40;
	if (opcode == "return")
		return 0x00;
	return '\0';
}

// given a list of assembly instruction tokens, return the instruction in binary form
bin_instruction assemble(string token_list[])
{
	int format = get_format(token_list[0]);	// determine the format
	bin_instruction inst;	// declare and init return value
	inst.init();
	inst.bytes[0] = get_bin_opcode(token_list[0]); // get the opcode
	switch (format)	// decode based on format
	{
	case 0:		// 3R type
		inst.byte_count = 2;
		inst.bytes[0] |= get_register(token_list[1]);
		inst.bytes[1] |= (get_register(token_list[2]) << 4);
		inst.bytes[1] |= (get_register(token_list[3]));
		break;
	case 1:		// AI type
		inst.byte_count = 2;
		inst.bytes[0] |= get_register(token_list[1]);
		inst.bytes[1] |= (signed char)stoi(token_list[2]);
		break;
	case 2:		// BR type
	{
		inst.byte_count = 4;
		inst.bytes[0] |= get_register(token_list[1]);
		inst.bytes[1] |= (get_register(token_list[2]) << 4);
		int off = stoi(token_list[3]);		// get offset

		// store offset byte by byte, shiift 8 bits after reading a byte
		inst.bytes[3] = (char)off;
		off = off >> 8;
		inst.bytes[2] = (char)off;
		off = off >> 8;

		/*char x = (char)off;
		inst.bytes[1] |= 0x0f & x;*/

		inst.bytes[1] |= off;
		break;
	}
	case 3:		// JMP type
	{
		inst.byte_count = 4;
		int addr = stoi(token_list[1]);
		inst.bytes[3] = (char)addr;
		addr = addr >> 8;
		inst.bytes[2] = (char)addr;
		addr = addr >> 8;
		inst.bytes[1] = (char)addr;
		addr = addr >> 8;

		/*char x = (char)addr;
		inst.bytes[0] |= 0x0f & x;*/

		inst.bytes[0] |= addr;
		break;
	}
	case 4:		// INT type
		inst.byte_count = 2;
		inst.bytes[1] = stoi(token_list[1]);
		break;
	case 5:
		inst.byte_count = 2;		// LS type
		inst.bytes[0] |= get_register(token_list[1]);
		inst.bytes[1] |= (get_register(token_list[2]) << 4);
		inst.bytes[1] |= stoi(token_list[3]);
	break;
	case 6:
		inst.byte_count = 2;		// STACK type
		if (token_list[0] != "return")
			inst.bytes[0] |= get_register(token_list[1]);
		inst.bytes[1] |= get_stack_code(token_list[0]);
		break;
	default:
		break;
	}
	return inst;
}

int main(int argc, char* argv[])
{
	// open the input file
	ifstream in_file(argv[1]);
	string instruction; // str to store instruction
	ofstream out_file(argv[2], std::ios::binary);	// binary output file
	while (getline(in_file, instruction))			// read input file line by line
	{
		stringstream sstream(instruction);	// break each line by the space char
		// store tokens in an array
		string token_list[4];
		int i = 0;
		while (getline(sstream, token_list[i], ' '))
		{
			++i;
		}
		bin_instruction inst = assemble(token_list);
		out_file.write(inst.bytes, inst.byte_count);	// write assembled instruction to output file
	}
	in_file.close();
	out_file.close();
}