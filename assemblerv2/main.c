#include <global.h>
#include <message_handler.h>
#include <lex.h>
#include <assembly.h>
#include <writer.h>
#include <util.h>
#include <unistd.h>

FILE* _input_file = NULL;
FILE* _output_file = NULL;
char* _input_file_name = NULL;
char* _output_file_name = NULL;

struct label* _labels = NULL;
uint32_t _label_count = 0;

struct reference {
	uint32_t where;
	uint32_t what;
	uint8_t modified;
};

int main(int argc, char** argv) {
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-i") == 0) {
			_input_file_name = strdup(argv[i + 1]);
			_input_file = fopen(_input_file_name, "r");
		} else if (strcmp(argv[i], "-o") == 0) {
			_output_file_name = strdup(argv[i + 1]);
			_output_file = fopen(_output_file_name, "w");
		} else if (strcmp(argv[i], "-debug") == 0) {
			_debug = 1;
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
		while (next_token(&tokens[i], tokens, i) && tokens[i].type != T_NEWLINE) i++;
		assemble(tokens, i + 1);
		memset(tokens, 0, 1024 * sizeof(struct token));
		i = 0;
	}

	// Fill in references
	int total_references = 0;
	for (int i = 0; i < _label_count; i++)
		total_references += _labels[i].reference_count;

	struct reference ref_list[total_references];
	int reference_i = 0;


	// Sort all references into a big list in ascending order
	for (int i = 0; i < _label_count; i++) {
		debug("Filling label %s with %d reference(s)", _labels[i].name, _labels[i].reference_count);
	
		if (!_labels[i].defined)
			fatal_error("Label %s was never defined", _labels[i].name);

		for (int j = 0; j < _labels[i].reference_count; j++) {
			ref_list[reference_i].where = _labels[i].references[j];
			ref_list[reference_i].what = _labels[i].index;
			ref_list[reference_i++].modified = 0;
		}
	}

	struct reference ref_tmp;
	for (int i = 0; i < total_references; i++) {
		for (int j = 1; j < total_references; j++) {
			if (ref_list[j - 1].where > ref_list[j].where) {
				ref_tmp = ref_list[j - 1];
				ref_list[j - 1] = ref_list[j];
				ref_list[j] = ref_tmp;
			}
		}
	}

	for (int i = 0; i < total_references; i++)
		printf("%X: %X\n", ref_list[i].where, _labels[ref_list[i].what].address);

	int error = 0;
	// for (int j = 0; j < 2; j++) {
		for (int i = 0; i < total_references; i++) {
			// if (ref_list[i].where < _labels[ref_list[i].what].address) {
				ref_list[i].where += error;
				error += size_in_bytes(_labels[ref_list[i].what].address + size_in_bytes(_labels[ref_list[i].what].address) + 1) + 1; // Number of bytes added
				_labels[ref_list[i].what].address += error;

				// _labels[ref_list[i].what].address;

				// if (j == 0) {
				// 	_labels[ref_list[i].what].address += size_in_bytes(_labels[ref_list[i].what].address) + 1;
				// } else if (j == 1) {
				// 	// ref_list[i].what = _labels[ref_list[i].what].address;
				// }
			// }
		}
	// }
	
	printf("\n");

	for (int i = 0; i < total_references; i++)
		printf("%X: %X\n", ref_list[i].where, _labels[ref_list[i].what].address);
	
	fclose(_output_file);

	FILE* temp = fopen(_output_file_name, "r");
	uint8_t* out_temp = malloc(1);
	long out_temp_size = 1;
	long out_temp_ptr = 0;

	uint16_t byte = 0x0;
	set_writer_position(0);
	error = 0;

	while ((byte = fgetc(temp)) != 0xFFFF) {
		for (int i = 0; i < total_references; i++)
			if (get_writer_position() == ref_list[i].where) {
				out_temp_size += size_in_bytes(_labels[ref_list[i].what].address) + 1;
				out_temp = realloc(out_temp, out_temp_size);

				for (int j = 0; j < size_in_bytes(_labels[ref_list[i].what].address) + 1; j++)
					out_temp[out_temp_ptr++] = (_labels[ref_list[i].what].address >> (j * 8)) & 0xFF;
			}

		out_temp_size++;
		out_temp = realloc(out_temp, out_temp_size);
		out_temp[out_temp_ptr++] = byte & 0xFF;

		printf("%X\n", byte);
	}

	fclose(temp);

	_output_file = fopen(_output_file_name, "w");
	fwrite(out_temp, 1, out_temp_ptr, _output_file);
	fclose(_output_file);

	// int error = 0;
	// int previous_error = 0;
	// int error_counter = 0;

	// for (int i = 0; i < total_references; i++)
	// 	printf("%d\n", ref_list[i].what);

	// while (error_counter < 2) {
	// 	for (int i = 0; i < total_references; i++) {
	// 		// if (ref_list[i].where < ref_list[i].what) {

	// 		// 	// error += size_in_bytes(ref_list[i].what) + 1 + error;
	// 		// 	// ref_list[i].what += error;
	// 		// } else {
	// 			int current_size = size_in_bytes(ref_list[i].what) + 1;
	// 			int future_size = size_in_bytes(ref_list[i].what + current_size + error) + 1;

	// 			if (current_size != future_size || !ref_list[i].modified) {
	// 				error += future_size;
	// 				ref_list[i].what += error;
	// 				ref_list[i].modified = 1;
	// 			}
				

	// 			// error += size_in_bytes(ref_list[i].what + (size_in_bytes(ref_list[i].what) + 1 + error));
	// 			// ref_list[i].what += error;
	// 		// }

	// 		if (error == previous_error)
	// 			error_counter++;

	// 		previous_error = error;
	// 	}
	// }

	// printf("---\n");
	// for (int i = 0; i < total_references; i++)
	// 	printf("%d %d\n", ref_list[i].what, ref_list[i].modified);

}
