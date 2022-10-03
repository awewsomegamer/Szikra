#include <assembly.h>
#include <writer.h>
#include <message_handler.h>
#include <util.h>

uint32_t wrh_stack[1024];
uint32_t wrh_ptr = 0;

int _varient = 3;

struct argument get_arg(struct token* tokens, int* i) {
	struct argument ret;
	memset(&ret, 0, sizeof(struct argument));

	switch (tokens[*i].type) {
	case T_INT: // Is integer
		ret.type = CODE_RVALUE;
		ret.length = size_in_bytes(tokens[*i].value);
		ret.instruction = 1;
		ret.reference = 0;
		ret.value = tokens[*i].value;
		
		(*i)++;

		return ret;

	case T_LSQR_BRACKET: // Is pointer
		int _i = *i + 1;
		struct argument value = get_arg(tokens, &_i);

		ret.type = value.type == CODE_RREG ? CODE_PREG : CODE_PVALUE;
		ret.length = size_in_bytes(value.value);
		ret.value = value.value;
		ret.instruction = 1;
		ret.reference = value.reference;

		*i = _i;

		if (tokens[*i].type != T_RSQR_BRACKET)
			error("Expected right square bracket on line %d", _line);
		
		(*i)++;

		return ret;

	case T_STRING: // Is reference
		struct label* label = find_label(tokens[*i].extra_bytes);

		if (label == NULL) {
			// Create new label
			create_label(tokens[*i].extra_bytes, 0, 0);

			label = &_labels[_label_count - 1];
			
			// Return 32 bit 0
			ret.value = label->index;
			ret.type = CODE_RVALUE;
			ret.reference = 1;
			ret.instruction = 1;

			(*i)++;

			return ret;
		} else if (!label->defined) {
			ret.value = label->index;
			ret.type = CODE_RVALUE;
			ret.reference = 1;
			ret.instruction = 1;

			(*i)++;

			return ret;
		}

		ret.instruction = 1;
		ret.type = CODE_RVALUE;
		ret.value = label->index;
		ret.reference = 1;

		(*i)++;

		return ret;

	case T_REGISTER: // Is register
		ret.type = CODE_RREG;
		ret.length = 0;
		ret.value = tokens[*i].value;
		ret.instruction = 1;
		ret.reference = 0;

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
	int i = 1;

	// printf("%s\n", V3_ISA[tokens[0].value].name);

	switch (tokens[0].value) {
	case V3_I_DB_INSTRUCTION:
	case I_DB_INSTRUCTION: {
		do {
			if (tokens[i].type == T_STRING) {
				for (int j = 0; j < strlen(tokens[i].extra_bytes); j++)
					write_byte(tokens[i].extra_bytes[j]);
			} else if (tokens[i].type == T_INT){
				for (int j = 0; j < size_in_bytes(tokens[i].value) + 1; j++)
					write_byte((tokens[i].value << (j * 8)) & 0xFF);
			}

			i++;
		} while (tokens[i++].type == T_COMMA);

		break;
	}

	case V3_I_WRPR_INSTRUCTION:
	case V3_I_RDPR_INSTRUCTION: {
		if (_varient == 3) {
			struct argument arguments[256];
			int arg_i = 0;
			
			uint8_t information = 0;

			for (int j = 0; j < V3_ISA[tokens[0].value].argc; j++) {
				arguments[arg_i++] = get_arg(tokens, &i);

				information |= ((arguments[arg_i - 1].length << 2) | arguments[arg_i - 1].type) << (4 * j);
			}

			if (arguments[0].type != CODE_RVALUE || arguments[1].type != CODE_RREG || arguments[0].reference) {
				error("Incorrect syntax for %s instruction, should be %s INT, REG", V3_ISA[tokens[0].value].name, V3_ISA[tokens[0].value].name);
				return;
			}

			write_byte(tokens[0].value);
			write_byte(information);

			for (int j = 0; j < arg_i; j++)
				for (int x = 0; x < size_in_bytes(arguments[j].value) + 1; x++)
					write_byte((arguments[j].value >> (8 * x)) & 0xFF);

			break;
		}
	}

	default: {
		if (_varient == 2) {
			uint8_t register_offset = 0;
			int arg_i = 0;

			do {
				struct argument arg = get_arg(tokens, &i);
				
				if (arg.reference) {
					write_byte(1 << 7);
					insert_reference(&_labels[arg.value], get_writer_position(), -1, 0);
					arg_i++;
					continue;
				}

				if (arg.type == CODE_RREG && arg_i == 0 && tokens[0].value <= I_CMPR_INSTRUCTION && tokens[0].value >= I_MOV_INSTRUCTION) {
					register_offset = arg.value;
					arg_i++;
					continue;
				}

				uint8_t info_byte = (arg.instruction << 7) | (arg.type << 5) | (arg.length << 3) | (arg.cast << 1) | (arg.offset << 0);
				write_byte(info_byte);

				for (int j = 0; j < arg.length + 1; j++)
					write_byte((arg.value >> (j * 8)) & 0xFF);

				arg_i++;
			} while (tokens[i].type == T_COMMA);
			
			write_byte(tokens[0].value + (register_offset != 0 ? 1 : 0));
			if (register_offset != 0) write_byte(register_offset);
		} else if (_varient == 3) {
			write_byte(tokens[0].value);

			if (V3_ISA[tokens[0].value].argc > ZERO_ARGUMENTS) {

				struct argument arguments[256];

				uint8_t information = 0;
				int arg_i = 0;
				
				for (int j = 0; j < V3_ISA[tokens[0].value].argc; j++) {
					arguments[arg_i++] = get_arg(tokens, &i);

					information |= ((arguments[arg_i - 1].length << 2) | arguments[arg_i - 1].type) << (4 * j);
				}

				write_byte(information);
				int offset = 0;
				for (int j = 0; j < arg_i; j++) {
					if (arguments[j].reference) {
						insert_reference(&_labels[arguments[j].value], get_writer_position(), j, offset);
						continue;
					}

					for (int x = 0; x < size_in_bytes(arguments[j].value) + 1; x++)
						write_byte((arguments[j].value >> (8 * x)) & 0xFF);

					offset += size_in_bytes(arguments[j].value) + 1;
				}
			}
		}

		break;
	}
	}

	// If tokens are still left, call assemble function
	printf("%d\n", i);
	if (i < size)
		assemble(tokens + i, size - i);
}

void do_directive(struct token* tokens, int size) {
	switch (tokens[1].value) {
	case I_DIRECTIVE_AT:
		if (tokens[2].type == T_INT) {
			set_writer_position(tokens[2].value);
		} else if (tokens[2].type == T_STRING) {
			struct label* label = find_label(tokens[2].extra_bytes);
			
			if (label == NULL || label->defined == 0) // Make it so that this will write to a temp file with the label name, then append it to the main out file when done assembling at the given location of the filename when interpreted as a label.
				fatal_error("Label %s, cannot be used as directive, line %d", label->name, _line);
			
			set_writer_position(label->address);
		} else {
			error("Improper directive on line %d", _line);
		}

		if (3 < size) assemble(tokens + 3, size - 3);
		
		break;

	case I_DIRECTIVE_PUSHWRH:
		printf("PUSHING WRITE HEAD\n");
		wrh_stack[wrh_ptr++] = get_writer_position();
		if (2 < size) assemble(tokens + 2, size - 2);

		break;

	case I_DIRECTIVE_POPWRH:
		uint32_t wrh_pos = wrh_stack[--wrh_ptr];
		wrh_stack[wrh_ptr] = 0;
		set_writer_position(wrh_pos);
		if (2 < size) assemble(tokens + 2, size - 2);

		break;

	case I_DIRECTIVE_DATA:
		break;
	case I_DIRECTIVE_TEXT:
		break;
	}
}

void assemble(struct token* tokens, int size) {
	// printf("%s %X %d\n", TOKEN_NAMES[tokens[size-2].type], tokens[size-2].value, _line);


	// debug("ASSEMBLING TOKENS (%d):", _line);
	// for (int i = 0; i < size; i++)
	// 	debug("%s (%d, %s, %s)", TOKEN_NAMES[tokens[i].type], tokens[i].type, tokens[i].extra_bytes, V3_ISA[tokens[i].value].name);

	switch (tokens[0].type) {
	case T_INSTRUCTION:
		for (int i = 0; i < size; i++)
			printf("%s ", TOKEN_NAMES[tokens[i].type]);
		printf("\n");
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
			error("Unexpected string \"%s\" found on line %d", tokens[0].extra_bytes, _line);
		}

		break;
	
	}
}
