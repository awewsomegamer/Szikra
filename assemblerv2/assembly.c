#include <assembly.h>
#include <writer.h>
#include <message_handler.h>
#include <util.h>

uint32_t wrh_stack[1024];
uint32_t wrh_ptr = 0;

struct argument get_arg(struct token* tokens, int* i) {
	struct argument ret;
	memset(&ret, 0, sizeof(struct argument));

	switch (tokens[*i].type) {
	case T_INT: // Is integer
		ret.type = CODE_RVALUE;
		ret.length = size_in_bytes(tokens[*i].value);
		ret.instruction = 1;
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
			ret.type = CODE_REF;
			ret.instruction = 1;

			(*i)++;

			return ret;
		} else if (!label->defined) {
			ret.value = label->index;
			ret.type = CODE_REF;
			ret.instruction = 1;

			(*i)++;

			return ret;
		}

		ret.instruction = 1;
		ret.value = label->index;
		ret.type = CODE_REF;

		(*i)++;

		return ret;

	case T_REGISTER: // Is register
		ret.type = CODE_RREG;
		ret.length = 0;
		ret.value = tokens[*i].value;
		ret.instruction = 1;

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

	switch (tokens[0].value) {
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

	default: {
		// While there are more arguments, write them
		do {
			struct argument arg = get_arg(tokens, &i);
			
			if (arg.type == CODE_REF) {
				write_byte(1 << 7);
				insert_reference(&_labels[arg.value], get_writer_position());
				continue;
			}

			uint8_t info_byte = (arg.instruction << 7) | (arg.type << 5) | (arg.length << 3) | (arg.cast << 1) | (arg.offset << 0);
			write_byte(info_byte);

			for (int j = 0; j < arg.length + 1; j++)
				write_byte((arg.value >> (j * 8)) & 0xFF);
		} while (tokens[i].type == T_COMMA);

		write_byte(tokens[0].value);

		break;
	}
	}

	// If tokens are still left, call assemble function
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
	debug("ASSEMBLING TOKENS (%d):", _line);
	for (int i = 0; i < size; i++)
		debug("%s (%d, %s, %s)", TOKEN_NAMES[tokens[i].type], tokens[i].type, tokens[i].extra_bytes, ISA[tokens[i].value].name);

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
				label->address = get_writer_position();// + (label->reference_count * (size_in_bytes(get_writer_position()) + 1));
				label->defined = 1;

				// printf("%s:%X\n", label->name, label->address);

				// uint8_t* filled = malloc(1);
				// uint32_t filled_size = 1;
				// uint32_t filled_ptr = 0;

				// FILE* redo = fopen(_output_file_name, "r");
				
				// uint16_t byte = 0;
				// int error = 0;

				// while ((byte = fgetc(redo)) != 0xFFFF) {
				// 	long where = ftell(redo);

				// 	for (int i = 0; i < label->reference_count; i++) {
				// 		if (label->references[i] != -1 && where == (label->references[i] + error)) {
				// 			filled_size += size_in_bytes(label->address) + 1;
				// 			filled = realloc(filled, filled_size);

				// 			for (int j = 0; j < size_in_bytes(label->address) + 1; j++)
				// 				filled[filled_ptr++] = (label->address >> (j * 8)) & 0xFF;

				// 			error += size_in_bytes(label->address) + 1;

				// 			label->references[i] = -1;
				// 		}
				// 	}

				// 	filled_size++;
				// 	filled = realloc(filled, filled_size);

				// 	filled[filled_ptr++] = byte;
				// }

				// for (int j = 0; j < filled_size; j++)
				// 	printf("%02X ", filled[j]);
				// printf("\n");


				// fclose(redo);

				// _output_file = fopen(_output_file_name, "w");

				// fwrite(filled, 1, sizeof(filled), _output_file);

				// fseek(_output_file, 0, SEEK_END);
				// long end = ftell(_output_file);
				// set_writer_position(end);

				// int error = 0;
				// while (1) {
				// 	for (int i = 0; i < label->reference_count; i++) {
				// 		if (get_writer_position() == label->references[i]) {
				// 			filled_size += size_in_bytes(label->address) + 1;
				// 			filled = realloc(filled, filled_size);

				// 			for (int j = 0; j < size_in_bytes(label->address) + 1; j++)
				// 				*(filled + get_writer_position() + j + error) = (label->address >> (j * 8)) & 0xFF;

				// 			error = size_in_bytes(label->address) + 1;
				// 			set_writer_position(get_writer_position() + size_in_bytes(label->address) + 1);
				// 		}
				// 	}

				// 	filled_size++;
				// 	filled = realloc(filled, filled_size);
				// 	*(filled + error) = byte;
					
				// 	if (get_writer_position() == limit)
				// 		break;
				// }
				


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
