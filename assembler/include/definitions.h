#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define DEFINE_INSTRUCTION(name, args) {name, args}

#include <global.h>

enum {
	// End token
	T_EOF,
	
	// Symbols
	T_ADD, 		// +
	T_SUB, 		// -
	T_MUL, 		// *
	T_DIV, 		// /
	T_COMMA, 	// ,
	T_DOT, 		// .
	T_COLON,	// :
	T_DIRECTIVE,	// !
	T_LSQR_BRACKET, // [
	T_RSQR_BRACKET, // ]
	T_LPARAN,	// (
	T_RPARAN,	// )

	// Complex construct
	T_INT,
	T_STRING,
	T_INSTRUCTION,
	T_REGISTER,
	T_SIZE
};

static const char* TOKEN_NAMES[] = {
	[T_EOF] 		= "EOF",
	[T_ADD] 		= "ADD",
	[T_SUB] 		= "SUB",
	[T_MUL] 		= "MUL",
	[T_DIV] 		= "DIV",
	[T_COMMA] 		= ",",
	[T_DOT] 		= ".",
	[T_COLON] 		= ":",
	[T_DIRECTIVE] 		= "!",
	[T_LSQR_BRACKET] 	= "[",
	[T_RSQR_BRACKET] 	= "]",
	[T_LPARAN] 		= "(",
	[T_RPARAN] 		= ")",
	[T_INT] 		= "INT",
	[T_STRING] 		= "STRING",
	[T_INSTRUCTION] 	= "INSTRUCTION",
	[T_REGISTER] 		= "REGISTER",
	[T_SIZE] 		= "SIZE"
};

struct instruction {
	char* name;
	int argc;
};

struct label {
	char* name;
	uint32_t address;
	struct label* local_labels;

	struct label* next;
};

struct token {
	int type;
	int value;
	uint8_t* extra_bytes;

	struct token* next;
};

enum {
	ZERO_ARGUMENTS,
	ONE_ARGUMENT,
	TWO_ARGUMENTS,
	N_ARGUMENTS,
	ARGUMENTS_MAX
};

enum {
	SZ_BYTE,
	SZ_WORD,
	SZ_DWORD
};

enum {
	I_REG_AX,
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
	I_ADD_INSTRUCTION,
	I_SUB_INSTRUCTION,
	I_MUL_INSTRUCTION,
	I_DIV_INSTRUCTION,
	I_OR_INSTRUCTION,
	I_XOR_INSTRUCTION,
	I_AND_INSTRUCTION,
	I_NOT_INSTRUCTION,
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
	I_INSTRUCTION_MAX
};

static const struct instruction ISA[] = {
	[I_NOP_INSTRUCTION] = DEFINE_INSTRUCTION("NOP", ZERO_ARGUMENTS),
	[I_MOV_INSTRUCTION] = DEFINE_INSTRUCTION("MOV", TWO_ARGUMENTS),
	[I_ADD_INSTRUCTION] = DEFINE_INSTRUCTION("ADD", TWO_ARGUMENTS),
	[I_SUB_INSTRUCTION] = DEFINE_INSTRUCTION("SUB", TWO_ARGUMENTS),
	[I_MUL_INSTRUCTION] = DEFINE_INSTRUCTION("MUL", TWO_ARGUMENTS),
	[I_DIV_INSTRUCTION] = DEFINE_INSTRUCTION("DIV", TWO_ARGUMENTS),
	[I_OR_INSTRUCTION] = DEFINE_INSTRUCTION("OR", TWO_ARGUMENTS),
	[I_XOR_INSTRUCTION] = DEFINE_INSTRUCTION("XOR", TWO_ARGUMENTS),
	[I_AND_INSTRUCTION] = DEFINE_INSTRUCTION("AND", TWO_ARGUMENTS),
	[I_NOT_INSTRUCTION] = DEFINE_INSTRUCTION("NOT", ONE_ARGUMENT),
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

#endif