#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <global.h>

#define DEFINE_INSTRUCTION(name, args) {name, args}

struct token {
	int value;
	int type;
	int line;
	uint8_t* extra_bytes;
};

struct instruction {
	char* name;
	int argc;
};

struct argument {
	uint32_t value;
	uint8_t instruction : 1;
	uint8_t type : 2;
	uint8_t length : 2;
	uint8_t cast : 2;
	uint8_t offset : 1;
	uint8_t reference : 1;
};

struct reference {
	uint32_t where;
	uint32_t info_offset;
	int argc;
};

struct label {
	char* name;
	uint32_t address;
	struct reference* references;
	uint32_t reference_count;
	int line;
	int index;
	uint8_t defined : 1;
};

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
	T_SEMI, 	// ;
	T_COMMENT,	// ;
	T_DIRECTIVE,	// !
	T_LSQR_BRACKET, // [
	T_RSQR_BRACKET, // ]
	T_LPARAN,	// (
	T_RPARAN,	// )
	T_HASH,		// #
	T_PERCENT,	// %
	T_NEWLINE,
	
	// Complex constructs
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
	[T_COMMA] 		= "','",
	[T_DOT] 		= "'.'",
	[T_COLON] 		= "':'",
	[T_SEMI]		= "';'",
	[T_COMMENT] 		= "';'",
	[T_DIRECTIVE] 		= "'!'",
	[T_LSQR_BRACKET] 	= "'['",
	[T_RSQR_BRACKET] 	= "']'",
	[T_LPARAN] 		= "'('",
	[T_RPARAN] 		= "')'",
	[T_HASH]		= "'#'",
	[T_PERCENT]		= "'%'",
	[T_NEWLINE] 		= "'\\n'",
	[T_INT] 		= "INT",
	[T_STRING] 		= "STRING",
	[T_INSTRUCTION] 	= "INSTRUCTION",
	[T_REGISTER] 		= "REGISTER",
	[T_SIZE] 		= "SIZE"
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
	I_DIRECTIVE_AT,
	I_DIRECTIVE_PUSHWRH,
	I_DIRECTIVE_POPWRH,
	I_DIRECTIVE_DATA,
	I_DIRECTIVE_TEXT,
	I_DIRECTIVE_MAX
};

// ISA for V2 encoding
enum {
	I_NOP_INSTRUCTION,
	
	I_MOV_INSTRUCTION,
	I_MOVR_INSTRUCTION,

	I_ADD_INSTRUCTION,
	I_ADDR_INSTRUCTION,
	I_SUB_INSTRUCTION,
	I_SUBR_INSTRUCTION,
	I_MUL_INSTRUCTION,
	I_MULR_INSTRUCTION,
	I_DIV_INSTRUCTION,
	I_DIVR_INSTRUCTION,
	I_OR_INSTRUCTION,
	I_ORR_INSTRUCTION,
	I_AND_INSTRUCTION,
	I_ANDR_INSTRUCTION,
	I_NOT_INSTRUCTION,
	I_NOTR_INSTRUCTION,
	I_SHR_INSTRUCTION,
	I_SHRR_INSTRUCTION,
	I_SHL_INSTRUCTION,
	I_SHLR_INSTRUCTION,
	I_CMP_INSTRUCTION,
	I_CMPR_INSTRUCTION,

	I_INT_INSTRUCTION,
	I_SIVTE_INSTRUCTION, // Will not jump to 0 addresses 

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
	[I_NOP_INSTRUCTION] =   DEFINE_INSTRUCTION("NOP", ZERO_ARGUMENTS),
	
	[I_MOV_INSTRUCTION] =   DEFINE_INSTRUCTION("MOV", TWO_ARGUMENTS),
	[I_MOVR_INSTRUCTION] =  DEFINE_INSTRUCTION("MOVR", ONE_ARGUMENT),
	
	[I_ADD_INSTRUCTION] =   DEFINE_INSTRUCTION("ADD", TWO_ARGUMENTS),
	[I_ADDR_INSTRUCTION] =  DEFINE_INSTRUCTION("ADDR", ONE_ARGUMENT),
	[I_SUB_INSTRUCTION] =   DEFINE_INSTRUCTION("SUB", TWO_ARGUMENTS),
	[I_SUBR_INSTRUCTION] =  DEFINE_INSTRUCTION("SUBR", ONE_ARGUMENT),
	[I_MUL_INSTRUCTION] =   DEFINE_INSTRUCTION("MUL", TWO_ARGUMENTS),
	[I_MULR_INSTRUCTION] =  DEFINE_INSTRUCTION("MULR", ONE_ARGUMENT),
	[I_DIV_INSTRUCTION] =   DEFINE_INSTRUCTION("DIV", TWO_ARGUMENTS),
	[I_DIVR_INSTRUCTION] =  DEFINE_INSTRUCTION("DIVR", ONE_ARGUMENT),
	[I_OR_INSTRUCTION] =    DEFINE_INSTRUCTION("OR",  TWO_ARGUMENTS),
	[I_ORR_INSTRUCTION] =   DEFINE_INSTRUCTION("ORR",  ONE_ARGUMENT),
	[I_AND_INSTRUCTION] =   DEFINE_INSTRUCTION("AND", TWO_ARGUMENTS),
	[I_ANDR_INSTRUCTION] =  DEFINE_INSTRUCTION("ANDR", ONE_ARGUMENT),
	[I_NOT_INSTRUCTION] =   DEFINE_INSTRUCTION("NOT", ONE_ARGUMENT),
	[I_NOTR_INSTRUCTION] =  DEFINE_INSTRUCTION("NOTR", ONE_ARGUMENT),
	[I_SHR_INSTRUCTION] =   DEFINE_INSTRUCTION("SHR", TWO_ARGUMENTS),
	[I_SHRR_INSTRUCTION] =  DEFINE_INSTRUCTION("SHRR", ONE_ARGUMENT),
	[I_SHL_INSTRUCTION] =   DEFINE_INSTRUCTION("SHL", TWO_ARGUMENTS),
	[I_SHLR_INSTRUCTION] =  DEFINE_INSTRUCTION("SHLR", ONE_ARGUMENT),
	[I_CMP_INSTRUCTION] =   DEFINE_INSTRUCTION("CMP", TWO_ARGUMENTS),
	[I_CMPR_INSTRUCTION] =  DEFINE_INSTRUCTION("CMPR", ONE_ARGUMENT),

	[I_INT_INSTRUCTION] =   DEFINE_INSTRUCTION("INT",   ONE_ARGUMENT),
	[I_SIVTE_INSTRUCTION] = DEFINE_INSTRUCTION("SIVTE", TWO_ARGUMENTS),
	[I_RET_INSTRUCTION] =   DEFINE_INSTRUCTION("RET",   ZERO_ARGUMENTS),

	[I_JMP_INSTRUCTION] =   DEFINE_INSTRUCTION("JMP",  ONE_ARGUMENT),
	[I_CALL_INSTRUCTION] =  DEFINE_INSTRUCTION("CALL", ONE_ARGUMENT),

	// TODO: Put this in the compressor
	[I_JE_INSTRUCTION] =    DEFINE_INSTRUCTION("JE",  ONE_ARGUMENT),
	[I_JNE_INSTRUCTION] =   DEFINE_INSTRUCTION("JNE", ONE_ARGUMENT),
	[I_JG_INSTRUCTION] =    DEFINE_INSTRUCTION("JG",  ONE_ARGUMENT),
	[I_JGE_INSTRUCTION] =   DEFINE_INSTRUCTION("JGE", ONE_ARGUMENT),
	[I_JL_INSTRUCTION] =    DEFINE_INSTRUCTION("JL",  ONE_ARGUMENT),
	[I_JLE_INSTRUCTION] =   DEFINE_INSTRUCTION("JLE", ONE_ARGUMENT),
	[I_JC_INSTRUCTION] =    DEFINE_INSTRUCTION("JC",  ONE_ARGUMENT),
	[I_JNC_INSTRUCTION] =   DEFINE_INSTRUCTION("JNC", ONE_ARGUMENT),
	[I_JZ_INSTRUCTION] =    DEFINE_INSTRUCTION("JZ",  ONE_ARGUMENT),
	[I_JNZ_INSTRUCTION] =   DEFINE_INSTRUCTION("JNZ", ONE_ARGUMENT),

	[I_PUSH_INSTRUCTION] =  DEFINE_INSTRUCTION("PUSH", ONE_ARGUMENT),
	[I_POP_INSTRUCTION] =   DEFINE_INSTRUCTION("POP",  ONE_ARGUMENT),

	// Extraneous
	[I_DB_INSTRUCTION] = DEFINE_INSTRUCTION("DB", N_ARGUMENTS),
	[I_DS_INSTRUCTION] = DEFINE_INSTRUCTION("DS", N_ARGUMENTS)
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
// End

// ISA for V3 encoding
enum {
	V3_I_NOP_INSTRUCTION,
	V3_I_MOV_INSTRUCTION,
	V3_I_ADD_INSTRUCTION,
	V3_I_SUB_INSTRUCTION,
	V3_I_MUL_INSTRUCTION,
	V3_I_DIV_INSTRUCTION,
	V3_I_OR_INSTRUCTION,
	V3_I_AND_INSTRUCTION,
	V3_I_NOT_INSTRUCTION,
	V3_I_SHR_INSTRUCTION,
	V3_I_SHL_INSTRUCTION,
	V3_I_CMP_INSTRUCTION,
	V3_I_INT_INSTRUCTION,
	V3_I_SIVTE_INSTRUCTION, // Will not jump to 0 addresses 
	V3_I_JMP_INSTRUCTION,
	V3_I_CALL_INSTRUCTION,
	V3_I_RET_INSTRUCTION,
	V3_I_JE_INSTRUCTION,
	V3_I_JNE_INSTRUCTION,
	V3_I_JG_INSTRUCTION,
	V3_I_JGE_INSTRUCTION,
	V3_I_JL_INSTRUCTION,
	V3_I_JLE_INSTRUCTION,
	V3_I_JC_INSTRUCTION,
	V3_I_JNC_INSTRUCTION,
	V3_I_JZ_INSTRUCTION,
	V3_I_JNZ_INSTRUCTION,
	V3_I_PUSH_INSTRUCTION,
	V3_I_POP_INSTRUCTION,

	V3_I_RDPR_INSTRUCTION,
	V3_I_WRPR_INSTRUCTION,
	
	V3_I_DB_INSTRUCTION,
	V3_I_DS_INSTRUCTION,
	V3_I_INSTRUCTION_MAX
};

static const struct instruction V3_ISA[] = {
	[V3_I_NOP_INSTRUCTION] =   DEFINE_INSTRUCTION("NOP", ZERO_ARGUMENTS),
	[V3_I_MOV_INSTRUCTION] =   DEFINE_INSTRUCTION("MOV", TWO_ARGUMENTS),
	[V3_I_ADD_INSTRUCTION] =   DEFINE_INSTRUCTION("ADD", TWO_ARGUMENTS),
	[V3_I_SUB_INSTRUCTION] =   DEFINE_INSTRUCTION("SUB", TWO_ARGUMENTS),
	[V3_I_MUL_INSTRUCTION] =   DEFINE_INSTRUCTION("MUL", TWO_ARGUMENTS),
	[V3_I_DIV_INSTRUCTION] =   DEFINE_INSTRUCTION("DIV", TWO_ARGUMENTS),
	[V3_I_OR_INSTRUCTION] =    DEFINE_INSTRUCTION("OR",  TWO_ARGUMENTS),
	[V3_I_AND_INSTRUCTION] =   DEFINE_INSTRUCTION("AND", TWO_ARGUMENTS),
	[V3_I_NOT_INSTRUCTION] =   DEFINE_INSTRUCTION("NOT", ONE_ARGUMENT),
	[V3_I_SHR_INSTRUCTION] =   DEFINE_INSTRUCTION("SHR", TWO_ARGUMENTS),
	[V3_I_SHL_INSTRUCTION] =   DEFINE_INSTRUCTION("SHL", TWO_ARGUMENTS),
	[V3_I_CMP_INSTRUCTION] =   DEFINE_INSTRUCTION("CMP", TWO_ARGUMENTS),

	[V3_I_INT_INSTRUCTION] =   DEFINE_INSTRUCTION("INT",   ONE_ARGUMENT),
	[V3_I_SIVTE_INSTRUCTION] = DEFINE_INSTRUCTION("SIVTE", TWO_ARGUMENTS),
	[V3_I_RET_INSTRUCTION] =   DEFINE_INSTRUCTION("RET",   ZERO_ARGUMENTS),
	[V3_I_JMP_INSTRUCTION] =   DEFINE_INSTRUCTION("JMP",  ONE_ARGUMENT),
	[V3_I_CALL_INSTRUCTION] =  DEFINE_INSTRUCTION("CALL", ONE_ARGUMENT),

	[V3_I_JE_INSTRUCTION] =    DEFINE_INSTRUCTION("JE",  ONE_ARGUMENT),
	[V3_I_JNE_INSTRUCTION] =   DEFINE_INSTRUCTION("JNE", ONE_ARGUMENT),
	[V3_I_JG_INSTRUCTION] =    DEFINE_INSTRUCTION("JG",  ONE_ARGUMENT),
	[V3_I_JGE_INSTRUCTION] =   DEFINE_INSTRUCTION("JGE", ONE_ARGUMENT),
	[V3_I_JL_INSTRUCTION] =    DEFINE_INSTRUCTION("JL",  ONE_ARGUMENT),
	[V3_I_JLE_INSTRUCTION] =   DEFINE_INSTRUCTION("JLE", ONE_ARGUMENT),
	[V3_I_JC_INSTRUCTION] =    DEFINE_INSTRUCTION("JC",  ONE_ARGUMENT),
	[V3_I_JNC_INSTRUCTION] =   DEFINE_INSTRUCTION("JNC", ONE_ARGUMENT),
	[V3_I_JZ_INSTRUCTION] =    DEFINE_INSTRUCTION("JZ",  ONE_ARGUMENT),
	[V3_I_JNZ_INSTRUCTION] =   DEFINE_INSTRUCTION("JNZ", ONE_ARGUMENT),
	[V3_I_PUSH_INSTRUCTION] =  DEFINE_INSTRUCTION("PUSH", ONE_ARGUMENT),
	[V3_I_POP_INSTRUCTION] =   DEFINE_INSTRUCTION("POP",  ONE_ARGUMENT),

	[V3_I_RDPR_INSTRUCTION] =  DEFINE_INSTRUCTION("RDPR", TWO_ARGUMENTS),
	[V3_I_WRPR_INSTRUCTION] =  DEFINE_INSTRUCTION("WRPR",  TWO_ARGUMENTS),
	
	[V3_I_DB_INSTRUCTION] = DEFINE_INSTRUCTION("DB", N_ARGUMENTS),
	[V3_I_DS_INSTRUCTION] = DEFINE_INSTRUCTION("DS", N_ARGUMENTS)
};

enum {
	V3_I_REG_AR = 1,
	V3_I_REG_BR,
	V3_I_REG_CR,
	V3_I_REG_DR,
	V3_I_REG_ER,
	V3_I_REG_FR,
	V3_I_REG_GR,
	V3_I_REG_HR,
	V3_I_REG_BP,
	V3_I_REG_SP,
	V3_I_REG_IP,
	V3_I_REG_I1,
	V3_I_REG_I2,
	V3_I_REG_I3,
	V3_I_REG_I4,
	V3_I_REG_I5,
	V3_I_REG_I6,
	V3_I_REG_I7,
	V3_I_REG_I8,
	V3_I_REG_MAX
};

static const char* V3_REGISTERS[] = {
	[V3_I_REG_AR] = "AR",
	[V3_I_REG_BR] = "BR",
	[V3_I_REG_CR] = "CR",
	[V3_I_REG_DR] = "DR",
	[V3_I_REG_ER] = "AR",
	[V3_I_REG_FR] = "BR",
	[V3_I_REG_GR] = "CR",
	[V3_I_REG_HR] = "DR",

	[V3_I_REG_BP] = "BP",
	[V3_I_REG_SP] = "SP",
	[V3_I_REG_IP] = "IP",

	[V3_I_REG_I1] = "I1",
	[V3_I_REG_I2] = "I2",
	[V3_I_REG_I3] = "I3",
	[V3_I_REG_I4] = "I4",
	[V3_I_REG_I5] = "I5",
	[V3_I_REG_I6] = "I6",
	[V3_I_REG_I7] = "I7",
	[V3_I_REG_I8] = "I8",
};
// End


static const char* DIRECTIVES[] = {
	[I_DIRECTIVE_AT] = "AT",
	[I_DIRECTIVE_PUSHWRH] = "PUSHWRH",
	[I_DIRECTIVE_POPWRH] = "POPWRH",
	[I_DIRECTIVE_DATA] = "DATA",
	[I_DIRECTIVE_TEXT] = "TEXT",
};

#endif