#include <lex.h>

uint32_t _line = 1;
char putback = 0;
uint8_t _eof_reached = 0;
uint8_t _comment_togle = 0;

char read_char() {
	char c = fgetc(_input_file);

	if (c == '\n')
		_line++;
	else if (c == EOF)
		_eof_reached = 1;

	return c;
}

char get_next() {
	if (putback != 0) {
		char c = putback;
		putback = 0;
		return c;
	}

	return read_char();
}

char next_char() {
	char c = get_next();

	while (c == ' ' || c == '\t' || c == '\r')
		c = get_next();

	return c;
}

char* get_string(char c, uint8_t(*function)(char c)) {
	char buffer[1024];
	memset(buffer, 0, 1024);

	int i = 0;
	while ((*function)(c)) {
		buffer[i++] = c;
		c = get_next();
	}

	putback = c;

	char* str = malloc(i + 1);
	memset(str, 0, i +1);
	strcpy(str, buffer);

	return str;
}

int read_number(char c, int base) {
	return strtol(get_string(c, isalnum), NULL, base);
}

int next_token(struct token* t, struct token* tokens, int index) {
	char c = next_char();
	// printf("RECIEVED %c\n", c);

	switch (c) {
	case EOF:
		debug("EOF");
		t->type = T_EOF;
	
		return 0;

	case '+':
		debug("'+'");
		t->type = T_ADD;

		return 1;

	case '-':
		debug("'-'");
		t->type = T_SUB;

		return 1;

	case '*':
		debug("'*'");
		t->type = T_MUL;

		return 1;

	case '/':
		debug("'/'");
		t->type = T_DIV;

		return 1;

	case ',':
		debug("','");
		t->type = T_COMMA;

		return 1;

	case '.':
		debug("'.'");
		t->type = T_DOT;

		return 1;

	case ':':
		debug("':'");
		t->type = T_COLON;

		return 1;

	case '!':
		debug("'!'");
		t->type = T_DIRECTIVE;

		return 1;

	case '[':
		debug("'['");
		t->type = T_LSQR_BRACKET;

		return 1;

	case ']':
		debug("']'");
		t->type = T_RSQR_BRACKET;

		return 1;

	case '(':
		debug("'('");
		t->type = T_LPARAN;

		return 1;

	case ')':
		debug("')'");
		t->type = T_RPARAN;

		return 1;

	case '#':
		debug("'#'");
		t->type = T_HASH;

		return 1;

	case '%':
		debug("'%'");
		t->type = T_PERCENT;

		return 1;

	case '\n':
		debug("NEW LINE");
		t->type = T_NEWLINE;
		
		return 1;

	case ';':
		// _comment_togle = !_comment_togle;
		
		return 1;

	default:
		// Check if this is a integer literal
		// No? Check if this is a keyword
		// Check if this is a string literal

		if (isdigit(c) || c == '\'') {
			t->type = T_INT;

			if (c == '\'') {
				t->value = read_char();
				read_char();
				return 1;
			}

			char identifier = get_next();
			switch (identifier) {
			case 'x':
				t->value = read_number(read_char(), 16);
				return 1;
			case 'b':
				t->value = read_number(read_char(), 2);
				return 1;
			case 'o':
				t->value = read_number(read_char(), 8);
				return 1;
			default:
				putback = identifier;
				t->value = read_number(c, 10);
				return 1;
			}

			error("Unable to get number on line %d", _line);
			return 0;
		} else if (isalnum(c)) {
			char* keyword = get_string(c, isalnum);

			// Compare to directive
			if (index > 0 && tokens[index - 1].type == T_DIRECTIVE) {
				for (int i = 0; i < I_DIRECTIVE_MAX; i++) {
					if (strcasecmp(keyword, DIRECTIVES[i]) == 0) {
						t->type = T_INT;
						t->value = i;
						return 1;
					}
				}
			}

			// Compare to instruction
			for (int i = 0; i < I_INSTRUCTION_MAX; i++) {
				if (strcasecmp(ISA[i].name, keyword) == 0) {
					t->type = T_INSTRUCTION;
					t->value = i;
					return 1;
				}
			}

			// Comapre to registers
			for (int i = 1; i < I_REG_MAX; i++) {
				if (strcasecmp(REGISTERS[i], keyword) == 0) {
					t->type = T_REGISTER;
					t->value = i;
					return 1;
				}
			}
			// Compare to size
			t->type = T_SIZE;
			if (strcasecmp(keyword, "byte") == 0)
				t->value = SZ_BYTE;
			else if (strcasecmp(keyword, "byte") == 0)
				t->value = SZ_BYTE;
			else if (strcasecmp(keyword, "byte") == 0)
				t->value = SZ_BYTE;


			// It is a label
			t->type = T_STRING;
			t->extra_bytes = keyword;

			return 1;
		} else if (c == '\"') {
			char* string = malloc(1);
			int string_size = 1;
			char last = 0;

			do {
				last = c;
				c = read_char();

				if (c != '\"' || (last == '\\' && c == '\"')) {
					*(string + string_size - 1) = c;
					string_size++;
				}
			} while (c != '\"');

			t->type = T_STRING;
			t->extra_bytes = string;

			return 1;
		}

		return 0;
	}

	return 0;
}