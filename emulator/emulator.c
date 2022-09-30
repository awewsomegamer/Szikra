#include <emulator.h>
#include <interrupts.h>

uint8_t memory[UINT16_MAX];
uint8_t emulator_running = 1;
pthread_t process_thread;

uint32_t registers[I_REG_MAX];
struct flags cflags;

void(*evaluated_instructions[I_INSTRUCTION_MAX])(struct argument* arguments, int instruction);

#define EXPRESSION_DEPENDENT_BACKEND(instruction, value1, value2) \
		        if (instruction == I_MOV_INSTRUCTION) value1 =   value2; \ 
		   else if (instruction == I_ADD_INSTRUCTION) value1 +=  value2; \ 
		   else if (instruction == I_SUB_INSTRUCTION) value1 -=  value2; \ 
		   else if (instruction == I_MUL_INSTRUCTION) value1 *=  value2; \ 
		   else if (instruction == I_DIV_INSTRUCTION) value1 /=  value2; \ 
		   else if (instruction == I_AND_INSTRUCTION) value1 &=  value2; \ 
		   else if (instruction == I_OR_INSTRUCTION)  value1 |=  value2; \ 
		   else if (instruction == I_SHR_INSTRUCTION) value1 >>= value2; \ 
		   else if (instruction == I_SHL_INSTRUCTION) value1 <<= value2;
		//    else if (instruction == I_XOR_INSTRUCTION) value1 ^=  value2; \ 


int next_byte() {
	return memory[registers[I_REG_IP]++];
}

// Ensure instruction is valid, if so return the opcode and increment IP
int fetch_instruction() {
	if (memory[registers[I_REG_IP]] > I_INSTRUCTION_MAX) {
		 printf("Invalid opcode (%X) at %X\n", memory[registers[I_REG_IP]], registers[I_REG_IP]); // Call invalid opcode interrupt
		 for (;;);
		 return 0;
	}
	return memory[registers[I_REG_IP]++];
}

// Return a list of arguments based on ISA[instruction]
void load_arguments(int instruction, struct argument* argument_list) {
	int i = 0;
	while (((memory[registers[I_REG_IP]] >> 7) & 1)) {
		argument_list[i].instruction =  (memory[registers[I_REG_IP]] >> 7) & 0b1;
		argument_list[i].type        =  (memory[registers[I_REG_IP]] >> 5) & 0b11;
		argument_list[i].length      = ((memory[registers[I_REG_IP]] >> 3) & 0b11) + 1;
		argument_list[i].cast        =  (memory[registers[I_REG_IP]] >> 1) & 0b11;
		argument_list[i].offset      =  (memory[registers[I_REG_IP]] >> 0) & 0b1;
		argument_list[i].info_byte   =   memory[registers[I_REG_IP]];
		
		registers[I_REG_IP]++;

		argument_list[i].value = 0;
		for (int j = 0; j < argument_list[i].length; j++)
			argument_list[i].value |= memory[registers[I_REG_IP]++] << (j * 8);

		i++;
	}
}

// Instructions such as MOV need the full context of each argument
// to be fully able to preform its task.
// For this purpose there are two categories of instructions: expression dependent, and evaluated
// Where expression dependent are things like MOV, and evaluated are things like CMP
void do_expression_dependent_instruction(int instruction, struct argument* arguments) {
	uint8_t t1 = arguments[0].type;
	uint8_t t2 = arguments[1].type;

	if (instruction >= I_MOV_INSTRUCTION && instruction < I_CMP_INSTRUCTION && ((instruction - 1) % 2) == 1) {
		if (t1 == CODE_RREG) {
			EXPRESSION_DEPENDENT_BACKEND((instruction - 1), registers[next_byte()], registers[arguments[0].value]);
			return;
		} else if (t1 == CODE_PREG) {
			EXPRESSION_DEPENDENT_BACKEND((instruction - 1), registers[next_byte()], memory[registers[arguments[0].value]]);
			return;
		} else if (t1 == CODE_RVALUE) {
			EXPRESSION_DEPENDENT_BACKEND((instruction - 1), registers[next_byte()], arguments[0].value);
			return;
		} else if (t1 == CODE_PVALUE) {
			EXPRESSION_DEPENDENT_BACKEND((instruction - 1), registers[next_byte()], memory[arguments[0].value]);
			return;
		}
	}

	// A [B], [C]
	// A B, [C]
	if ((t1 == CODE_PVALUE && t2 == CODE_RVALUE) || (t1 == CODE_PVALUE && t2 == CODE_PVALUE)) {
		EXPRESSION_DEPENDENT_BACKEND(instruction, memory[arguments[0].value], memory[arguments[1].value]);
		return;
	}

	// A [B], C
	// A B, C
	if ((t1 == CODE_RVALUE && t2 == CODE_RVALUE) || (t1 == CODE_PVALUE && t2 == CODE_RVALUE)) {
		EXPRESSION_DEPENDENT_BACKEND(instruction, memory[arguments[0].value], arguments[1].value);
		return;
	}

	// A AX, BX
	if (t1 == CODE_RREG && t2 == CODE_RREG) {
		EXPRESSION_DEPENDENT_BACKEND(instruction, registers[arguments[0].value], registers[arguments[1].value]);
		return;
	}


	// A [AX], BX
	if (t1 == CODE_PREG && t2 == CODE_RREG) {
		EXPRESSION_DEPENDENT_BACKEND(instruction, memory[registers[arguments[0].value]], registers[arguments[1].value]);
		return;
	}

	// A AX, [BX]
	if (t1 == CODE_RREG && t2 == CODE_PREG) {
		EXPRESSION_DEPENDENT_BACKEND(instruction, registers[arguments[0].value], memory[registers[arguments[1].value]]);
		return;
	}

	// A AX, C
	if (t1 == CODE_RREG && t2 == CODE_RVALUE) {
		EXPRESSION_DEPENDENT_BACKEND(instruction, registers[arguments[0].value], arguments[1].value);
		return;
	}

	// A [AX], [BX]
	if (t1 == CODE_PREG && t2 == CODE_PREG) {
		EXPRESSION_DEPENDENT_BACKEND(instruction, memory[registers[arguments[0].value]], memory[registers[arguments[1].value]]);
		return;
	}

	// A [AX], C
	if (t1 == CODE_PREG && t2 == CODE_RVALUE) {
		EXPRESSION_DEPENDENT_BACKEND(instruction, memory[registers[arguments[0].value]], arguments[1].value);
		return;
	}

	// A [AX], [C]
	if (t1 == CODE_PREG && t2 == CODE_PVALUE) {
		EXPRESSION_DEPENDENT_BACKEND(instruction, memory[registers[arguments[0].value]], memory[arguments[1].value]);
		return;
	}

	// A [C], [AX]
	if (t1 == CODE_PVALUE && t2 == CODE_RREG) {
		EXPRESSION_DEPENDENT_BACKEND(instruction, memory[arguments[0].value], memory[registers[arguments[1].value]]);
		return;
	}

	// A C, AX
	// A [C], AX
	if ((t1 == CODE_RVALUE && t2 == CODE_RREG) || (t1 == CODE_PVALUE && t2 == CODE_RREG)) {
		EXPRESSION_DEPENDENT_BACKEND(instruction, memory[arguments[0].value], registers[arguments[1].value]);
		return;
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

void NOP_INSTRUCTION(struct argument* arguments, int instruction) { }

void NOT_INSTRUCTION(struct argument* arguments, int instruction) {
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

void INT_INSTRUCTION(struct argument* arguments, int instruction) {
	// Call interrupt wrapper
	call_interrupt(evaluate_argument(arguments[0]));
}

void SIVTE_INSTRUCTION(struct argument* arguments, int instruction) {
	// Call interrupt set instruction
}

void RIVTE_INSTRUCTION(struct argument* arguments, int instruction) {
	// Call interupt reset instruction
}

void IRET_INSTRUCTION(struct argument* arguments, int instruction) {
	// RET with a few extra steps, depends on what is done before
	// interrupt wrapper is called in INT_INSTRUCTION
}

void CMP_INSTRUCTION(struct argument* arguments, int instruction) {
	uint32_t a = 0;
	uint32_t b = 0;

	if (instruction == I_CMPR_INSTRUCTION) {
		a = registers[next_byte()];
		b = evaluate_argument(arguments[0]);
	} else {
		a = evaluate_argument(arguments[0]);
		b = evaluate_argument(arguments[1]);
	}

	if (a > b) {
		cflags.carry = 1;
		cflags.zero = 0;
	} else if (a == b) {
		cflags.carry = 0;
		cflags.zero = 0;
	} else if (a < b) {
		cflags.carry = 0;
		cflags.zero = 1;
	}
}

void JMP_INSTRUCTION(struct argument* arguments, int instruction) {
	registers[I_REG_IP] = evaluate_argument(arguments[0]);
}

void CALL_INSTRUCTION(struct argument* arguments, int instruction) {
	stack_push(registers[I_REG_IP]);
	registers[I_REG_IP] = evaluate_argument(arguments[0]);
}

void RET_INSTRUCTION(struct argument* arguments, int instruction) {
	registers[I_REG_IP] = stack_pop();
}

void JE_INSTRUCTION(struct argument* arguments, int instruction) {
	if (!cflags.carry && !cflags.zero) {
		registers[I_REG_IP] = evaluate_argument(arguments[0]);
	}
}

void JNE_INSTRUCTION(struct argument* arguments, int instruction) {
	if (cflags.carry || cflags.zero) registers[I_REG_IP] = evaluate_argument(arguments[0]);
}

void JG_INSTRUCTION(struct argument* arguments, int instruction) {
	if (cflags.carry && !cflags.zero) registers[I_REG_IP] = evaluate_argument(arguments[0]);
}

void JGE_INSTRUCTION(struct argument* arguments, int instruction) {
	if ((!cflags.carry && !cflags.zero) || (cflags.carry && !cflags.zero)) registers[I_REG_IP] = evaluate_argument(arguments[0]);
}

void JL_INSTRUCTION(struct argument* arguments, int instruction) {
	if (!cflags.carry && cflags.zero) registers[I_REG_IP] = evaluate_argument(arguments[0]);
}

void JLE_INSTRUCTION(struct argument* arguments, int instruction) {
	if ((!cflags.carry && !cflags.zero) || (!cflags.carry && cflags.zero)) registers[I_REG_IP] = evaluate_argument(arguments[0]);
}

void JC_INSTRUCTION(struct argument* arguments, int instruction) {
	if (cflags.carry) registers[I_REG_IP] = evaluate_argument(arguments[0]);
}

void JNC_INSTRUCTION(struct argument* arguments, int instruction) {
	if (!cflags.carry) registers[I_REG_IP] = evaluate_argument(arguments[0]);
}

void JZ_INSTRUCTION(struct argument* arguments, int instruction) {
	if (cflags.zero) registers[I_REG_IP] = evaluate_argument(arguments[0]);
}

void JNZ_INSTRUCTION(struct argument* arguments, int instruction) {
	if (!cflags.zero) registers[I_REG_IP] = evaluate_argument(arguments[0]);
}

void PUSH_INSTRUCTION(struct argument* arguments, int instruction) {
	stack_push(evaluate_argument(arguments[0]));
}

void POP_INSTRUCTION(struct argument* arguments, int instruction) {
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
	if ((ISA[instruction].argc == 2 || (instruction >= I_MOV_INSTRUCTION && instruction < I_CMP_INSTRUCTION))  && instruction != I_SIVTE_INSTRUCTION && instruction != I_CMP_INSTRUCTION)
		do_expression_dependent_instruction(instruction, arguments);
	else
		(*evaluated_instructions[instruction])(arguments, instruction);
}

void* proccess_cycle(void* arg) {
	while (emulator_running) {
		if (registers[I_REG_IP] >= sizeof(memory))
			registers[I_REG_IP] = 0x0;

		struct argument arguments[256];
		load_arguments(0, arguments);
		int instruction = fetch_instruction();

		process_instruction(instruction, arguments);
	}
}

void init_emulator() {
	init_interrupts();

	evaluated_instructions[I_NOP_INSTRUCTION]   = NOP_INSTRUCTION;
	evaluated_instructions[I_NOT_INSTRUCTION]   = NOT_INSTRUCTION;
	evaluated_instructions[I_INT_INSTRUCTION]   = INT_INSTRUCTION;
	evaluated_instructions[I_SIVTE_INSTRUCTION] = SIVTE_INSTRUCTION;
	evaluated_instructions[I_CMP_INSTRUCTION]   = CMP_INSTRUCTION;
	evaluated_instructions[I_CMPR_INSTRUCTION]  = CMP_INSTRUCTION;
	evaluated_instructions[I_JMP_INSTRUCTION]   = JMP_INSTRUCTION;
	evaluated_instructions[I_CALL_INSTRUCTION]  = CALL_INSTRUCTION;
	evaluated_instructions[I_RET_INSTRUCTION]   = RET_INSTRUCTION;
	evaluated_instructions[I_JE_INSTRUCTION]    = JE_INSTRUCTION;
	evaluated_instructions[I_JNE_INSTRUCTION]   = JNE_INSTRUCTION;
	evaluated_instructions[I_JG_INSTRUCTION]    = JG_INSTRUCTION;
	evaluated_instructions[I_JGE_INSTRUCTION]   = JGE_INSTRUCTION;
	evaluated_instructions[I_JL_INSTRUCTION]    = JL_INSTRUCTION;
	evaluated_instructions[I_JLE_INSTRUCTION]   = JLE_INSTRUCTION;
	evaluated_instructions[I_JC_INSTRUCTION]    = JC_INSTRUCTION;
	evaluated_instructions[I_JNC_INSTRUCTION]   = JNC_INSTRUCTION;
	evaluated_instructions[I_JZ_INSTRUCTION]    = JZ_INSTRUCTION;
	evaluated_instructions[I_JNZ_INSTRUCTION]   = JNZ_INSTRUCTION;
	evaluated_instructions[I_PUSH_INSTRUCTION]  = PUSH_INSTRUCTION;
	evaluated_instructions[I_POP_INSTRUCTION]   = POP_INSTRUCTION;
}