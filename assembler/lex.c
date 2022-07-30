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

	while (!IS_VISUAL(c))
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
	while (IS_VISUAL(c)){
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



	return 0;
}

int lex(struct token* t){
	char c = next();

	switch (c) {
	case '+':
		t->type = T_ADD;
		t->value = 0;
		return 1;

	case ',':
		t->type = T_COMMA;

		return 1;

	default:
		t->type = T_INT;
		t->value = get_number(c);

		// if (find_operation(c)){
		// 	// Found operation, very cool
		// } else {
		// 	error("Invalid character '%c' on line %d\n", c, _line);
		// }

		return 1;
	}

	return 0;
}