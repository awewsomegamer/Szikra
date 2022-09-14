#include <global.h>
#include <message_handler.h>
#include <lex.h>
#include <assembly.h>
#include <writer.h>
#include <util.h>

FILE* _input_file = NULL;
FILE* _output_file = NULL;
char* _input_file_name = NULL;
char* _output_file_name = NULL;

struct label* _labels = NULL;
uint32_t _label_count = 0;

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
		_output_file = fopen(_output_file_name, "w");
	}

	struct token tokens[1024];
	int i = 0;
	while (!_eof_reached) {
		while (next_token(&tokens[i]) && tokens[i].type != T_NEWLINE) i++;
		assemble(tokens, i + 1);
		memset(tokens, 0, 1024 * sizeof(struct token));
		i = 0;
	}

	for (int i = 0; i < _label_count; i++) {
		if (!_labels[i].defined)
			fatal_error("Label %s was never defined", _labels[i].name);
		
		if (_labels[i].reference_count > 0) {
			for (int j = 0; j < _labels[i].reference_count; j++) {
				debug("Filling value at %X with %X", _labels[i].references[j], _labels[i].address);
				set_writer_position(_labels[i].references[j]);
				fwrite(&_labels[i].address, size_in_bytes(_labels[i].address) + 1, 1, _output_file);
			}
		}
	}

}
