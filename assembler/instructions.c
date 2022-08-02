#include <instructions.h>

void(*instruction_list[ARGUMENTS_MAX])(struct token[]);

void init_instructions(){
	instruction_list[TWO_ARGUMENTS] = TWO_ARG_INSTRUCTION;
	instruction_list[ONE_ARGUMENT] = ONE_ARG_INSTRUCTION;
	instruction_list[ZERO_ARGUMENTS] = ZERO_ARG_INSTRUCTION;
	instruction_list[N_ARGUMENTS] = N_ARG_INSTRUCTION;
}

// Syntax functions
int get_size(struct token tokens[], int* index){
	if (tokens[*index].type == T_SIZE)
		return tokens[(*index)++].value;
	
	return 0;
}

// Check for:
// Register - ax (T_REGISTER)
// Register ptr - [ax] (T_LSQR, T_REGISTER, T_RSQR)
// Value - 123, 0x55, 0b11 (T_INT)
// Value ptr - [123], [0x55], [0b11] (T_LSQR, T_INT, T_RSQR)

int get_arg(struct token tokens[], int* index, int* arg_info){
	int type_arg = 0;
	int offset_value = 0;
	
	switch (tokens[*index].type){
	case T_REGISTER:
		debug("FOUND REGISTER");
		
		*arg_info = CODE_RREG;
		return tokens[(*index)++].value;

	case T_LSQR_BRACKET:
		debug("FOUND REFERENCE TO ADDRESS");

		int index_ = *index + 1;
		int value = get_arg(tokens, &index_, arg_info);
		*arg_info = *arg_info == CODE_RREG ? CODE_PREG : CODE_PVALUE;

		*index += 2; // Jump over closing ], revise later to check for ] and to check for :

		return value;

	case T_INT:
		debug("FOUND INTEGER LITERAL");

		*arg_info = CODE_RVALUE;
		return tokens[(*index)++].value;
	
	case T_STRING: // Label
		debug("FOUND LABEL REFERENCE");

		*arg_info = CODE_RVALUE;


		// Find global label
		// If found
		// 	Check for local label reference (repeat this until no more labels are found)
		// 	Return labels address (if local label return global label's address + local label's address)
		// If not found
		//	Note the current file location
		// 	Return -1 for label not found, and caller can interpret it as 32-bit 0

		break;
	}

	return -1;
}

void assert_comma(struct token t, int* index){
	if (t.type != T_COMMA){
		fatal_error("Expected comma");
		return;
	}

	(*index)++;
}

// mov <size> <ax> <,> <size> <bx>
void TWO_ARG_INSTRUCTION(struct token tokens[]){
	int index = 1;					  // Current token index

	int size_1 = get_size(tokens, &index); 		  // Check current token for size
	int arg_info_1 = 0;				  // Info parsed from arg
	int arg_1 = get_arg(tokens, &index, &arg_info_1); // Get argument 1

	debug("size_1: %d, arg_info_1: %d, arg_1: %d", size_1, arg_info_1, arg_1);

	assert_comma(tokens[index], &index);		  // 

	int size_2 = get_size(tokens, &index); 		  // Check current token for size
	int arg_info_2 = 0;				  // Info parsed from arg
	int arg_2 = get_arg(tokens, &index, &arg_info_2); // Get argument 2
	
	debug("size_2: %d, arg_info_2: %d, arg_2: %d", size_2, arg_info_2, arg_2);

	// Write
}

void ONE_ARG_INSTRUCTION(struct token tokens[]){
	int index = 1;
	int size = get_size(tokens, &index);
	int arg_info = 0;
	int arg = get_arg(tokens, &index, &arg_info);

	debug("size: %d, arg_info: %d, arg: %d", size, arg_info, arg);

	// Write
}

void ZERO_ARG_INSTRUCTION(struct token tokens[]){
	// Write
}

void N_ARG_INSTRUCTION(struct token tokens[]){
	
}
