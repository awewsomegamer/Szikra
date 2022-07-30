#include <global.h>
#include <lex.h>

uint32_t _line = 1;
char _putback = 0;
bool _error_detected = false;
bool _eof_reached = false;

FILE* _in_file = NULL;
FILE* _out_file = NULL;

int main(int argc, char** argv){
	char* in_file_name = NULL;
	char* out_file_name = NULL;

	for (int i = 1; i < argc; i++){
		if (strcmp(argv[i], "-i") == 0){
			in_file_name = strdup(argv[++i]);
			continue;
		} else if (strcmp(argv[i], "-o") == 0){
			out_file_name = strdup(argv[++i]);
			continue;
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
	
	bool stop = false;

	while (!stop){
		struct token* head = malloc(sizeof(struct token));
		struct token* current = head;

		int current_line = _line;
		
		// printf("%d, %d\n", current_line, _line);

		// Get current token series
		while (_line == current_line){
			lex(current);

			struct token* n = malloc(sizeof(struct token));
			n->next = NULL;

			current->next = n;

			current = n;

			if (_eof_reached){
				stop = true;
				break;
			}
		}

		// Assembling

		// Clean up
		current = head;
		struct token* next = current->next;

		// while (current->next != NULL){
		// 	free(current);

		// 	next = next->next;
		// 	current = next;	
		// }

		free(current);

		if (stop)
			break;
	}

	printf("\n");
}
