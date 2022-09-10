#include <emulator.h>

uint8_t memory[UINT16_MAX];
uint8_t emulator_running = 1;
pthread_t process_thread;

uint32_t registers[I_REG_MAX]; 

// Ensure instruction is valid, if so return the opcode and increment IP
int fetch_instruction() {
	if (memory[registers[I_REG_IP]] > I_INSTRUCTION_MAX); // Call invalid opcode interrupt
	return memory[registers[I_REG_IP]++];
}

// Return a list of arguments based on ISA[instruction]
void load_arguments(int instruction, struct argument* argument_list) {
	for (int i = 0; i < ISA[instruction].argc; i++) {
		argument_list[i].type   =  (memory[registers[I_REG_IP]] >> 6) & 0b11;
		argument_list[i].length = ((memory[registers[I_REG_IP]] >> 4) & 0b11) + 1;
		argument_list[i].cast   =  (memory[registers[I_REG_IP]] >> 2) & 0b11;
		argument_list[i].offset =  (memory[registers[I_REG_IP]] >> 1) & 0b1;
		argument_list[i].sign   =  (memory[registers[I_REG_IP]] >> 0) & 0b1;

		registers[I_REG_IP]++;

		for (int j = 0; j < argument_list[i].length; i++)
			argument_list[i].value = memory[registers[I_REG_IP]++] << (i * 8);
		
		registers[I_REG_IP]++;
	}

	registers[I_REG_IP]++;
}

void* proccess_cycle(void* arg) {
	int instruction = fetch_instruction();
	struct argument arguments[256];
	load_arguments(instruction, arguments);

	printf("%s %X %X (IP: %X)\n", ISA[instruction].name, arguments[0].value, arguments[1].value, registers[I_REG_IP]);
}