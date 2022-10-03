#include <emulator.h>
#include <interrupts.h>

uint8_t memory[UINT16_MAX];
uint32_t ports[16];
uint8_t emulator_running = 1;
pthread_t process_thread;

uint8_t _disassemble = 0;

#define EXPRESSION_DEPENDENT_BACKEND(instruction, value1, value2) \
		        if (instruction == V3_I_MOV_INSTRUCTION) value1 =   value2; \ 
		   else if (instruction == V3_I_ADD_INSTRUCTION) value1 +=  value2; \ 
		   else if (instruction == V3_I_SUB_INSTRUCTION) value1 -=  value2; \ 
		   else if (instruction == V3_I_MUL_INSTRUCTION) value1 *=  value2; \ 
		   else if (instruction == V3_I_DIV_INSTRUCTION) value1 /=  value2; \ 
		   else if (instruction == V3_I_AND_INSTRUCTION) value1 &=  value2; \ 
		   else if (instruction == V3_I_OR_INSTRUCTION)  value1 |=  value2; \ 
		   else if (instruction == V3_I_SHR_INSTRUCTION) value1 >>= value2; \ 
		   else if (instruction == V3_I_SHL_INSTRUCTION) value1 <<= value2;
		//    else if (instruction == I_XOR_INSTRUCTION) value1 ^=  value2; \ 

void(*evaluated_instructions[V3_I_INSTRUCTION_MAX])(struct argument* arguments, int instruction);

uint32_t registers[V3_I_REG_MAX];
struct flags cflags;

int fetch_instruction() {
	if (memory[registers[V3_I_REG_IP]] >= V3_I_INSTRUCTION_MAX) {
		printf("Invalid opcode %X at %X\n", memory[registers[V3_I_REG_IP]], registers[V3_I_REG_IP]);
		exit(1);
	}

	return memory[registers[V3_I_REG_IP]++];
}

int next_byte() {
	return memory[registers[V3_I_REG_IP]++];
}

// Return a list of arguments based on V3_ISA[instruction]
void load_arguments(int instruction, int information, struct argument* argument_list) {
	for (int i = 0; i < V3_ISA[instruction].argc; i++) {
		argument_list[i].info_byte = information;
		argument_list[i].length = ((information >> (4 * i)) >> 2) & 0b11;
		argument_list[i].type = ((information >> (4 * i))) & 0b11;

		argument_list[i].value = 0;
		for (int j = 0; j < argument_list[i].length + 1; j++)
			argument_list[i].value |= (next_byte() << (j * 8));
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
	memory[registers[V3_I_REG_SP]++] = v	 & 0xFF;
	memory[registers[V3_I_REG_SP]++] = v >> 8   & 0xFF;
	memory[registers[V3_I_REG_SP]++] = v >> 16  & 0xFF;
	memory[registers[V3_I_REG_SP]++] = v >> 24  & 0xFF;
}

uint32_t stack_pop() {
	registers[V3_I_REG_SP] -= 4;

	uint32_t result = memory[registers[V3_I_REG_SP]] | memory[registers[V3_I_REG_SP] + 1] << 8 | memory[registers[V3_I_REG_SP] + 2] << 16 |memory[registers[V3_I_REG_SP] + 3] << 24;

	memory[registers[V3_I_REG_SP]] = 0;
	memory[registers[V3_I_REG_SP] + 1] = 0;
	memory[registers[V3_I_REG_SP] + 2] = 0;
	memory[registers[V3_I_REG_SP] + 3] = 0;

	return result;
}

void V3_NOP_INSTRUCTION(struct argument* arguments, int instruction) { }

void V3_NOT_INSTRUCTION(struct argument* arguments, int instruction) {
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

void V3_INT_INSTRUCTION(struct argument* arguments, int instruction) {
	// Call interrupt wrapper
	call_interrupt(evaluate_argument(arguments[0]));
}

void V3_SIVTE_INSTRUCTION(struct argument* arguments, int instruction) {
	set_interrupt(evaluate_argument(arguments[0]), evaluate_argument(arguments[1]));
}

void V3_CMP_INSTRUCTION(struct argument* arguments, int instruction) {
	uint32_t a = 0;
	uint32_t b = 0;

	a = evaluate_argument(arguments[0]);
	b = evaluate_argument(arguments[1]);

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

void V3_JMP_INSTRUCTION(struct argument* arguments, int instruction) {
	registers[V3_I_REG_IP] = evaluate_argument(arguments[0]);
}

void V3_CALL_INSTRUCTION(struct argument* arguments, int instruction) {
	stack_push(registers[V3_I_REG_IP]);
	registers[V3_I_REG_IP] = evaluate_argument(arguments[0]);
}

void V3_RET_INSTRUCTION(struct argument* arguments, int instruction) {
	registers[V3_I_REG_IP] = stack_pop();
}

void V3_JE_INSTRUCTION(struct argument* arguments, int instruction) {
	if (!cflags.carry && !cflags.zero) {
		registers[V3_I_REG_IP] = evaluate_argument(arguments[0]);
	}
}

void V3_JNE_INSTRUCTION(struct argument* arguments, int instruction) {
	if (cflags.carry || cflags.zero) registers[V3_I_REG_IP] = evaluate_argument(arguments[0]);
}

void V3_JG_INSTRUCTION(struct argument* arguments, int instruction) {
	if (cflags.carry && !cflags.zero) registers[V3_I_REG_IP] = evaluate_argument(arguments[0]);
}

void V3_JGE_INSTRUCTION(struct argument* arguments, int instruction) {
	if ((!cflags.carry && !cflags.zero) || (cflags.carry && !cflags.zero)) registers[V3_I_REG_IP] = evaluate_argument(arguments[0]);
}

void V3_JL_INSTRUCTION(struct argument* arguments, int instruction) {
	if (!cflags.carry && cflags.zero) registers[V3_I_REG_IP] = evaluate_argument(arguments[0]);
}

void V3_JLE_INSTRUCTION(struct argument* arguments, int instruction) {
	if ((!cflags.carry && !cflags.zero) || (!cflags.carry && cflags.zero)) registers[V3_I_REG_IP] = evaluate_argument(arguments[0]);
}

void V3_JC_INSTRUCTION(struct argument* arguments, int instruction) {
	if (cflags.carry) registers[V3_I_REG_IP] = evaluate_argument(arguments[0]);
}

void V3_JNC_INSTRUCTION(struct argument* arguments, int instruction) {
	if (!cflags.carry) registers[V3_I_REG_IP] = evaluate_argument(arguments[0]);
}

void V3_JZ_INSTRUCTION(struct argument* arguments, int instruction) {
	if (cflags.zero) registers[V3_I_REG_IP] = evaluate_argument(arguments[0]);
}

void V3_JNZ_INSTRUCTION(struct argument* arguments, int instruction) {
	if (!cflags.zero) registers[V3_I_REG_IP] = evaluate_argument(arguments[0]);
}

void V3_PUSH_INSTRUCTION(struct argument* arguments, int instruction) {
	stack_push(evaluate_argument(arguments[0]));
}

void V3_POP_INSTRUCTION(struct argument* arguments, int instruction) {
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

void V3_RDRP_INSTRUCTION(struct argument* arguments, int instruction) {
	registers[arguments[1].value] = ports[arguments[0].value];
}

void V3_WRRP_INSTRUCTION(struct argument* arguments, int instruction) {
	ports[arguments[0].value] = registers[arguments[1].value];
}

void process_instruction(int instruction, struct argument* arguments) {
	if (V3_ISA[instruction].argc == 2 && instruction > 0 && instruction < V3_I_CMP_INSTRUCTION)
		do_expression_dependent_instruction(instruction, arguments);
	else
		(*evaluated_instructions[instruction])(arguments, instruction);
}

void* proccess_cycle(void* arg) {
	if (!_disassemble) {
		while (emulator_running) {
			if (registers[V3_I_REG_IP] >= sizeof(memory))
				registers[V3_I_REG_IP] = 0x0;

			int instruction = fetch_instruction();
			struct argument arguments[256];
			int information = -1;

			if (V3_ISA[instruction].argc > ZERO_ARGUMENTS) {
				information = next_byte();
				load_arguments(instruction, information, arguments);
			}
			
			process_instruction(instruction, arguments);

			usleep(100);
		}
	} else {
		char* string = malloc(256);
		char* current = malloc(25);

		while (registers[V3_I_REG_IP] != _file_size) {
			int instruction = fetch_instruction();
			printf("%02X ", instruction);

			sprintf(current, "%s ", V3_ISA[instruction].name);
			strcat(string, current);

			if (V3_ISA[instruction].argc > ZERO_ARGUMENTS) {
				struct argument arguments[256];
				int information = next_byte();

				printf("%02X ", information);

				load_arguments(instruction, information, arguments);

				for (int i = 0; i < V3_ISA[instruction].argc; i++) {
					printf("%08X ", arguments[i].value);
					
					switch (arguments[i].type) {
					case CODE_RVALUE:
						sprintf(current, "0x%X%c ", arguments[i].value, ((V3_ISA[instruction].argc == TWO_ARGUMENTS && i == 0) ? ',' : 0));
						break;
					case CODE_PVALUE:
						sprintf(current, "[0x%X]%c ", arguments[i].value, ((V3_ISA[instruction].argc == TWO_ARGUMENTS && i == 0) ? ',' : 0));
						break;
					case CODE_RREG:
						sprintf(current, "%s%c ", V3_REGISTERS[arguments[i].value], ((V3_ISA[instruction].argc == TWO_ARGUMENTS && i == 0) ? ',' : 0));
						break;
					case CODE_PREG:
						sprintf(current, "[%s]%c ", V3_REGISTERS[arguments[i].value], ((V3_ISA[instruction].argc == TWO_ARGUMENTS && i == 0) ? ',' : 0));
						break;
					}

					strcat(string, current);
				}
			}
			
			printf("\t%c%C%c%s\n", (V3_ISA[instruction].argc < TWO_ARGUMENTS ? '\t' : 0), (V3_ISA[instruction].argc < TWO_ARGUMENTS ? '\t' : 0), (V3_ISA[instruction].argc == ZERO_ARGUMENTS ? '\t' : 0), string);

			memset(string, 0, 256);
		}
	}
}

void init_emulator() {
	init_interrupts();

	evaluated_instructions[V3_I_NOP_INSTRUCTION]   = V3_NOP_INSTRUCTION;
	evaluated_instructions[V3_I_NOT_INSTRUCTION]   = V3_NOT_INSTRUCTION;
	evaluated_instructions[V3_I_INT_INSTRUCTION]   = V3_INT_INSTRUCTION;
	evaluated_instructions[V3_I_SIVTE_INSTRUCTION] = V3_SIVTE_INSTRUCTION;
	evaluated_instructions[V3_I_CMP_INSTRUCTION]   = V3_CMP_INSTRUCTION;
	evaluated_instructions[V3_I_JMP_INSTRUCTION]   = V3_JMP_INSTRUCTION;
	evaluated_instructions[V3_I_CALL_INSTRUCTION]  = V3_CALL_INSTRUCTION;
	evaluated_instructions[V3_I_RET_INSTRUCTION]   = V3_RET_INSTRUCTION;
	evaluated_instructions[V3_I_JE_INSTRUCTION]    = V3_JE_INSTRUCTION;
	evaluated_instructions[V3_I_JNE_INSTRUCTION]   = V3_JNE_INSTRUCTION;
	evaluated_instructions[V3_I_JG_INSTRUCTION]    = V3_JG_INSTRUCTION;
	evaluated_instructions[V3_I_JGE_INSTRUCTION]   = V3_JGE_INSTRUCTION;
	evaluated_instructions[V3_I_JL_INSTRUCTION]    = V3_JL_INSTRUCTION;
	evaluated_instructions[V3_I_JLE_INSTRUCTION]   = V3_JLE_INSTRUCTION;
	evaluated_instructions[V3_I_JC_INSTRUCTION]    = V3_JC_INSTRUCTION;
	evaluated_instructions[V3_I_JNC_INSTRUCTION]   = V3_JNC_INSTRUCTION;
	evaluated_instructions[V3_I_JZ_INSTRUCTION]    = V3_JZ_INSTRUCTION;
	evaluated_instructions[V3_I_JNZ_INSTRUCTION]   = V3_JNZ_INSTRUCTION;
	evaluated_instructions[V3_I_PUSH_INSTRUCTION]  = V3_PUSH_INSTRUCTION;
	evaluated_instructions[V3_I_POP_INSTRUCTION]   = V3_POP_INSTRUCTION;
	evaluated_instructions[V3_I_RDPR_INSTRUCTION]  = V3_RDRP_INSTRUCTION;
	evaluated_instructions[V3_I_WRPR_INSTRUCTION]  = V3_WRRP_INSTRUCTION;
}