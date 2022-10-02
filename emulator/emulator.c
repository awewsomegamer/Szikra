#include <emulator.h>
#include <interrupts.h>

uint8_t memory[UINT16_MAX];
uint8_t emulator_running = 1;
pthread_t process_thread;

int next_byte() {
	return memory[registers[I_REG_IP]++];
}

// Ensure instruction is valid, if so return the opcode and increment IP
int fetch_instruction() {
	if (memory[registers[I_REG_IP]] > I_INSTRUCTION_MAX) {
		 printf("Invalid opcode (%X) at %X\n", memory[registers[I_REG_IP]], registers[I_REG_IP]); // Call invalid opcode interrupt
		 for (;;);
		 return 0;
	}
	return memory[registers[I_REG_IP]++];
}

void* proccess_cycle(void* arg) {
	while (emulator_running) {
		if (registers[I_REG_IP] >= sizeof(memory))
			registers[I_REG_IP] = 0x0;

		v2_cycle();

		usleep(1000);
	}
}

void init_emulator() {
	init_interrupts();
	init_v2();
}