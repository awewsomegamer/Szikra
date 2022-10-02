# Szikra Architecture

### What
A revised version of awewsomegamer/Compiler and awewsomegamer/Emulator

### Contents
<pre>
/assembler/ 	- Discontinued assembler, first version
/assemblerv2/   - New assembler, second version

/compiler/	- Simple compiler designed to compile to the assembly language (not yet worked on)

/emulator/ 	- Works with assemblerv2

/test/		- Test files 
</pre>

## Architecture

### Instructions
<pre>
NOP				  ; Preform no operation

MOV 		V1, V2 		  ; Move V2 into V1

ADD 		V1, V2 		  ; Add V2 to V1
SUB 		V1, V2 		  ; Subtract V2 from V1
MUL 		V1, V2 		  ; Multiply V1 by V2
DIV 		V1, V2 		  ; Divide V1 by V2

OR		V1, V2 		  ; Bitwise OR V1 with V2
AND		V1, V2 		  ; Bitwise AND V1 with V2
NOT		V1	  	  ; Bitwise NOT V1

INT 		V1		  ; Call interrupt V1
SIVTE 		V1, V2 		  ; Set interrupt V1 to point to V2

CMP 		V1, V2 		  ; Compare V1 with V2

JMP 		V1		  ; Jump to address V1

CALL 		V1		  ; Push the current IP and jump to address V1
RET				  ; Return to the next instruction after a call

JE		V1	   	  ; Jump to V1 if compare yielded an equal
JNE		V1	   	  ; Jump to V1 if compare did not yield an equal

JG		V1	   	  ; Jump to V1 if compare yielded a greater than
JGE		V1	   	  ; Jump to V1 if compare did not yield a greater than

JL		V1	   	  ; Jump to V1 if compare yielded a less than
JLE		V1	   	  ; Jump to V1 if compare did not yield a less than

JC		V1	   	  ; Jump to V1 if carry flag is set
JNC		V1	   	  ; Jump to V1 if carry flag is not set

JZ		V1	   	  ; Jump to V1 if zero flag is set
JNZ		V1		  ; Jump to V1 if zero flag is not set

PUSH		V1		  ; Push V1 to the stack
POP		V1 	   	  ; Pop the last element on the stack into V1

DB		... <, ...> 	  ; Define (a) value(s)
DS		"..." <, "...">   ; Define (a) string(s) of ASCII characters
</pre>

### Registers
<pre>
AR	+- General purpose registers
BR	|  Are used for interrupt operands
CR	|
DR	|
ER	|
FR	|
GR	|
HR	+

I1	+- General purpose integer registers
I2	|  Used for arithmetic instructions
I3	|
I4	|
I5	|
I6	|
I7	|
I8	+

BP	- Stack base pointer register
SP	- Stack pointer register
IP	- Instruction pointer register
</pre>

### Instruction encoding
<pre>
+ 1 byte opcode
+ 1 byte information
	+ 2 bits - (Size - 1) (in bytes) of operand 2
	+ 2 bits - Operand 2 is: 0: Regular, 1: Register, 2: Pointer, 3: Register Pointer
	+ 2 bits - (Size - 1) (in bytes) of operand 1
	+ 2 bits - Operand 1 is: 0: Regular, 1: Register, 2: Pointer, 3: Register Pointer

+-- Repeated for each operand --+
|	+ x byte operand	|
+-------------------------------+
</pre>

### Syntax
`[]` Reference value at enclosed value <br>
`x:` Define a global label named x <br>
`.x:` Define a local label named x (possibly)<br>
`!` Assembler directive <br>
`#include "<path/to/file">` Include binary here<br>
`%include "<path/to/file">` Include code here (possibly)<br>

### Directives
`!AT V1` Move writer to V1 <br>
`!TEXT` Everything after this point is code <br>
`!DATA` Everything after this point is data (relocated to the end of the file by default) <br>
`!PUSHWRH` Saves the current write position (!PUSHWRH !AT 0x100 ... !POPWRH) <br>
`!POPWRH` Restores the last write position <br>

### Memory map
`0x0 -> 0x1FF` Boot sector (marked by last two bytes being 0xAABB)

### IO Device Map
`0x0` Keyboard scancode

### Interrupt Map
<pre>
Interrupt 0: Clock interrupt
Interrupt 1: Screen functions
	+ AR - Value
	+ BR - Color
	+ CR - Mode (0: putc, 1: set cursor position, 2: get cursor position, 3: draw point)

Interrupt 2: Disk functions
	+ AR - Disk number
	+ BR - Pointer to buffer
	+ CR - Buffer length

Interrupt 3: Keyboard interrupt

</pre>

### Microcode (not up to date)
<pre>
* Everything reads in from the bus, and writes out to the bus

+= Read into registers =+
| AIN			|
| BIN			|
| CIN			|
| DIN			|
| SPIN			|
| BPIN			|
| IPIN			|
| I1IN			|
| I2IN			|
| I3IN			|
| I4IN			|
+-----------------------+

+= Write out from registers =+
| AOUT			     |
| BOUT			     |
| COUT			     |
| DOUT			     |
| SPOUT			     |
| BPOUT			     |
| IPOUT			     |
| I1OUT			     |
| I2OUT			     |
| I3OUT			     |
| I4OUT			     |
+----------------------------+

+= Increment Program Counter =+
PCI

+= Write to RAM =+
MO

+= Read in from RAM =+
MI

+= Write to memory address register =+
WMR

+= Read from memory address register =+
RMR

+= Write bus =+
WBUS

+= Write bus =+
RBUS

// The operation will be set using a bus, A OP B = C
+= ALU Registers =+
| R1IN		  | // A
| R2IN		  | // B
| R3OUT		  | // C
+-----------------+    

</pre>