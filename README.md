# Simple Instruction Architechture (SIA)
SIA is a simple RISC assembly language based on MIPS. SIA is designed as a beginner friendly language to quickly learn fundamental assembly and low-level programming concepts. 

## SIA Instructions 

- **add** 
	(opcode 1)
	Adds the values of 2 registers and places the answer in a third register. 
	- Example: add r1 r2 r3 ; r3  r1 + r2
	- Instruction format: 3R

- **addimmediate** 
	(opcode 9)
	Adds a signed 8 bit value to a register
	- Example: addimediate r1 -127 ; adds -127 to r1
	- Instruction format: ai

- **and** 
	(opcode 2)
	Preforms a bitwise and on 2 registers and stores the result in a third register
	- Example: and r1 r2 r3 ; r3  r1 & r2
	- Instruction format: 3R

- **branchifequal**
	(opcode 10)
	Compares two registers – if they hold the same value, jump to an offset from the current program counter. The offset can be between -524,286 and 524,286. The offset is the number of words (2 bytes) forward or back. PC <= PC + (2 * offset).
	- Example: branchifequal r1 r2 1000 
	- Instruction format: br

- branchifless (opcode 11)
Compares two registers – if the first is less than the second, jump to an offset from the current program counter. The offset can be between -524,286 and 524,286. The offset is the number of words (2 bytes) forward or back. PC <= PC + (2 * offset).
Example: branchifless r1 r2 1000 
Instruction format: br

- call (opcode 13)
Calls a “function” – pushes the PC of the next instruction onto the stack (R15), then jumps to the address specified by this instruction times 2 (0 – 1,073,741,824).
Example: call 444 
Instruction format: jmp

- divide (opcode 3)
Divides the value of the first register by the second and places the answer in a third register. This is integer math with the fractional portion discarded.
	Example: divide r1 r2 r3 ; r3  r1 / r2
	Instruction format: 3R

- halt (opcode 0)
	Stops the CPU.
	Example: halt
	Instruction format: 3R (the register values don’t matter)

- interrupt (opcode 8)
Interrupts the CPU using a particular interrupt number. This could be used to jump between kernel mode and user mode or to support devices. For the virtual machine, two interrupts are supported: 0 (print registers) and 1 (print out memory)
Example: interrupt 17
Instruction format: int

- jump (opcode 12)
	Jumps to the location specified in the instruction times 2 (0 – 1,073,741,824)
	Example: jump 1000
	instruction format: jmp

- load (opcode 14)
Loads a register from the memory pointed to by another register plus 2 times the offset (0 to 30). Note that both the address in the register and the offset are in words (memory locations).
	Example: load r1 r2 10 ; loads r1 with the value pointed to by r2 plus 20 bytes
	instruction format: ls

- multiply (opcode 4)
Multiplies the value of the first register times the second and places the answer in a third register. 
	Example: multiply r1 r2 r3 ; r3  r1 * r2
	Instruction format: 3R

- or (opcode 6)
	Performs a bitwise OR on 2 registers and stores the result in a third register
	Example: or r1 r2 r3 ; r3  r1 | r2
	Instruction format: 3R

- pop (opcode 7)
Copies data from stack pointer through stack pointer + 3 to specified register. Adds four to the stack pointer.
	Example: pop R1
	Instruction format: stack

- push (opcode 7)
Subtracts four from the stack pointer. Takes the value in the specified register and stores it in the memory address indicated by the stack pointer. 
	Example: pop R1
	Instruction format: stack

- return (opcode 7)
	Pops the top value from the stack and jumps to that address
	Example: return
	Instruction format: stack

- store (opcode 15)
Stores a register’s value into memory memory pointed to by another register plus 2 times the offset (0 to 30). Note that both the address in the register and the offset are in words (memory locations).
Example: store r1 r2 10 ; stores r1’s value into the memory pointed to by r2 plus 20 bytes
	instruction format: ls

- subtract (opcode 5)
Subtracts the value of the second register from the first and places the answer in a third register. 
	Example: subtract r1 r2 r3 ; r3  r1 - r2
	Instruction format: 3R

- The assembler is a C++ program to assemble source files in SIA to binary
- The virtual machine is also designed in C++, and can be used to test SIA binary code
