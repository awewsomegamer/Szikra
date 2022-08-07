#include <lex.h>

bool is_comment = false;

char read_char(){
	char c = fgetc(_in_file);

	switch (c){
	case EOF:
		_eof_reached = true;
		break;
	case '\n':
		_line++;
		break;
	case ';':
		is_comment = !is_comment;
		return 0;
	}

	return c;
}

void putback(char c) {
	debug("PUTTING BACK: %d\n", c);

	if (c == '\n')
		_line--;

	_putback = c;
}

char skip(){
	char c = read_char();
	
	while (!IS_VISUAL(c) && !_eof_reached || is_comment)
		c = read_char();

	debug("SKIPPED TO: %c", c);

	return c;
}

char next(){
	if (_putback != 0 && IS_VISUAL(_putback)){
		char c = _putback;

		if (c == '\n')
			_line++;

		_putback = 0;
		return c;
	}

	return skip();
}

char* get_str(char c, bool(*function)(char c)){
	char buffer[1024];
	memset(buffer, 0, 1024);

	int i = 0;
	while ((*function)(c) && !_eof_reached){
		buffer[i++] = c;
		c = read_char();
	}

	putback(c);
	debug("[LEX]: PUTBACK: %d, BUFFER: %s", c, buffer);

	char* str = malloc(i + 1);
	memset(str, 0, i + 1);
	strncpy(str, buffer, i);

	return str;
}

int get_number(char c){
	return strtol(strdup(get_str(c, IS_ALPHANUMERIC)), NULL, 10);
}

int find_operation(char* str){
	char* filtered = filter_characters(str, IS_ALPHA);

	for (int i = 0; i < I_INSTRUCTION_MAX; i++)
		if (strcmp(ISA[i].name, filtered) == 0)
			return i;
	
	// No valid instruction found
	return -1;
}

int find_size(char* str){
	char* filtered = filter_characters(str, IS_ALPHA);

	if (strcmp(filtered, "BYTE") == 0){
		return SZ_BYTE;
	} else if (strcmp(filtered, "WORD") == 0){
		return SZ_WORD;
	} else if (strcmp(filtered, "DWORD") == 0){
		return SZ_DWORD;
	}

	return -1;
}

int find_register(char* str){
	char* filtered = filter_characters(str, IS_ALPHANUMERIC);

	for (int i = 0; i < I_REG_MAX; i++)
		if (strcmp(REGISTERS[i], filtered) == 0)
			return i;

	return -1;
}

int lex(struct token* t){
	char c = next();
	
	t->line = _line;
	
	debug("CURRENT CHARACTER: %d\n", c);

	switch (c) {
	case EOF:
		debug("EOF");
		t->type = T_EOF;
	
		return 0;

	case '+':
		debug("+");
		t->type = T_ADD;

		return 1;

	case '-':
		debug("-");
		t->type = T_SUB;

		return 1;

	case '*':
		debug("*");
		t->type = T_MUL;

		return 1;

	case '/':
		debug("/");
		t->type = T_DIV;

		return 1;

	case ',':
		debug(",");
		t->type = T_COMMA;

		return 1;

	case '.':
		debug(".");
		t->type = T_DOT;

		return 1;

	case ':':
		debug(":");
		t->type = T_COLON;

		return 1;

	case '!':
		debug("!");
		t->type = T_DIRECTIVE;

		return 1;

	case '[':
		debug("[");
		t->type = T_LSQR_BRACKET;

		return 1;

	case ']':
		debug("]");
		t->type = T_RSQR_BRACKET;

		return 1;

	case '(':
		debug("(");
		t->type = T_LPARAN;

		return 1;

	case ')':
		debug(")");
		t->type = T_RPARAN;

		return 1;

	case '#':
		debug("#");
		t->type = T_HASH;

		return 1;

	case '%':
		debug("%");
		t->type = T_PERCENT;

		return 1;

	default:
		debug("DEFAULT");
		if (IS_DIGIT(c)){
			t->type = T_INT;
			t->value = get_number(c);

			return 1;
		} else if (IS_ALPHANUMERIC(c)){
			char* str = get_str(c, IS_ALPHANUMERIC);
			char* upper = strdup(str);
			uppercaseString(upper);

			debug("\"%s\" -> \"%s\"\n", str, upper);

			// Check for an instruction
			int instruction_found = find_operation(upper);
			if (instruction_found != -1){
				t->type = T_INSTRUCTION;
				t->value = instruction_found;
				debug("ITRUCTION FOUND\n");
				return 1;
			}

			// Check for a size descriptor
			int size = find_size(upper);
			if (size != -1){
				t->type = T_SIZE;
				t->value = size;
				debug("SE FOUND\n");

				return 1;
			}

			// Check for a register
			int reg = find_register(upper);
			if (reg != -1){
				t->type = T_REGISTER;
				t->value = reg;
				debug("R FOUND\n");

				return 1;
			}
			
			// Label
			t->type = T_STRING;
			t->extra_bytes = str; // If a token's extra_bytes result in (null) when printed, change this to use strdup to give a copy of the string to the token

			return 1;
		} else if (IS_VISUAL(c)){
			// Dump string into a string token
			char* str = get_str(c, IS_VISUAL);
			
			debug("Putting string \"%s\"\n", str);

			t->type = T_STRING;
			t->extra_bytes = str; // If a token's extra_bytes result in (null) when printed, change this to use strdup to give a copy of the string to the token
		}


		return 1;
	}

	return 0;
}