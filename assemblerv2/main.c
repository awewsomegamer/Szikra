#include <global.h>
#include <message_handler.h>
#include <lex.h>

FILE* _input_file = NULL;
FILE* _output_file = NULL;
char* _input_file_name = NULL;
char* _output_file_name = NULL;

int main(int argc, char** argv) {
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-i") == 0) {
			_input_file_name = strdup(argv[i + 1]);
			_input_file = fopen(_input_file_name, "r");
		} else if (strcmp(argv[i], "-o") == 0) {
			_output_file_name = strdup(argv[i + 1]);
			_output_file = fopen(_output_file_name, "w");
		}
	}

	if (_input_file_name == NULL) {
		fatal_error("No input file given");
	}

	if (_output_file_name == NULL) {
		_output_file_name = malloc(sizeof(_input_file_name) + 4);
		sprintf(_output_file_name, "%s.out", _input_file_name);
		warn("No output file give, defaulting to %s", _output_file_name);
		_output_file_name = fopen(_output_file_name, "w");
	}

	struct token tokens[1024];
	int i = 0;
	while (!_eof_reached) {
		while (lex(&tokens[i]) && tokens[i].type != T_NEWLINE) i++;
		assemble(tokens, i + 1);
		memset(tokens, 0, 1024 * sizeof(struct token));
	}

}
