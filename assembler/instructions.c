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
	if (tokens[*index].type == T_SIZE){
		printf("Found size\n");

		(*index)++;
		return tokens[*index].value;
	}
	
	return 0;
}

int get_arg(struct token tokens[], int* index, int* arg_info){


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

	int size_1 = get_size(tokens, &index); 		  // Check current token for size
	// int arg_info_1 = 0;				  // Info parsed from arg
	// int arg_1 = get_arg(tokens, &index, &arg_info_1); // Get argument 1

	// int size_2 = get_size(tokens, &index); 		  // Check current token for size
	// int arg_info_2 = 0;				  // Info parsed from arg
	// int arg_2 = get_arg(tokens, &index, &arg_info_1); // Get argument 2

	printf("size 1: %d (@%d)\n", size_1, index);

	// Write
}

void ONE_ARG_INSTRUCTION(struct token tokens[]){

}

void ZERO_ARG_INSTRUCTION(struct token tokens[]){

}

void N_ARG_INSTRUCTION(struct token tokens[]){

}
