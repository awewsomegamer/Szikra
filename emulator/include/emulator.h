#ifndef EMULATOR_H
#define EMULATOR_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <pthread.h>

#define PROCCESSOR_SPEED_HZ 10000
#define DEFINE_INSTRUCTION(name, args) {name, args}

extern FILE* input_file;
extern uint8_t memory[];
extern uint8_t emulator_running;

extern uint32_t registers[];
extern struct flags cflags;

extern pthread_t process_thread;

struct instruction {
	char* name;
	int argc;
};

struct argument {
	uint8_t instruction : 1;
	uint8_t type : 4;
	uint8_t length : 2;
	uint8_t cast : 2;
	uint8_t offset : 1;
	uint8_t info_byte;
	uint32_t value;
};

struct flags {
	uint8_t zero : 1;
	uint8_t carry : 1;
	uint8_t overflow : 1;
};

enum {
	ZERO_ARGUMENTS,
	ONE_ARGUMENT,
	TWO_ARGUMENTS,
	N_ARGUMENTS,
	ARGUMENTS_MAX
};

enum {
	SZ_DWORD,
	SZ_WORD,
	SZ_BYTE
};

enum {
	CODE_RVALUE,
	CODE_PVALUE,
	CODE_RREG,
	CODE_PREG	
};

enum {
	I_REG_AX = 1,
	I_REG_BX,
	I_REG_CX,
	I_REG_DX,
	
	I_REG_BP,
	I_REG_SP,
	I_REG_IP,

	I_REG_I1,
	I_REG_I2,
	I_REG_I3,
	I_REG_I4,
	I_REG_MAX
};

enum {
	I_NOP_INSTRUCTION,
	
	I_MOV_INSTRUCTION,
	I_MOVA_INSTRUCTION,
	I_MOVB_INSTRUCTION,
	I_MOVC_INSTRUCTION,
	I_MOVD_INSTRUCTION,
	I_MOVSP_INSTRUCTION,
	I_MOVBP_INSTRUCTION,
	I_MOVIP_INSTRUCTION,
	I_MOVI1_INSTRUCTION,
	I_MOVI2_INSTRUCTION,
	I_MOVI3_INSTRUCTION,
	I_MOVI4_INSTRUCTION,

	I_ADD_INSTRUCTION,
	// I_ADDI1_INSTRUCTION,
	// I_ADDI2_INSTRUCTION,
	// I_ADDI3_INSTRUCTION,
	// I_ADDI4_INSTRUCTION,

	I_SUB_INSTRUCTION,
	// I_SUBI1_INSTRUCTION,
	// I_SUBI2_INSTRUCTION,
	// I_SUBI3_INSTRUCTION,
	// I_SUBI4_INSTRUCTION,

	I_MUL_INSTRUCTION,
	// I_MULI1_INSTRUCTION,
	// I_MULI2_INSTRUCTION,
	// I_MULI3_INSTRUCTION,
	// I_MULI4_INSTRUCTION,
	
	I_DIV_INSTRUCTION,
	// I_DIVI1_INSTRUCTION,
	// I_DIVI2_INSTRUCTION,
	// I_DIVI3_INSTRUCTION,
	// I_DIVI4_INSTRUCTION,
	
	I_OR_INSTRUCTION,
	// I_ORI1_INSTRUCTION,
	// I_ORI2_INSTRUCTION,
	// I_ORI3_INSTRUCTION,
	// I_ORI4_INSTRUCTION,
	
	// I_XOR_INSTRUCTION,
	// I_XORI1_INSTRUCTION,
	// I_XORI2_INSTRUCTION,
	// I_XORI3_INSTRUCTION,
	// I_XORI4_INSTRUCTION,
	
	I_AND_INSTRUCTION,
	// I_ANDI1_INSTRUCTION,
	// I_ANDI2_INSTRUCTION,
	// I_ANDI3_INSTRUCTION,
	// I_ANDI4_INSTRUCTION,
	
	I_NOT_INSTRUCTION,
	// I_NOTI1_INSTRUCTION,
	// I_NOTI2_INSTRUCTION,
	// I_NOTI3_INSTRUCTION,
	// I_NOTI4_INSTRUCTION,

	I_INT_INSTRUCTION,
	I_SIVTE_INSTRUCTION,
	I_RIVTE_INSTRUCTION,
	I_IRET_INSTRUCTION,
	I_CMP_INSTRUCTION,
	I_JMP_INSTRUCTION,
	I_CALL_INSTRUCTION,
	I_RET_INSTRUCTION,
	I_JE_INSTRUCTION,
	I_JNE_INSTRUCTION,
	I_JG_INSTRUCTION,
	I_JGE_INSTRUCTION,
	I_JL_INSTRUCTION,
	I_JLE_INSTRUCTION,
	I_JC_INSTRUCTION,
	I_JNC_INSTRUCTION,
	I_JZ_INSTRUCTION,
	I_JNZ_INSTRUCTION,
	I_PUSH_INSTRUCTION,
	I_POP_INSTRUCTION,
	
	I_DB_INSTRUCTION,
	I_DS_INSTRUCTION,

	// MUL / DIV by n*2
	I_SHR_INSTRUCTION,
	I_SHL_INSTRUCTION,
	
	I_INSTRUCTION_MAX
};

static const struct instruction ISA[] = {
	[I_NOP_INSTRUCTION] = DEFINE_INSTRUCTION("NOP", ZERO_ARGUMENTS),
	
	[I_MOV_INSTRUCTION] = DEFINE_INSTRUCTION("MOV", TWO_ARGUMENTS),
	[I_MOVA_INSTRUCTION] = DEFINE_INSTRUCTION("MOV", TWO_ARGUMENTS),
	[I_MOVB_INSTRUCTION] = DEFINE_INSTRUCTION("MOV", TWO_ARGUMENTS),
	[I_MOVC_INSTRUCTION] = DEFINE_INSTRUCTION("MOV", TWO_ARGUMENTS),
	[I_MOVD_INSTRUCTION] = DEFINE_INSTRUCTION("MOV", TWO_ARGUMENTS),
	[I_MOVSP_INSTRUCTION] = DEFINE_INSTRUCTION("MOV", TWO_ARGUMENTS),
	[I_MOVBP_INSTRUCTION] = DEFINE_INSTRUCTION("MOV", TWO_ARGUMENTS),
	[I_MOVIP_INSTRUCTION] = DEFINE_INSTRUCTION("MOV", TWO_ARGUMENTS),
	[I_MOVI1_INSTRUCTION] = DEFINE_INSTRUCTION("MOV", TWO_ARGUMENTS),
	[I_MOVI2_INSTRUCTION] = DEFINE_INSTRUCTION("MOV", TWO_ARGUMENTS),
	[I_MOVI3_INSTRUCTION] = DEFINE_INSTRUCTION("MOV", TWO_ARGUMENTS),
	[I_MOVI4_INSTRUCTION] = DEFINE_INSTRUCTION("MOV", TWO_ARGUMENTS),
	
	[I_ADD_INSTRUCTION] = DEFINE_INSTRUCTION("ADD", TWO_ARGUMENTS),
	// [I_ADDI1_INSTRUCTION] = DEFINE_INSTRUCTION("ADD", TWO_ARGUMENTS),
	// [I_ADDI2_INSTRUCTION] = DEFINE_INSTRUCTION("ADD", TWO_ARGUMENTS),
	// [I_ADDI3_INSTRUCTION] = DEFINE_INSTRUCTION("ADD", TWO_ARGUMENTS),
	// [I_ADDI4_INSTRUCTION] = DEFINE_INSTRUCTION("ADD", TWO_ARGUMENTS),

	[I_SUB_INSTRUCTION] = DEFINE_INSTRUCTION("SUB", TWO_ARGUMENTS),
	// [I_SUBI1_INSTRUCTION] = DEFINE_INSTRUCTION("SUB", TWO_ARGUMENTS),
	// [I_SUBI2_INSTRUCTION] = DEFINE_INSTRUCTION("SUB", TWO_ARGUMENTS),
	// [I_SUBI3_INSTRUCTION] = DEFINE_INSTRUCTION("SUB", TWO_ARGUMENTS),
	// [I_SUBI4_INSTRUCTION] = DEFINE_INSTRUCTION("SUB", TWO_ARGUMENTS),

	[I_MUL_INSTRUCTION] = DEFINE_INSTRUCTION("MUL", TWO_ARGUMENTS),
	// [I_MULI1_INSTRUCTION] = DEFINE_INSTRUCTION("MUL", TWO_ARGUMENTS),
	// [I_MULI2_INSTRUCTION] = DEFINE_INSTRUCTION("MUL", TWO_ARGUMENTS),
	// [I_MULI3_INSTRUCTION] = DEFINE_INSTRUCTION("MUL", TWO_ARGUMENTS),
	// [I_MULI4_INSTRUCTION] = DEFINE_INSTRUCTION("MUL", TWO_ARGUMENTS),

	[I_DIV_INSTRUCTION] = DEFINE_INSTRUCTION("DIV", TWO_ARGUMENTS),
	// [I_DIVI1_INSTRUCTION] = DEFINE_INSTRUCTION("DIV", TWO_ARGUMENTS),
	// [I_DIVI2_INSTRUCTION] = DEFINE_INSTRUCTION("DIV", TWO_ARGUMENTS),
	// [I_DIVI3_INSTRUCTION] = DEFINE_INSTRUCTION("DIV", TWO_ARGUMENTS),
	// [I_DIVI4_INSTRUCTION] = DEFINE_INSTRUCTION("DIV", TWO_ARGUMENTS),
	
	[I_SHR_INSTRUCTION] = DEFINE_INSTRUCTION("SHR", TWO_ARGUMENTS),
	// [I_SHRI1_INSTRUCTION] = DEFINE_INSTRUCTION("SHR", TWO_ARGUMENTS),
	// [I_SHRI2_INSTRUCTION] = DEFINE_INSTRUCTION("SHR", TWO_ARGUMENTS),
	// [I_SHRI3_INSTRUCTION] = DEFINE_INSTRUCTION("SHR", TWO_ARGUMENTS),
	// [I_SHRI4_INSTRUCTION] = DEFINE_INSTRUCTION("SHR", TWO_ARGUMENTS),

	[I_SHL_INSTRUCTION] = DEFINE_INSTRUCTION("SHL", TWO_ARGUMENTS),
	// [I_SHLI1_INSTRUCTION] = DEFINE_INSTRUCTION("SHL", TWO_ARGUMENTS),
	// [I_SHLI2_INSTRUCTION] = DEFINE_INSTRUCTION("SHL", TWO_ARGUMENTS),
	// [I_SHLI3_INSTRUCTION] = DEFINE_INSTRUCTION("SHL", TWO_ARGUMENTS),
	// [I_SHLI4_INSTRUCTION] = DEFINE_INSTRUCTION("SHL", TWO_ARGUMENTS),

	[I_OR_INSTRUCTION] = DEFINE_INSTRUCTION("OR", TWO_ARGUMENTS),
	// [I_ORI1_INSTRUCTION] = DEFINE_INSTRUCTION("OR", TWO_ARGUMENTS),
	// [I_ORI2_INSTRUCTION] = DEFINE_INSTRUCTION("OR", TWO_ARGUMENTS),
	// [I_ORI3_INSTRUCTION] = DEFINE_INSTRUCTION("OR", TWO_ARGUMENTS),
	// [I_ORI4_INSTRUCTION] = DEFINE_INSTRUCTION("OR", TWO_ARGUMENTS),

	[I_AND_INSTRUCTION] = DEFINE_INSTRUCTION("AND", TWO_ARGUMENTS),
	// [I_ANDI1_INSTRUCTION] = DEFINE_INSTRUCTION("AND", ONE_ARGUMENT),
	// [I_ANDI2_INSTRUCTION] = DEFINE_INSTRUCTION("AND", ONE_ARGUMENT),
	// [I_ANDI3_INSTRUCTION] = DEFINE_INSTRUCTION("AND", ONE_ARGUMENT),
	// [I_ANDI4_INSTRUCTION] = DEFINE_INSTRUCTION("AND", ONE_ARGUMENT),

	[I_NOT_INSTRUCTION] = DEFINE_INSTRUCTION("NOT", ONE_ARGUMENT),
	// [I_NOTI1_INSTRUCTION] = DEFINE_INSTRUCTION("NOT", TWO_ARGUMENTS),
	// [I_NOTI2_INSTRUCTION] = DEFINE_INSTRUCTION("NOT", TWO_ARGUMENTS),
	// [I_NOTI3_INSTRUCTION] = DEFINE_INSTRUCTION("NOT", TWO_ARGUMENTS),
	// [I_NOTI4_INSTRUCTION] = DEFINE_INSTRUCTION("NOT", TWO_ARGUMENTS),

	[I_INT_INSTRUCTION] = DEFINE_INSTRUCTION("INT", ONE_ARGUMENT),
	[I_SIVTE_INSTRUCTION] = DEFINE_INSTRUCTION("SIVTE", TWO_ARGUMENTS),
	[I_RIVTE_INSTRUCTION] = DEFINE_INSTRUCTION("RIVTE", ONE_ARGUMENT),
	[I_IRET_INSTRUCTION] = DEFINE_INSTRUCTION("IRET", ZERO_ARGUMENTS),
	[I_CMP_INSTRUCTION] = DEFINE_INSTRUCTION("CMP", TWO_ARGUMENTS),
	[I_JMP_INSTRUCTION] = DEFINE_INSTRUCTION("JMP", ONE_ARGUMENT),
	[I_CALL_INSTRUCTION] = DEFINE_INSTRUCTION("CALL", ONE_ARGUMENT),
	[I_RET_INSTRUCTION] = DEFINE_INSTRUCTION("RET", ZERO_ARGUMENTS),
	[I_JE_INSTRUCTION] = DEFINE_INSTRUCTION("JE", ONE_ARGUMENT),
	[I_JNE_INSTRUCTION] = DEFINE_INSTRUCTION("JNE", ONE_ARGUMENT),
	[I_JG_INSTRUCTION] = DEFINE_INSTRUCTION("JG", ONE_ARGUMENT),
	[I_JGE_INSTRUCTION] = DEFINE_INSTRUCTION("JGE", ONE_ARGUMENT),
	[I_JL_INSTRUCTION] = DEFINE_INSTRUCTION("JL", ONE_ARGUMENT),
	[I_JLE_INSTRUCTION] = DEFINE_INSTRUCTION("JLE", ONE_ARGUMENT),
	[I_JC_INSTRUCTION] = DEFINE_INSTRUCTION("JC", ONE_ARGUMENT),
	[I_JNC_INSTRUCTION] = DEFINE_INSTRUCTION("JNC", ONE_ARGUMENT),
	[I_JZ_INSTRUCTION] = DEFINE_INSTRUCTION("JZ", ONE_ARGUMENT),
	[I_JNZ_INSTRUCTION] = DEFINE_INSTRUCTION("JNZ", ONE_ARGUMENT),
	[I_PUSH_INSTRUCTION] = DEFINE_INSTRUCTION("PUSH", ONE_ARGUMENT),
	[I_POP_INSTRUCTION] = DEFINE_INSTRUCTION("POP", ONE_ARGUMENT),
	[I_DB_INSTRUCTION] = DEFINE_INSTRUCTION("DB", N_ARGUMENTS),
	[I_DS_INSTRUCTION] = DEFINE_INSTRUCTION("DS", N_ARGUMENTS)
};

static const char* REGISTERS[] = {
	[I_REG_AX] = "AX",
	[I_REG_BX] = "BX",
	[I_REG_CX] = "CX",
	[I_REG_DX] = "DX",
	[I_REG_BP] = "BP",
	[I_REG_SP] = "SP",
	[I_REG_IP] = "IP",
	[I_REG_I1] = "I1",
	[I_REG_I2] = "I2",
	[I_REG_I3] = "I3",
	[I_REG_I4] = "I4",
};

void* proccess_cycle(void* arg);
void init_emulator();

#endif