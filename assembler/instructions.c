#include <instructions.h>
#include <writer.h>

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

int get_offset(struct token tokens[], int* index, int* offset_info){

	return 0;
}

// Checks for:
// Register - ax (T_REGISTER)
// Register ptr - [ax] (T_LSQR, T_REGISTER, T_RSQR)
// Value - 123, 0x55, 0b11 (T_INT)
// Value ptr - [123], [0x55], [0b11] (T_LSQR, T_INT, T_RSQR)
// Should also check for:
// Offsets - 


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

		*index = index_; // Jump over closing ], revise later to check for ] and to check for :

		int offset_info = 0;
		int offset = get_offset(tokens, index, &offset_info);

		return value;

	case T_INT:
		debug("FOUND INTEGER LITERAL");

		*arg_info = CODE_RVALUE;
		return tokens[(*index)++].value;
	
	case T_STRING: // Label
		debug("FOUND LABEL REFERENCE");

		*arg_info = CODE_RVALUE;
		
		printf("%s\n", tokens[*index].extra_bytes);

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

uint8_t to_information_byte(int argument, int argument_type, int cast){
	// 00 00 00 00
	// |  |  |  `---- Offset descriptor
	// |  |  `------- Argument size cast
	// |  `---------- Argument size
	// `------------- Argument type

	uint8_t byte = 0;

	byte |= (argument_type & 0b11) << 6;
	byte |= ((size_in_bytes(argument)) & 0b11) << 4;
	byte |= (cast & 0b11) << 2;

	return byte;
}

// mov <size> <ax> <,> <size> <bx>
void TWO_ARG_INSTRUCTION(struct token tokens[]){
	int index = 1;					  // Current token index

	int size_1 = get_size(tokens, &index); 		  // Check current token for size
	int arg_info_1 = 0;				  // Info parsed from arg
	int arg_1 = get_arg(tokens, &index, &arg_info_1); // Get argument 1

	debug("size_1: %d, arg_info_1: %d, arg_1: %d", size_1, arg_info_1, arg_1);

	assert_comma(tokens[index], &index);		  // Assert comma

	int size_2 = get_size(tokens, &index); 		  // Check current token for size
	int arg_info_2 = 0;				  // Info parsed from arg
	int arg_2 = get_arg(tokens, &index, &arg_info_2); // Get argument 2
	
	debug("size_2: %d, arg_info_2: %d, arg_2: %d", size_2, arg_info_2, arg_2);

	// Write opcode
	write_byte(tokens[0].value);

	// Argument 1
	write_byte(to_information_byte(arg_1, arg_info_1, size_1));
	write_byte(arg_1);

	// Argument 2
	write_byte(to_information_byte(arg_2, arg_info_2, size_2));
	write_byte(arg_2);
}

void ONE_ARG_INSTRUCTION(struct token tokens[]){
	int index = 1;
	int size = get_size(tokens, &index);
	int arg_info = 0;
	int arg = get_arg(tokens, &index, &arg_info);

	debug("size: %d, arg_info: %d, arg: %d", size, arg_info, arg);

	// Write instruction code
	write_byte(tokens[0].value);

	// Write argument
	write_byte(to_information_byte(arg, arg_info, size));
	write_byte(arg);
}

void ZERO_ARG_INSTRUCTION(struct token tokens[]){
	write_byte(tokens[0].value);
}

void N_ARG_INSTRUCTION(struct token tokens[]){

}
