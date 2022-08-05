#include <global.h>
#include <lex.h>
#include <assembly.h>
#include <instructions.h>

uint32_t _line = 1;
uint32_t _label_count = 0;
char* _file_format = NULL;
char _putback = 0;
bool _error_detected = false;
bool _eof_reached = false;
bool _debug = false;

FILE* _in_file = NULL;
FILE* _out_file = NULL;

struct label* _labels = NULL;
struct label* _current_label = NULL;
struct label* _tmp_label = NULL;

struct reference* _references = NULL;
struct reference* _current_reference = NULL;
struct reference* _tmp_reference = NULL;

int main(int argc, char** argv){
	char* in_file_name = NULL;
	char* out_file_name = NULL;
	char* out_file_format = NULL;

	for (int i = 1; i < argc; i++){
		if (strcmp(argv[i], "-i") == 0){
			in_file_name = strdup(argv[++i]);
			continue;
		} else if (strcmp(argv[i], "-o") == 0){
			out_file_name = strdup(argv[++i]);
			continue;
		} else if (strcmp(argv[i], "-format") == 0){
			out_file_format = strdup(argv[++i]);
			continue;
		} else if (strcmp(argv[i], "-debug") == 0){
			_debug = true;
		}
	}

	if (in_file_name == NULL)
		fatal_error("No input file specified");
	
	if (out_file_name == NULL){
		warn("No output file specified, defaulting to \"%s.out\"", in_file_name);

		out_file_name = malloc(strlen(in_file_name) + 4);
		sprintf(out_file_name, "%s.out", in_file_name);
	}

	_in_file = fopen(in_file_name, "r");
	_out_file = fopen(out_file_name, "w");
	_file_format = out_file_format;
	
	_labels = (struct label*)malloc(sizeof(struct label));
	_current_label = _labels;

	_references = (struct reference*)malloc(sizeof(struct reference));
	_current_reference = _references;

	init_instructions();

	struct token* head = malloc(sizeof(struct token));
	struct token* current = head;

	int token_count = 1; // There is at least 1 token
	// int current_line = -1;

	// Get current token series
	while (lex(current)){
		token_count++;

		struct token* n = malloc(sizeof(struct token));
		n->next = NULL;

		current->next = n;

		current = n;
	}

	// Assembling
	if (head->type != T_EOF)
		assemble(head, token_count);

	// Stitch main out file and data file
	// Fill in references

	fclose(_in_file);
	fclose(_out_file);

	printf("\n");
}
