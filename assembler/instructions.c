#include <instructions.h>
#include <writer.h>

void(*instruction_list[ARGUMENTS_MAX])(struct token[], int*);

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

int get_arg(struct token tokens[], int* index, int* arg_info, int* size_override){
	int type_arg = 0;
	int offset_value = 0;
	
	switch (tokens[*index].type){
	case T_REGISTER: {
		debug("FOUND REGISTER");
		
		*arg_info = CODE_RREG;
		return tokens[(*index)++].value;
	}

	case T_LSQR_BRACKET: {
		debug("FOUND REFERENCE TO ADDRESS");

		int index_ = *index + 1;
		int value = get_arg(tokens, &index_, arg_info, size_override);
		*arg_info = *arg_info == CODE_RREG ? CODE_PREG : CODE_PVALUE;

		*index = index_ + 1; // Jump over closing ], revise later to check for ] and to check for :

		int offset_info = 0;
		int offset = get_offset(tokens, index, &offset_info);

		return value;
	}

	case T_INT: {
		debug("FOUND INTEGER LITERAL");

		*arg_info = CODE_RVALUE;
		return tokens[(*index)++].value;
	}
	
	case T_DOT: {

	}

	case T_STRING: { // Label
		// A global label is found
		// Save the current label
		// Move the index forwards by 1
		// Recursively call get_arg();
		// Until a final address can be written

		debug("FOUND LABEL REFERENCE TO %s", tokens[*index].extra_bytes);

		*arg_info = CODE_RVALUE;

		struct label* found = find_label_from_name(tokens[*index].extra_bytes, _labels, true);

		if (found == NULL){
			// Not found
			*size_override = 3;

			insert_reference(&tokens[*index], get_write_position() + 1);

			(*index)++;			

			return 0;
		}

		// Was global label and immediately found
		(*index)++;

		*size_override = size_in_bytes(found->address);
		return found->address;

		// printf("%s\n", tokens[*index].extra_bytes);
		
		// struct label* result = NULL;
		// find_label_from_series(tokens, *index, result);

		// if (result == NULL){
		// 	// Label not found, add missing reference
		// } else {
		// 	return result->address;
		// }

		// Find global label
		// If found
		// 	Check for local label reference (repeat this until no more labels are found)
		// 	Return labels address (if local label return global label's address + local label's address)
		// If not found
		//	Note the current file location
		// 	Return -1 for label not found, and caller can interpret it as 32-bit 0

		break;
	}
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
void TWO_ARG_INSTRUCTION(struct token tokens[], int* i){
	// Write opcode
	write_byte(tokens[*i].value);

	int index = *i + 1;				  	   // Current token index
	
	// Argument 1
	int size_1 = get_size(tokens, &index); 		  	   // Check current token for size
	int arg_info_1 = 0;				  	   // Info parsed from arg
	int arg_1 = get_arg(tokens, &index, &arg_info_1, &size_1); // Get argument 1

	debug("size_1: %d, arg_info_1: %d, arg_1: %d", size_1, arg_info_1, arg_1);
	
	write_byte(to_information_byte(arg_1, arg_info_1, size_1));
	write(arg_1, size_1);

	assert_comma(tokens[index], &index);		  	   // Assert comma
		
	// Argument 2
	int size_2 = get_size(tokens, &index); 		  	   // Check current token for size
	int arg_info_2 = 0;				  	   // Info parsed from arg
	int arg_2 = get_arg(tokens, &index, &arg_info_2, &size_2); // Get argument 2
	
	debug("size_2: %d, arg_info_2: %d, arg_2: %d", size_2, arg_info_2, arg_2);

	write_byte(to_information_byte(arg_2, arg_info_2, size_2));
	write(arg_2, size_2);

	*i = index;
}

void ONE_ARG_INSTRUCTION(struct token tokens[], int* i){
	int index = *i + 1;
	int size = get_size(tokens, &index);
	int arg_info = 0;
	int arg = get_arg(tokens, &index, &arg_info, &size);

	debug("size: %d, arg_info: %d, arg: %d", size, arg_info, arg);

	// Write instruction code
	write_byte(tokens[*i].value);

	// Write argument
	write_byte(to_information_byte(arg, arg_info, size));
	write(arg, size);

	*i = index;
}

void ZERO_ARG_INSTRUCTION(struct token tokens[], int* i){
	write_byte(tokens[0].value);
	*i++;
}

void N_ARG_INSTRUCTION(struct token tokens[], int* i){
	switch (tokens[*i].value) {
	case I_DS_INSTRUCTION:
		(*i)++;
		
		do {
			write_bytes(tokens[*i].extra_bytes, strlen(tokens[*i].extra_bytes));
			(*i)++;
		} while (tokens[*i].type == T_COMMA);

		break;
	case I_DB_INSTRUCTION:
		(*i)++;
		
		do {
			write(tokens[*i].value, size_in_bytes(tokens[*i].value));
			(*i)++;
		} while (tokens[*i].type == T_COMMA);

		break;
	}
}
