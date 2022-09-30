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
	char* name;
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
			ref_list[reference_i].what = _labels[i].address;
			ref_list[reference_i++].name = _labels[i].name;
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

	printf("Pre calc:\n");
	for (int i = 0; i < total_references; i++)
		printf("%X \"%s\": %X\n", ref_list[i].where, ref_list[i].name, ref_list[i].what);
	printf("\n");

	// New address calculator
	int error = 0;
	for (int i = 0; i < total_references; i++) {
		ref_list[i].where += error;
		error += size_in_bytes(ref_list[i].what + size_in_bytes(ref_list[i].what) + 1) + 1; // Number of bytes added

		// if (ref_list[i].where > ref_list[i].what)
		printf("\"%s\"[%d]: ", ref_list[i].name, i);
		int org_what = ref_list[i].what;
		for (int j = 0; (ref_list[j].where < ref_list[i].what) && (ref_list[j].name != NULL) && (ref_list[j].where <= org_what); j++) {
			ref_list[i].what += size_in_bytes(ref_list[j].what + size_in_bytes(ref_list[j].what) + 1) + 1;
			printf("%s (%d) + ", ref_list[j].name, size_in_bytes(ref_list[j].what + size_in_bytes(ref_list[j].what) + 1) + 1);
		}
		printf("\n");

		// else
		// 	for (int j = i; (j < total_references) && (ref_list[j].where <= ref_list[i].what); j++)
		// 		ref_list[i].what += size_in_bytes(ref_list[j].what + size_in_bytes(ref_list[j].what) + 1) + 1;
	}

	printf("\nPost calc:\n");
	for (int i = 0; i < total_references; i++)
		printf("%X \"%s\": %X\n", ref_list[i].where, ref_list[i].name, ref_list[i].what);
		
	
	fclose(_output_file);

	// Filler
	FILE* temp = fopen(_output_file_name, "r");
	uint8_t* out_temp = malloc(1);
	long out_temp_size = 1;
	long out_temp_ptr = 0;

	uint8_t byte = 0x0;
	int FF_counter = 0;
	error = 0;

	while (FF_counter != 5) {
		byte = fgetc(temp);

		uint8_t found_reference = 0;

		for (int i = 0; i < total_references; i++)
			if (get_writer_position() == ref_list[i].where - error) {
				byte |= (size_in_bytes(ref_list[i].what) & 0b11) << 3;

				out_temp_size += size_in_bytes(ref_list[i].what) + 2;
				out_temp = realloc(out_temp, out_temp_size);
				error += size_in_bytes(ref_list[i].what) + 1;

				out_temp[out_temp_ptr++] = byte & 0xFF;

				for (int j = 0; j < size_in_bytes(ref_list[i].what) + 1; j++)
					out_temp[out_temp_ptr++] = (ref_list[i].what) >> (j * 8) & 0xFF;

				found_reference = 1;
			}

		if (!found_reference) {
			out_temp_size++;
			out_temp = realloc(out_temp, out_temp_size);
			out_temp[out_temp_ptr++] = byte & 0xFF;
		}

		if (byte == 0xFF)
			FF_counter++;
		else
			FF_counter = 0;
	}

	fclose(temp);

	_output_file = fopen(_output_file_name, "w");
	fwrite(out_temp, 1, out_temp_ptr - FF_counter, _output_file);
	fclose(_output_file);

	return 0;
}
