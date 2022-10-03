#include <emulator.h>
#include <screen.h>
#include <message_handler.h>
#include <unistd.h>
#include <string.h>

FILE* input_file = NULL;
long _file_size = 0;

int main(int argc, char** argv) {
	init_emulator();

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-i") == 0) {
			input_file = fopen(argv[i + 1], "r");
			i += 2;
		} else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
			printf("Usage:\n");
			printf("./emulator -i /path/to/input.bin [additional options]\n");
			printf("-m number : Sets the number of bytes memory has");
			return 0;
		} else if (strcmp(argv[i], "-disasm") == 0) {
			_disassemble = 1;
		}
	}
	
	if (input_file == NULL)
		fatal_error("No input file specified");
	
	if (!_disassemble) init_screen();

	fseek(input_file, 0, SEEK_END);
	_file_size = ftell(input_file);
	fseek(input_file, 0, SEEK_SET);
	fread(memory, 1, _file_size, input_file);

	pthread_create(&process_thread, NULL, proccess_cycle, NULL);

	if (!_disassemble) {
		while (emulator_running) {
			update();
			render();
		}
	}

	pthread_join(process_thread, NULL);

	return 0;
}
