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
		*arg_info = CODE_RREG;
		return tokens[*index].value;

	case T_LSQR_BRACKET:
		int index_ = *index + 1;
		int value = get_arg(tokens, &index_, arg_info);
		*arg_info = *arg_info == CODE_RREG ? CODE_PREG : CODE_PVALUE;
		return value;

	case T_INT:
		*arg_info = CODE_RVALUE;
		return tokens[*index].value;
	}



	return 0;
}

void assert_comma(struct token t, int* index){
	if (t.type != T_COMMA){
		fatal_error("Expected comma, line %d", _line);
		return;
	}

	(*index)++;
}

// mov <size> <ax> <,> <size> <bx>
void TWO_ARG_INSTRUCTION(struct token tokens[]){
	int index = 1;					  // Current token index

	// int size_1 = get_size(tokens, &index); 		  // Check current token for size
	// int arg_info_1 = 0;				  // Info parsed from arg
	// int arg_1 = get_arg(tokens, &index, &arg_info_1); // Get argument 1

	// assert_comma(tokens[index], &index);

	// int size_2 = get_size(tokens, &index); 		  // Check current token for size
	// int arg_info_2 = 0;				  // Info parsed from arg
	// int arg_2 = get_arg(tokens, &index, &arg_info_1); // Get argument 2

	// Write
}

void ONE_ARG_INSTRUCTION(struct token tokens[]){
	int index = 1;
	int size = get_size(tokens, &index);
	int arg_info = 0;
	int arg = get_arg(tokens, &index, &arg_info);

	printf("size: %d, arg_info: %d, arg: %d\n", size, arg_info, arg);
}

void ZERO_ARG_INSTRUCTION(struct token tokens[]){

}

void N_ARG_INSTRUCTION(struct token tokens[]){

}
