#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <global.h>

void(*instruction_list[256])(struct token[]);

void init_instructions();

void NOP_INSTRUCTION(struct token tokens[]);
void MOV_INSTRUCTION(struct token tokens[]);
void ADD_INSTRUCTION(struct token tokens[]);
void SUB_INSTRUCTION(struct token tokens[]);
void MUL_INSTRUCTION(struct token tokens[]);
void DIV_INSTRUCTION(struct token tokens[]);
void OR_INSTRUCTION(struct token tokens[]);
void XOR_INSTRUCTION(struct token tokens[]);
void AND_INSTRUCTION(struct token tokens[]);
void NOT_INSTRUCTION(struct token tokens[]);
void INT_INSTRUCTION(struct token tokens[]);
void SIVTE_INSTRUCTION(struct token tokens[]);
void RIVTE_INSTRUCTION(struct token tokens[]);
void IRET_INSTRUCTION(struct token tokens[]);
void CMP_INSTRUCTION(struct token tokens[]);
void JMP_INSTRUCTION(struct token tokens[]);
void CALL_INSTRUCTION(struct token tokens[]);
void RET_INSTRUCTION(struct token tokens[]);
void JE_INSTRUCTION(struct token tokens[]);
void JNE_INSTRUCTION(struct token tokens[]);
void JG_INSTRUCTION(struct token tokens[]);
void JGE_INSTRUCTION(struct token tokens[]);
void JL_INSTRUCTION(struct token tokens[]);
void JLE_INSTRUCTION(struct token tokens[]);
void JC_INSTRUCTION(struct token tokens[]);
void JNC_INSTRUCTION(struct token tokens[]);
void JZ_INSTRUCTION(struct token tokens[]);
void JNZ_INSTRUCTION(struct token tokens[]);
void PUSH_INSTRUCTION(struct token tokens[]);
void POP_INSTRUCTION(struct token tokens[]);
void DB_INSTRUCTION(struct token tokens[]);
void DS_INSTRUCTION(struct token tokens[]);

#endif