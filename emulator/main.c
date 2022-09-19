#include <emulator.h>
#include <screen.h>
#include <message_handler.h>
#include <unistd.h>

FILE* input_file = NULL;

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
	long file_size = ftell(input_file);
	fseek(input_file, 0, SEEK_SET);
	fread(memory, 1, file_size, input_file);

	init_emulator();
	init_screen();

	pthread_create(&process_thread, NULL, proccess_cycle, NULL);

	while (emulator_running) {
		update();
		render();
	}

	pthread_join(process_thread, NULL);

	return 0;
}
