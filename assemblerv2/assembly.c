#include <assembly.h>
#include <writer.h>
#include <message_handler.h>
#include <util.h>

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
		struct label* label = find_label(tokens[*i].extra_bytes);

		if (label == NULL) {
			// Create new label
			create_label(tokens[*i].extra_bytes, 0, 0);

			label = &_labels[_label_count - 1];

			// Insert reference
			label->reference_count = 1;
			label->references = realloc(label->references, label->reference_count);
			label->references[0] = get_writer_position() + 1;
			
			// Return 32 bit 0
			ret.value = 0;
			ret.type = CODE_RVALUE;
			ret.length = 3;

			(*i)++;

			return ret;
		} else if (!label->defined) {
			label->reference_count++;
			label->references = realloc(label->references, label->reference_count);
			label->references[label->reference_count - 1] = get_writer_position() + 1;

			ret.value = 0;
			ret.type = CODE_RVALUE;
			ret.length = 3;

			(*i)++;

			return ret;
		}

		ret.value = label->address;
		ret.length = size_in_bytes(label->address);
		ret.type = CODE_RREG;

		(*i)++;

		return ret;

	case T_REGISTER: // Is register
		ret.type = CODE_RREG;
		ret.length = 0;
		ret.value = tokens[*i].value;

		(*i)++;

		return ret;
	
	case T_COMMA:
		int i2 = *i + 1;
		ret = get_arg(tokens, &i2);
		*i = i2;

		return ret;
	}
}

// General instruction for building most instructions
void build_instruction(struct token* tokens, int size) {
	// Write opcode
	write_byte(tokens[0].value);

	int i = 1;
	
	// While there are more arguments, write them
	do {
		struct argument arg = get_arg(tokens, &i);
		
		uint8_t info_byte = (arg.type << 6) | (arg.length << 4) | (arg.cast << 2) | (arg.offset << 1) | (arg.sign);
		write_byte(info_byte);

		for (int j = 0; j < arg.length + 1; j++)
			write_byte((arg.value >> (j * 8)) & 0xFF);

	} while (tokens[i].type == T_COMMA);

	// If tokens are still left, call assemble function
	if (i < size)
		assemble(tokens + i, size - i);
}

void do_directive(struct token* tokens, int size) {
	switch (hash_string(tokens[1].extra_bytes)) {
	case AT_DIRECTIVE_HASH:
		if (tokens[2].type == T_INT) {
			set_writer_position(tokens[2].value);
		} else if (tokens[2].type == T_STRING) {
			// struct label* label = find_label(tokens[2].extra_bytes);
		} else {
			error("Improper directive on line %d", _line);
		}
		
		break;
	case DATA_DIRECTIVE_HASH:
		break;
	case TEXT_DIRECTIVE_HASH:
		break;
	}
}

void assemble(struct token* tokens, int size) {
	debug("ASSEMBLING TOKENS (%d): \n", _line);
	for (int i = 0; i < size; i++)
		debug("%s (%d, %s) ", TOKEN_NAMES[tokens[i].type], tokens[i].type, tokens[i].extra_bytes);

	switch (tokens[0].type) {
	case T_INSTRUCTION:
		build_instruction(tokens, size);

		break;

	case T_DIRECTIVE:
		do_directive(tokens, size + 1);

		break;

	case T_STRING:
		if (tokens[1].type == T_COLON) {
			struct label* label = find_label(tokens[0].extra_bytes);

			if (label == NULL) {
				create_label(tokens[0].extra_bytes, get_writer_position(), 1);

				if (2 < size)
					assemble(tokens + 2, size);

				break;
			} else if (!label->defined) {
				label->address = get_writer_position();
				label->defined = 1;

				if (2 < size)
					assemble(tokens + 2, size);

				break;
			}
			
			error("Label %s, originally defined at line %d, redefined at %d", label->name, label->line, _line);
		} else {
			error("Unexpected string found on line %d", _line);
		}

		break;
	
	}
}
