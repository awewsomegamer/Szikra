#include <v3processor.h>

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

static void(*evaluated_instructions[V3_I_INSTRUCTION_MAX])(struct argument* arguments, int instruction);

static uint32_t v3_registers[V3_I_REG_MAX];
static struct flags v3_cflags;

static int v3_fetch_instruction() {
	if (memory[v3_registers[V3_I_REG_IP]] >= V3_I_INSTRUCTION_MAX) {
		printf("Invalid opcode %X at %X V3\n", memory[v3_registers[V3_I_REG_IP]], v3_registers[V3_I_REG_IP]);
		for (;;);
	}

	return memory[v3_registers[V3_I_REG_IP]++];
}

static int v3_next_byte() {
	return memory[v3_registers[V3_I_REG_IP]++];
}

// Return a list of arguments based on ISA[instruction]
static void v3_load_arguments(int instruction, int information, struct argument* argument_list) {
	for (int i = 0; V3_ISA[instruction].argc; i++) {
		argument_list[i].info_byte = information;
		argument_list[i].length = ((instruction >> (4 * i)) >> 2) & 0b11;
		argument_list[i].type = ((instruction >> (4 * i))) & 0b11;

		argument_list[i].value = 0;

		v3_registers[V3_I_REG_IP]++;
		for (int j = 0; j < argument_list[i].length + 1; j++)
			argument_list[i].value |= (v3_next_byte(V3_I_REG_IP) << (j * 8));
	}
}

// Instructions such as MOV need the full context of each argument
// to be fully able to preform its task.
// For this purpose there are two categories of instructions: expression dependent, and evaluated
// Where expression dependent are things like MOV, and evaluated are things like CMP
static void v3_do_expression_dependent_instruction(int instruction, struct argument* arguments) {
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
		EXPRESSION_DEPENDENT_BACKEND(instruction, v3_registers[arguments[0].value], v3_registers[arguments[1].value]);
		return;
	}


	// A [AX], BX
	if (t1 == CODE_PREG && t2 == CODE_RREG) {
		EXPRESSION_DEPENDENT_BACKEND(instruction, memory[v3_registers[arguments[0].value]], v3_registers[arguments[1].value]);
		return;
	}

	// A AX, [BX]
	if (t1 == CODE_RREG && t2 == CODE_PREG) {
		EXPRESSION_DEPENDENT_BACKEND(instruction, v3_registers[arguments[0].value], memory[v3_registers[arguments[1].value]]);
		return;
	}

	// A AX, C
	if (t1 == CODE_RREG && t2 == CODE_RVALUE) {
		EXPRESSION_DEPENDENT_BACKEND(instruction, v3_registers[arguments[0].value], arguments[1].value);
		return;
	}

	// A [AX], [BX]
	if (t1 == CODE_PREG && t2 == CODE_PREG) {
		EXPRESSION_DEPENDENT_BACKEND(instruction, memory[v3_registers[arguments[0].value]], memory[v3_registers[arguments[1].value]]);
		return;
	}

	// A [AX], C
	if (t1 == CODE_PREG && t2 == CODE_RVALUE) {
		EXPRESSION_DEPENDENT_BACKEND(instruction, memory[v3_registers[arguments[0].value]], arguments[1].value);
		return;
	}

	// A [AX], [C]
	if (t1 == CODE_PREG && t2 == CODE_PVALUE) {
		EXPRESSION_DEPENDENT_BACKEND(instruction, memory[v3_registers[arguments[0].value]], memory[arguments[1].value]);
		return;
	}

	// A [C], [AX]
	if (t1 == CODE_PVALUE && t2 == CODE_RREG) {
		EXPRESSION_DEPENDENT_BACKEND(instruction, memory[arguments[0].value], memory[v3_registers[arguments[1].value]]);
		return;
	}

	// A C, AX
	// A [C], AX
	if ((t1 == CODE_RVALUE && t2 == CODE_RREG) || (t1 == CODE_PVALUE && t2 == CODE_RREG)) {
		EXPRESSION_DEPENDENT_BACKEND(instruction, memory[arguments[0].value], v3_registers[arguments[1].value]);
		return;
	}
}

int v3_evaluate_argument(struct argument arg) {
	switch (arg.type) {
	case CODE_RVALUE:
		return arg.value;
	case CODE_PVALUE:
		return memory[arg.value];
	case CODE_RREG:
		return v3_registers[arg.value];
	case CODE_PREG:
		return memory[v3_registers[arg.value]];
	}
}

static void v3_stack_push(uint32_t v) {
	memory[v3_registers[V3_I_REG_SP]++] = v	 & 0xFF;
	memory[v3_registers[V3_I_REG_SP]++] = v >> 8   & 0xFF;
	memory[v3_registers[V3_I_REG_SP]++] = v >> 16  & 0xFF;
	memory[v3_registers[V3_I_REG_SP]++] = v >> 24  & 0xFF;
}

static uint32_t v3_stack_pop() {
	v3_registers[I_REG_SP] -= 4;

	uint32_t result = memory[v3_registers[V3_I_REG_SP]] | memory[v3_registers[V3_I_REG_SP] + 1] << 8 | memory[v3_registers[V3_I_REG_SP] + 2] << 16 |memory[v3_registers[V3_I_REG_SP] + 3] << 24;

	memory[v3_registers[V3_I_REG_SP]] = 0;
	memory[v3_registers[V3_I_REG_SP] + 1] = 0;
	memory[v3_registers[V3_I_REG_SP] + 2] = 0;
	memory[v3_registers[V3_I_REG_SP] + 3] = 0;

	return result;
}

static void V3_NOP_INSTRUCTION(struct argument* arguments, int instruction) { }

static void V3_NOT_INSTRUCTION(struct argument* arguments, int instruction) {
	switch (arguments[0].type) {
	case CODE_RVALUE:
	case CODE_PVALUE:
		memory[arguments[0].value] = ~memory[arguments[0].value];
		break;

	case CODE_RREG:
		v3_registers[arguments[0].value] = ~v3_registers[arguments[0].value];
		break;

	case CODE_PREG:
		memory[v3_registers[arguments[0].value]] = ~memory[v3_registers[arguments[0].value]];
		break;
	}
}

static void V3_INT_INSTRUCTION(struct argument* arguments, int instruction) {
	// Call interrupt wrapper
	call_interrupt(v3_evaluate_argument(arguments[0]));
}

static void V3_SIVTE_INSTRUCTION(struct argument* arguments, int instruction) {
	// Call interrupt set instruction
}

static void IV3_RET_INSTRUCTION(struct argument* arguments, int instruction) {
	// RET with a few extra steps, depends on what is done before
	// interrupt wrapper is called in V3_INT_INSTRUCTION
}

static void V3_CMP_INSTRUCTION(struct argument* arguments, int instruction) {
	uint32_t a = 0;
	uint32_t b = 0;

	a = v3_evaluate_argument(arguments[0]);
	b = v3_evaluate_argument(arguments[1]);

	if (a > b) {
		v3_cflags.carry = 1;
		v3_cflags.zero = 0;
	} else if (a == b) {
		v3_cflags.carry = 0;
		v3_cflags.zero = 0;
	} else if (a < b) {
		v3_cflags.carry = 0;
		v3_cflags.zero = 1;
	}
}

static void V3_JMP_INSTRUCTION(struct argument* arguments, int instruction) {
	v3_registers[I_REG_IP] = v3_evaluate_argument(arguments[0]);
}

static void V3_CALL_INSTRUCTION(struct argument* arguments, int instruction) {
	v3_stack_push(v3_registers[I_REG_IP]);
	v3_registers[I_REG_IP] = v3_evaluate_argument(arguments[0]);
}

static void V3_RET_INSTRUCTION(struct argument* arguments, int instruction) {
	v3_registers[I_REG_IP] = v3_stack_pop();
}

static void V3_JE_INSTRUCTION(struct argument* arguments, int instruction) {
	if (!v3_cflags.carry && !v3_cflags.zero) {
		v3_registers[I_REG_IP] = v3_evaluate_argument(arguments[0]);
	}
}

static void V3_JNE_INSTRUCTION(struct argument* arguments, int instruction) {
	if (v3_cflags.carry || v3_cflags.zero) v3_registers[I_REG_IP] = v3_evaluate_argument(arguments[0]);
}

static void V3_JG_INSTRUCTION(struct argument* arguments, int instruction) {
	if (v3_cflags.carry && !v3_cflags.zero) v3_registers[I_REG_IP] = v3_evaluate_argument(arguments[0]);
}

static void V3_JGE_INSTRUCTION(struct argument* arguments, int instruction) {
	if ((!v3_cflags.carry && !v3_cflags.zero) || (v3_cflags.carry && !v3_cflags.zero)) v3_registers[I_REG_IP] = v3_evaluate_argument(arguments[0]);
}

static void V3_JL_INSTRUCTION(struct argument* arguments, int instruction) {
	if (!v3_cflags.carry && v3_cflags.zero) v3_registers[I_REG_IP] = v3_evaluate_argument(arguments[0]);
}

static void V3_JLE_INSTRUCTION(struct argument* arguments, int instruction) {
	if ((!v3_cflags.carry && !v3_cflags.zero) || (!v3_cflags.carry && v3_cflags.zero)) v3_registers[I_REG_IP] = v3_evaluate_argument(arguments[0]);
}

static void V3_JC_INSTRUCTION(struct argument* arguments, int instruction) {
	if (v3_cflags.carry) v3_registers[I_REG_IP] = v3_evaluate_argument(arguments[0]);
}

static void V3_JNC_INSTRUCTION(struct argument* arguments, int instruction) {
	if (!v3_cflags.carry) v3_registers[I_REG_IP] = v3_evaluate_argument(arguments[0]);
}

static void V3_JZ_INSTRUCTION(struct argument* arguments, int instruction) {
	if (v3_cflags.zero) v3_registers[I_REG_IP] = v3_evaluate_argument(arguments[0]);
}

static void V3_JNZ_INSTRUCTION(struct argument* arguments, int instruction) {
	if (!v3_cflags.zero) v3_registers[I_REG_IP] = v3_evaluate_argument(arguments[0]);
}

static void V3_PUSH_INSTRUCTION(struct argument* arguments, int instruction) {
	v3_stack_push(v3_evaluate_argument(arguments[0]));
}

static void V3_POP_INSTRUCTION(struct argument* arguments, int instruction) {
	switch (arguments[0].type) {
	case CODE_RVALUE:
	case CODE_PVALUE:
		memory[arguments[0].value] = v3_stack_pop();
		break;
	case CODE_RREG:
		v3_registers[arguments[0].value] = v3_stack_pop();
		break;
	case CODE_PREG:
		memory[v3_registers[arguments[0].value]] = v3_stack_pop();
		break;
	}
}

static void v3_process_instruction(int instruction, struct argument* arguments) {
	if (ISA[instruction].argc == 2  && instruction != V3_I_SIVTE_INSTRUCTION && instruction != V3_I_CMP_INSTRUCTION)
		v3_do_expression_dependent_instruction(instruction, arguments);
	else
		(*evaluated_instructions[instruction])(arguments, instruction);
}

void v3_cycle() {
	if (v3_registers[I_REG_IP] >= sizeof(memory))
		v3_registers[I_REG_IP] = 0x0;

	int instruction = v3_fetch_instruction(V3_I_REG_IP);
	int information = v3_next_byte(V3_I_REG_IP);

	struct argument arguments[256];
	v3_load_arguments(instruction, information, arguments);

	v3_process_instruction(instruction, arguments);
}

uint32_t v3_get_register(int reg) {
	return v3_registers[reg];
}

void init_v3() {
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
}

