#include <emulator.h>

uint8_t memory[UINT16_MAX];
uint8_t emulator_running = 1;
pthread_t process_thread;

uint32_t registers[I_REG_MAX]; 

void(*evaluated_instructions[I_INSTRUCTION_MAX])(struct argument* arguments);

#define EXPRESSION_DEPENDENT_BACKEND(instruction, value1, value2) \
		        if (instruction == I_MOV_INSTRUCTION) value1 =   value2; \ 
		   else if (instruction == I_ADD_INSTRUCTION) value1 +=  value2; \ 
		   else if (instruction == I_SUB_INSTRUCTION) value1 -=  value2; \ 
		   else if (instruction == I_MUL_INSTRUCTION) value1 *=  value2; \ 
		   else if (instruction == I_DIV_INSTRUCTION) value1 /=  value2; \ 
		   else if (instruction == I_AND_INSTRUCTION) value1 &=  value2; \ 
		   else if (instruction == I_OR_INSTRUCTION)  value1 |=  value2; \ 
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

// Instructions such as MOV need the full context of each argument
// to be fully able to preform its task.
// For this purpose there are two categories of instructions: expression dependent, and evaluated
// Where expression dependent are things like MOV, and evaluated are things like CMP
void do_expression_dependent_instruction(int instruction, struct argument* arguments) {
	uint8_t t1 = arguments[0].type;
	uint8_t t2 = arguments[1].type;

	// A [B], [C]
	// A B, [C]
	// A [B], C
	// A B, C
	if ((t1 == CODE_RVALUE && t2 == CODE_RVALUE) || (t1 == CODE_PVALUE && t2 == CODE_RVALUE)) {
		if ((t1 == CODE_RVALUE && t2 == CODE_PVALUE) || (t1 == CODE_PVALUE && t2 == CODE_PVALUE)) {
			EXPRESSION_DEPENDENT_BACKEND(instruction, memory[arguments[0].value], memory[arguments[1].value]);
			return;
		}

		EXPRESSION_DEPENDENT_BACKEND(instruction, memory[arguments[0].value], arguments[1].value);
		return;
	}

	// A [AX], [BX]
	// A AX, [BX]
	// A [AX], BX
	// A AX, BX
	if ((t1 == CODE_RREG && t2 == CODE_RREG) || (t1 == CODE_PREG && t2 == CODE_RREG)) {
		if ((t1 == CODE_RREG && t2 == CODE_PREG) || (t1 == CODE_PREG && t2 == CODE_PREG)) {
			EXPRESSION_DEPENDENT_BACKEND(instruction, memory[registers[arguments[0].value]], memory[registers[arguments[1].value]]);
			return;
		}

		EXPRESSION_DEPENDENT_BACKEND(instruction, memory[registers[arguments[0].value]], registers[arguments[1].value]);
		return;
	}

	// A AX, C
	if ((t1 == CODE_RREG && t2 == CODE_RVALUE)) {
		EXPRESSION_DEPENDENT_BACKEND(instruction, registers[arguments[0].value], arguments[1].value);
	}

	// A [AX], C
	// A [AX], [C]
	if (t1 == CODE_PREG) {
		if (t2 == CODE_RVALUE) {
			EXPRESSION_DEPENDENT_BACKEND(instruction, registers[arguments[0].value], arguments[1].value);
		} else if (t2 == CODE_PVALUE) {
			EXPRESSION_DEPENDENT_BACKEND(instruction, registers[arguments[0].value], arguments[1].value);
		}
	}

	// A C, AX
	// A [C], AX
	// A [C], [AX]
	if (t1 == CODE_RVALUE || t1 == CODE_PVALUE) {
		if (t2 == CODE_RREG) {
			EXPRESSION_DEPENDENT_BACKEND(instruction, memory[arguments[0].value], registers[arguments[1].value]);
		} else if (t2 == CODE_PREG) {
			EXPRESSION_DEPENDENT_BACKEND(instruction, memory[arguments[0].value], memory[registers[arguments[1].value]]);
		}
	}

}

int evaluate_argument(struct argument arg) {
	switch (arg.type) {
	case CODE_RVALUE:
		return arg.value;
	case CODE_PVALUE:
		return memory[arg.value];
	case CODE_RREG:
		return registers[arg.value];
	case CODE_PREG:
		return memory[registers[arg.value]];
	}
}

void stack_push(uint32_t v) {
	memory[registers[I_REG_SP]++] = v	 & 0xFF;
	memory[registers[I_REG_SP]++] = v >> 8   & 0xFF;
	memory[registers[I_REG_SP]++] = v >> 16  & 0xFF;
	memory[registers[I_REG_SP]++] = v >> 24  & 0xFF;
}

uint32_t stack_pop() {
	registers[I_REG_SP] -= 4;

	uint32_t result = memory[registers[I_REG_SP]] | memory[registers[I_REG_SP] + 1] << 8 | memory[registers[I_REG_SP] + 2] << 16 |memory[registers[I_REG_SP] + 3] << 24;

	memory[registers[I_REG_SP]] = 0;
	memory[registers[I_REG_SP] + 1] = 0;
	memory[registers[I_REG_SP] + 2] = 0;
	memory[registers[I_REG_SP] + 3] = 0;

	return result;
}

void NOP_INSTRUCTION(struct argument* arguments) { }

void NOT_INSTRUCTION(struct argument* arguments) {
	switch (arguments[0].type) {
	case CODE_RVALUE:
	case CODE_PVALUE:
		memory[arguments[0].value] = ~memory[arguments[0].value];
		break;

	case CODE_RREG:
		registers[arguments[0].value] = ~registers[arguments[0].value];
		break;

	case CODE_PREG:
		memory[registers[arguments[0].value]] = ~memory[registers[arguments[0].value]];
		break;
	}
}

void INT_INSTRUCTION(struct argument* arguments) {
	// Call interrupt wrapper
}

void SIVTE_INSTRUCTION(struct argument* arguments) {
	// Call interrupt set instruction
}

void RIVTE_INSTRUCTION(struct argument* arguments) {
	// Call interupt reset instruction
}

void IRET_INSTRUCTION(struct argument* arguments) {
	// RET with a few extra steps, depends on what is done before
	// interrupt wrapper is called in INT_INSTRUCTION
}

void CMP_INSTRUCTION(struct argument* arguments) {
	uint32_t a = evaluate_argument(arguments[0]);
	uint32_t b = evaluate_argument(arguments[0]);
}

void JMP_INSTRUCTION(struct argument* arguments) {
	registers[I_REG_IP] = evaluate_argument(arguments[0]);
}

void CALL_INSTRUCTION(struct argument* arguments) {
	stack_push(registers[I_REG_IP]);
	registers[I_REG_IP] = evaluate_argument(arguments[0]);
}

void RET_INSTRUCTION(struct argument* arguments) {
	registers[I_REG_IP] = stack_pop();
}

void JE_INSTRUCTION(struct argument* arguments) {
	
}

void JNE_INSTRUCTION(struct argument* arguments) {
	
}

void JG_INSTRUCTION(struct argument* arguments) {
	
}

void JGE_INSTRUCTION(struct argument* arguments) {
	
}

void JL_INSTRUCTION(struct argument* arguments) {
	
}

void JLE_INSTRUCTION(struct argument* arguments) {
	
}

void JC_INSTRUCTION(struct argument* arguments) {
	
}

void JNC_INSTRUCTION(struct argument* arguments) {
	
}

void JZ_INSTRUCTION(struct argument* arguments) {
	
}

void JNZ_INSTRUCTION(struct argument* arguments) {
	
}

void PUSH_INSTRUCTION(struct argument* arguments) {
	stack_push(evaluate_argument(arguments[0]));
}

void POP_INSTRUCTION(struct argument* arguments) {
	switch (arguments[0].type) {
	case CODE_RVALUE:
	case CODE_PVALUE:
		memory[arguments[0].value] = stack_pop();
		break;
	case CODE_RREG:
		registers[arguments[0].value] = stack_pop();
		break;
	case CODE_PREG:
		memory[registers[arguments[0].value]] = stack_pop();
		break;
	}
}

void process_instruction(int instruction, struct argument* arguments) {
	if (ISA[instruction].argc == 2 && instruction != I_SIVTE_INSTRUCTION && instruction != I_CMP_INSTRUCTION)
		do_expression_dependent_instruction(instruction, arguments);
	else
		(*evaluated_instructions[instruction])(arguments);
}

void* proccess_cycle(void* arg) {
	while (emulator_running) {
		struct argument arguments[256];
		int instruction = fetch_instruction();
		load_arguments(instruction, arguments);

		process_instruction(instruction, arguments);
	}
}

void init_emulator() {
	evaluated_instructions[I_NOP_INSTRUCTION] =   NOP_INSTRUCTION;
	evaluated_instructions[I_NOT_INSTRUCTION] =   NOT_INSTRUCTION;
	evaluated_instructions[I_INT_INSTRUCTION] =   INT_INSTRUCTION;
	evaluated_instructions[I_SIVTE_INSTRUCTION] = SIVTE_INSTRUCTION;
	evaluated_instructions[I_RIVTE_INSTRUCTION] = RIVTE_INSTRUCTION;
	evaluated_instructions[I_IRET_INSTRUCTION] =  IRET_INSTRUCTION;
	evaluated_instructions[I_CMP_INSTRUCTION] =   CMP_INSTRUCTION;
	evaluated_instructions[I_JMP_INSTRUCTION] =   JMP_INSTRUCTION;
	evaluated_instructions[I_CALL_INSTRUCTION] =  CALL_INSTRUCTION;
	evaluated_instructions[I_RET_INSTRUCTION] =   RET_INSTRUCTION;
	evaluated_instructions[I_JE_INSTRUCTION] =    JE_INSTRUCTION;
	evaluated_instructions[I_JNE_INSTRUCTION] =   JNE_INSTRUCTION;
	evaluated_instructions[I_JG_INSTRUCTION] =    JG_INSTRUCTION;
	evaluated_instructions[I_JGE_INSTRUCTION] =   JGE_INSTRUCTION;
	evaluated_instructions[I_JL_INSTRUCTION] =    JL_INSTRUCTION;
	evaluated_instructions[I_JLE_INSTRUCTION] =   JLE_INSTRUCTION;
	evaluated_instructions[I_JC_INSTRUCTION] =    JC_INSTRUCTION;
	evaluated_instructions[I_JNC_INSTRUCTION] =   JNC_INSTRUCTION;
	evaluated_instructions[I_JZ_INSTRUCTION] =    JZ_INSTRUCTION;
	evaluated_instructions[I_JNZ_INSTRUCTION] =   JNZ_INSTRUCTION;
	evaluated_instructions[I_PUSH_INSTRUCTION] =  PUSH_INSTRUCTION;
	evaluated_instructions[I_POP_INSTRUCTION] =   POP_INSTRUCTION;
}