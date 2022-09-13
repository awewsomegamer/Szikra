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
	memset(&ret, 0, sizeof(struct argument));

	switch (tokens[*i].type) {
	case T_INT: // Is integer
		ret.type = CODE_RVALUE;
		ret.length = size_in_bytes(tokens[*i].value);
		ret.sign = tokens[*i].value < 0;
		ret.value = tokens[*i].value;
		
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
		
		return ret;

	case T_STRING: // Is reference
		ret.type = 0;
		
		// Find label
		// If found process as integer but with .address
		// If not found insert a new label with name, and add writer position to its references pointer

		(*i)++;

		return ret;

	case T_REGISTER: // Is register
		ret.type = CODE_RREG;
		ret.length = 0;
		ret.value = tokens[*i].value;

		(*i)++;

		return ret;
	}
}

void build_instruction(struct token* tokens, int size) {
	write_byte(tokens[0].value); // Write OPCODE

	int i = 1;
	
	while (i < size) {
		struct argument arg = get_arg(tokens, &i);
		
		if (tokens[i].type != T_COMMA && i != size - 1 && ISA[tokens[0].value].argc > ONE_ARGUMENT)
			error("Expected comma line %d", _line);
		
		uint8_t info_byte = (arg.type << 6) | (arg.length << 4) | (arg.cast << 2) | (arg.offset << 1) | (arg.sign);
		write_byte(info_byte);

		for (int j = 0; j < size_in_bytes(arg.value) + 1; j++)
			write_byte((arg.value >> (j * 8)) & 0xFF);	

		i++;
	}
}

void assemble(struct token* tokens, int size) {
	switch (tokens[0].type) {
	case T_INSTRUCTION:
		build_instruction(tokens, size);

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
