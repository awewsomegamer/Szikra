# Szikra Architecture

### What
A revised version of awewsomegamer/Compiler and awewsomegamer/Emulator

### Why
The old assembler wasn't modular, by this I mean that it relied on the use
of sections, and that each operation would have only two arguments.
With this new system I hope to be able to add more instruction arguments as
well as directives local labels, and many more things to make the assembler
better.

## Architecture

### Instructions
<pre>
NOP				  : Preform no operation

MOV 		V1, V2 		  : Move V2 into V1

ADD 		V1, V2 		  : Add V2 to V1
SUB 		V1, V2 		  : Subtract V2 from V1
MUL 		V1, V2 		  : Multiply V1 by V2
DIV 		V1, V2 		  : Divide V1 by V2

OR		V1, V2 		  : Bitwise OR V1 with V2
XOR 		V1, V2 		  : Bitwise XOR V1 with V2
AND		V1, V2 		  : Bitwise AND V1 with V2

NOT		V1	  	  : Bitwise NOT V1

INT 		V1		  : Call interrupt V1
SIVTE 		V1, V2 		  : Set interrupt V1 to point to V2
RIVTE 		V1		  : Reset interrupt V1 to default pointer
IRET		   		  : Restore the context before interrupt was called

CMP 		V1, V2 		  : Compare V1 with V2

JMP 		V1		  : Jump to address V1

CALL 		V1		  : Push the current IP and jump to address V1
RET				  : Return to the next instruction after a call

JE		V1	   	  : Jump to V1 if compare yielded an equal
JNE		V1	   	  : Jump to V1 if compare did not yield an equal

JG		V1	   	  : Jump to V1 if compare yielded a greater than
JGE		V1	   	  : Jump to V1 if compare did not yield a greater than

JL		V1	   	  : Jump to V1 if compare yielded a less than
JLE		V1	   	  : Jump to V1 if compare did not yield a less than

JC		V1	   	  : Jump to V1 if carry flag is set
JNC		V1	   	  : Jump to V1 if carry flag is not set

JZ		V1	   	  : Jump to V1 if zero flag is set
JNZ		V1		  : Jump to V1 if zero flag is not set

PUSH		V1		  : Push V1 to the stack
POP		V1 	   	  : Pop the last element on the stack into V1

DB		... <, ...> 	  : Define (a) value(s)
DS		"..." <, "...">   : Define (a) string(s) of ASCII characters
</pre>

### Registers
<pre>
AX	+- General purpose registers
BX	|  Are used for interrupt operands
CX	|
DX	+-
I1	+- General purpose integer registers
I2	|  Used for arithmetic instructions
I3	|
I4	+-
BP	- Stack base pointer register
SP	- Stack pointer register
IP	- Instruction pointer register
</pre>

### Instruction encoding
<pre>
+ 1 byte opcode
+------------------------------------ Repeated for each argument --------------------------+
|+ 1 byte arg descriptor								   |
|	+ 2 bits - argument type (0: regular, 1: pointer, 2: register, 3: register pointer)|
|	+ 2 bits - argument length (subtracted by 1)					   |
|	+ 2 bits - argument cast (0: dword, 1: word, 2: byte, 3: reserved)		   |
|	+ 1 bit  - argument is offset (0: no, 1: yes)					   |
|	+ 1 bit  - argument sign (0: positive, 1: negative)				   |
|+ x byte arg										   |
+------------------------------------------------------------------------------------------+
</pre>

### Syntax
`[]` Reference value at enclosed value <br>
`x:` Define a global label named x <br>
`.x:` Define a local label named x <br>
`!` Assembler directive <br>
`#include "<path/to/file">` Include binary here <br>
`%include "<path/to/file">` Include code here <br>

### Directives
`!AT V1` Move writer to V1 <br>
`!TEXT` Everything after this point is code <br>
`!DATA` Everything after this point is data (relocated to the end of the file by default) <br>

### Memory map
`0x0 -> 0x1FF` Boot sector (marked by last two bytes being 0xAABB)
### IO Device Map
`0x0 -> 0x4` Keyboard (INFO(0x00) DATA(0x00 0x00 0x00))
### Interrupt Map
Interrupt 0: Clock interrupt
Interrupt 1: Screen functions
Interrupt 2: Disk functions
Interrupt 3: Keyboard interrupt
