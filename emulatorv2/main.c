#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

FILE* input_file;
uint8_t memory[];
uint8_t emulator_running;

uint8_t RAM[UINT16_MAX];
uint32_t ADDRESS_BUS = 0x0;
uint32_t ADDRESS_BUS_VALUE = 0x0;
uint32_t PROGRAM_COUNTER = 0x0;
uint8_t ALU_CONTROL = 0x0; // ALU Control word (0 = ADD, 1 = SUB, 2 = MUL, 4 = DIV (all integer operations))

// Destroyed registers, should not be used
uint32_t R1 = 0x0; // Register 1
uint32_t R2 = 0x0; // Register 2
uint32_t R3 = 0x0; // Register 3

void clock_tick_high() {
	// Do ALU (below code can probably be done with and gates)
	switch (ALU_CONTROL) {
	case 0x0:
		R3 = R1 + R2;
		break;
	case 0x1:
		R3 = R1 - R2;
		break;
	case 0x2:
		R3 = R1 * R2;
		break;
	case 0x3:
		R3 = R1 / R2;
		break;
	}

	// Load instructions
		// Load (instruction | info) word into R1
		// Load operand 1 into R2
		// Load operand 2 into R3
}

void clock_tick_low() {
	// Set R registers
	// Set control word
}

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

	// if (input_file == NULL) {
	// 	fatal_error("No input file specified");
	// }
	
	// fseek(input_file, 0, SEEK_END);
	// long file_size = ftell(input_file);
	// fseek(input_file, 0, SEEK_SET);
	// fread(memory, 1, file_size, input_file);

	while (1) {
		printf("%d -> ", R3);
		clock_tick_high();
		printf("%d -> ", R3);
		sleep(1);
		printf("%d -> ", R3);
		clock_tick_low();
		printf("%d\n", R3);
		sleep(1);
	}
}