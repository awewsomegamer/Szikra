#include <assembly.h>
#include <writer.h>
#include <message_handler.h>

int size_in_bytes(int argument){
	int i = 0;

	while (argument != 0){
		argument >>= 8;
		if (argument != 0) i++;
	}

	return i;
}

struct argument get_arg(struct token* tokens, int* i) {
	struct argument ret;

	switch (tokens[*i].type) {
	case T_INT: // Is integer
		ret.type = 0;
		ret.length = size_in_bytes(tokens[*i].value);
		ret.sign = tokens[*i].value < 0;
		
		(*i)++;

		return ret;

	case T_LSQR_BRACKET: // Is pointer
		int _i = *i + 1;
		struct argument value = get_arg(tokens, &_i);

		ret.type = value.type == CODE_RREG ? CODE_PREG : CODE_PVALUE;
		ret.length = size_in_bytes(value.value);
		ret.value = value.value;

		*i = _i + 1;

		if (tokens[*i].type != T_RSQR_BRACKET)
			error("Expected right square bracket on line %d", _line);
		
		break;

	case T_STRING: // Is reference
		ret.type = 0;
		
		// Find label
		// If found process as integer but with .address
		// If not found insert a new label with name, and add writer position to its references pointer

		break;

	case T_REGISTER: // Is register
		ret.type = CODE_RREG;
		ret.length = 0;
		ret.value = tokens[*i].value;

		break;
	}
}

void build_instruction(struct token* tokens, int size) {
	write_byte(tokens[0].value); // Write OPCODE

	int i = 1;
	
	while (i < size) {
		struct argument arg = get_arg(tokens, &i);

		
	}
}

void assemble(struct token* tokens, int size) {
	switch (tokens[0].type) {
	case T_INSTRUCTION:


		break;
	case T_DIRECTIVE:

		break;
	case T_STRING:
		// Find label
		// If label is found but address is -1, change its address to the writer position
		//				If address is >= 0 then error, most likely a redefinition

		break;
	
	}
}
