#include <lex.h>

#define IS_VISUAL(c) (c >= 0x21 && c <= 0x7E)
#define IS_DIGIT(c) (c >= 0x30 && c <= 0x39)

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

char* get_str(char c){
	char buffer[1024];
	memset(buffer, 0, 1024);

	int i = 0;
	while (IS_VISUAL(c) && !_eof_reached){
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
	return strtol(strdup(get_str(c)), NULL, 10);
}

int find_operation(char c){
	char* name = strdup(get_str(c));
	uppercaseString(name);

	for (int i = 0; i < I_INSTRUCTION_MAX; i++)
		if (strcmp(ISA[i].name, name) == 0)
			return i;
	
	// No valid instruction found
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

	default:
		if (IS_DIGIT(c)){
			t->type = T_INT;
			t->value = get_number(c);
		} else if (IS_VISUAL(c)){
			int instruction_found = find_operation(c);

			if (instruction_found != -1){
				t->type = T_INSTRUCTION;
				t->value = instruction_found;
			} else {
				// Regular string of characters found
				// Possibly label, local label follow up, or directive specifier
			}
		}


		return 1;
	}

	return 0;
}