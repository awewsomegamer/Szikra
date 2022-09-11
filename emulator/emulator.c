#include <emulator.h>

uint8_t memory[UINT16_MAX];
uint8_t emulator_running = 1;
pthread_t process_thread;

uint32_t registers[I_REG_MAX]; 

#define EXPRESSION_DEPENDANT_BACKEND(instruction, value1, value2) \
		        if (instruction == I_MOV_INSTRUCTION) value1 =   value2; \ 
		   else if (instruction == I_ADD_INSTRUCTION) value1 +=  value2; \ 
		   else if (instruction == I_SUB_INSTRUCTION) value1 -=  value2; \ 
		   else if (instruction == I_MUL_INSTRUCTION) value1 *=  value2; \ 
		   else if (instruction == I_DIV_INSTRUCTION) value1 /=  value2; \ 
		   else if (instruction == I_AND_INSTRUCTION) value1 &=  value2; \ 
		   else if (instruction == I_OR_INSTRUCTION) value1  |=  value2; \ 
		   else if (instruction == I_XOR_INSTRUCTION) value1 ^=  value2; \ 
		   else if (instruction == I_SHR_INSTRUCTION) value1 >>= value2; \ 
		   else if (instruction == I_SHL_INSTRUCTION) value1 <<= value2;

// Ensure instruction is valid, if so return the opcode and increment IP
int fetch_instruction() {
	if (memory[registers[I_REG_IP]] > I_INSTRUCTION_MAX); // Call invalid opcode interrupt
	return memory[registers[I_REG_IP]++];
}

// Return a list of arguments based on ISA[instruction]
void load_arguments(int instruction, struct argument* argument_list) {
	for (int i = 0; i < ISA[instruction].argc; i++) {
		argument_list[i].type   =  (memory[registers[I_REG_IP]] >> 6) & 0b11;
		argument_list[i].length = ((memory[registers[I_REG_IP]] >> 4) & 0b11) + 1;
		argument_list[i].cast   =  (memory[registers[I_REG_IP]] >> 2) & 0b11;
		argument_list[i].offset =  (memory[registers[I_REG_IP]] >> 1) & 0b1;
		argument_list[i].sign   =  (memory[registers[I_REG_IP]] >> 0) & 0b1;

		registers[I_REG_IP]++;

		for (int j = 0; j < argument_list[i].length; j++)
			argument_list[i].value = memory[registers[I_REG_IP]++] << (j * 8);
	}
}

void expression_dependant_instruction_evaluator(int instruction, struct argument* arguments) {
	uint8_t t1 = arguments[0].type;
	uint8_t t2 = arguments[1].type;

	// A [B], [C]
	// A B, [C]
	// A [B], C
	// A B, C
	if ((t1 == CODE_RVALUE && t2 == CODE_RVALUE) || (t1 == CODE_PVALUE && t2 == CODE_RVALUE)) {
		if ((t1 == CODE_RVALUE && t2 == CODE_PVALUE) || (t1 == CODE_PVALUE && t2 == CODE_PVALUE)) {
			EXPRESSION_DEPENDANT_BACKEND(instruction, memory[arguments[0].value], memory[arguments[1].value]);
			return;
		}

		EXPRESSION_DEPENDANT_BACKEND(instruction, memory[arguments[0].value], arguments[1].value);
		return;
	}

	// A [AX], [BX]
	// A AX, [BX]
	// A [AX], BX
	// A AX, BX
	if ((t1 == CODE_RREG && t2 == CODE_RREG) || (t1 == CODE_PREG && t2 == CODE_RREG)) {
		if ((t1 == CODE_RREG && t2 == CODE_PREG) || (t1 == CODE_PREG && t2 == CODE_PREG)) {
			EXPRESSION_DEPENDANT_BACKEND(instruction, memory[registers[arguments[0].value]], memory[registers[arguments[1].value]]);
			return;
		}

		EXPRESSION_DEPENDANT_BACKEND(instruction, memory[registers[arguments[0].value]], registers[arguments[1].value]);
		return;
	}

	// A AX, C
	if ((t1 == CODE_RREG && t2 == CODE_RVALUE)) {
		EXPRESSION_DEPENDANT_BACKEND(instruction, registers[arguments[0].value], arguments[1].value);
	}

	// A [AX], C
	// A [AX], [C]
	if (t1 == CODE_PREG) {
		if (t2 == CODE_RVALUE) {
			EXPRESSION_DEPENDANT_BACKEND(instruction, registers[arguments[0].value], arguments[1].value);
		} else if (t2 == CODE_PVALUE) {
			EXPRESSION_DEPENDANT_BACKEND(instruction, registers[arguments[0].value], arguments[1].value);
		}
	}


	// A C, AX
	// A [C], AX
	// A [C], [AX]
	if (t1 == CODE_RVALUE || t1 == CODE_PVALUE) {
		if (t2 == CODE_RREG) {
			EXPRESSION_DEPENDANT_BACKEND(instruction, memory[arguments[0].value], registers[arguments[1].value]);
		} else if (t2 == CODE_PREG) {
			EXPRESSION_DEPENDANT_BACKEND(instruction, memory[arguments[0].value], memory[registers[arguments[1].value]]);
		}
	}

}

void process_instruction(int instruction, struct argument* arguments) {
	expression_dependant_instruction_evaluator(instruction, arguments);
}

void* proccess_cycle(void* arg) {
	int instruction = fetch_instruction();
	struct argument arguments[256];
	load_arguments(instruction, arguments);

	process_instruction(instruction, arguments);

	printf("%s %X %X (IP: %X)\n", ISA[instruction].name, arguments[0].value, arguments[1].value, registers[I_REG_IP]);
	printf("%X\n", registers[I_REG_BX]);
}