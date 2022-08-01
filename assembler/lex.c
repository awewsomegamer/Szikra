#include <lex.h>

char read_char(){
	char c = fgetc(_in_file);

	if (c == EOF)
		_eof_reached = true;
	
	if (c == '\n')
		_line++;

	return c;
}

void putback(char c) {
	_putback = c;
}

char skip(){
	char c = read_char();

	while (!IS_VISUAL(c) && !_eof_reached)
		c = read_char();
		
	return c;
}


char next(){
	if (_putback != 0 && IS_VISUAL(_putback)){
		char c = _putback;
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

	char* str = malloc(i + 1);
	memset(str, 0, i + 1);
	strncpy(str, buffer, i);

	return str;
}

int get_number(char c){
	return strtol(strdup(get_str(c, IS_VISUAL)), NULL, 10);
}

int find_operation(char c){
	char* name = strdup(get_str(c, IS_VISUAL));
	uppercaseString(name);

	for (int i = 0; i < I_INSTRUCTION_MAX; i++)
		if (strcmp(ISA[i].name, name) == 0)
			return i;
	
	// No valid instruction found
	return -1;
}

int find_size(char c){
	char* size = strdup(get_str(c, IS_ALPHA));
	uppercaseString(size);

	if (strcmp(size, "BYTE") == 0){
		return SZ_BYTE;
	} else if (strcmp(size, "WORD") == 0){
		return SZ_WORD;
	} else if (strcmp(size, "DWORD") == 0){
		return SZ_DWORD;
	}

	return -1;
}

int find_register(char c){
	char* reg = strdup(get_str(c, IS_ALPHANUMERIC));

	for (int i = 0; i < I_REG_MAX; i++)
		if (strcmp(REGISTERS[i], reg) == 0)
			return i;

	return -1;
}

int lex(struct token* t){
	char c = next();
	
	switch (c) {
	case EOF:
		t->type = T_EOF;
	
		return 1;

	case '+':
		t->type = T_ADD;

		return 1;

	case '-':
		t->type = T_SUB;

		return 1;

	case '*':
		t->type = T_MUL;

		return 1;

	case '/':
		t->type = T_DIV;

		return 1;

	case ',':
		t->type = T_COMMA;

		return 1;

	case '.':
		t->type = T_DOT;

		return 1;

	case ':':
		t->type = T_COLON;

		return 1;

	case '!':
		t->type = T_DIRECTIVE;

		return 1;

	case '[':
		t->type = T_LSQR_BRACKET;

		return 1;

	case ']':
		t->type = T_RSQR_BRACKET;

		return 1;

	case '(':
		t->type = T_LPARAN;

		return 1;

	case ')':
		t->type = T_RPARAN;

		return 1;

	default:
		if (IS_DIGIT(c)){
			t->type = T_INT;
			t->value = get_number(c);
		} else if (IS_VISUAL(c)){
			int instruction_found = find_operation(c);

			if (instruction_found != -1){
				t->type = T_INSTRUCTION;
				t->value = instruction_found;
			}

			int size = find_size(c);

			if (size != -1){
				t->type = T_SIZE;
				t->value = size;
			}

			int reg = find_register(c);

			if (reg != -1){
				t->type = T_REGISTER;
				t->value = reg;
			}

			char* string = strdup(get_str(c, IS_ALPHA));

			t->type = T_STRING;
			t->extra_bytes = malloc(strlen(string));
			strcpy(t->extra_bytes, string);

			// Regular string of characters found
			// Possibly label, local label follow up, or directive specifier
		}


		return 1;
	}

	return 0;
}