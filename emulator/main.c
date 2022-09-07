#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <message_handler.h>

FILE* input_file = NULL;
uint8_t memory[UINT16_MAX];


int main(int argc, char** argv) {
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-i") == 0) {
			input_file = fopen(argv[i + 1], "r");
			i += 2;
		} else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
			printf("Usage:\n");
			printf("./emulator -i /path/to/input.bin [additional options]\n");
			printf("-m number : Sets the number of bytes memory has");
			return 0;
		}

	} 

	if (input_file == NULL) {
		fatal_error("No input file specified");
	}
	
	fseek(input_file, 0, SEEK_END);
	fread(memory, 1, ftell(input_file), input_file);
	fseek(input_file, 0, SEEK_SET);

	


	return 0;
}
