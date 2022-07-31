#include <instructions.h>

void init_instructions(){
	instruction_list[I_NOP_INSTRUCTION] = NOP_INSTRUCTION;
	instruction_list[I_MOV_INSTRUCTION] = MOV_INSTRUCTION;
	instruction_list[I_ADD_INSTRUCTION] = ADD_INSTRUCTION;
	instruction_list[I_SUB_INSTRUCTION] = SUB_INSTRUCTION;
	instruction_list[I_MUL_INSTRUCTION] = MUL_INSTRUCTION;
	instruction_list[I_DIV_INSTRUCTION] = DIV_INSTRUCTION;
	instruction_list[I_OR_INSTRUCTION] = OR_INSTRUCTION;
	instruction_list[I_XOR_INSTRUCTION] = XOR_INSTRUCTION;
	instruction_list[I_AND_INSTRUCTION] = AND_INSTRUCTION;
	instruction_list[I_NOT_INSTRUCTION] = NOT_INSTRUCTION;
	instruction_list[I_INT_INSTRUCTION] = INT_INSTRUCTION;
	instruction_list[I_SIVTE_INSTRUCTION] = SIVTE_INSTRUCTION;
	instruction_list[I_RIVTE_INSTRUCTION] = RIVTE_INSTRUCTION;
	instruction_list[I_IRET_INSTRUCTION] = IRET_INSTRUCTION;
	instruction_list[I_CMP_INSTRUCTION] = CMP_INSTRUCTION;
	instruction_list[I_JMP_INSTRUCTION] = JMP_INSTRUCTION;
	instruction_list[I_CALL_INSTRUCTION] = CALL_INSTRUCTION;
	instruction_list[I_RET_INSTRUCTION] = RET_INSTRUCTION;
	instruction_list[I_JE_INSTRUCTION] = JE_INSTRUCTION;
	instruction_list[I_JNE_INSTRUCTION] = JNE_INSTRUCTION;
	instruction_list[I_JG_INSTRUCTION] = JG_INSTRUCTION;
	instruction_list[I_JGE_INSTRUCTION] = JGE_INSTRUCTION;
	instruction_list[I_JL_INSTRUCTION] = JL_INSTRUCTION;
	instruction_list[I_JLE_INSTRUCTION] = JLE_INSTRUCTION;
	instruction_list[I_JC_INSTRUCTION] = JC_INSTRUCTION;
	instruction_list[I_JNC_INSTRUCTION] = JNC_INSTRUCTION;
	instruction_list[I_JZ_INSTRUCTION] = JZ_INSTRUCTION;
	instruction_list[I_JNZ_INSTRUCTION] = JNZ_INSTRUCTION;
	instruction_list[I_PUSH_INSTRUCTION] = PUSH_INSTRUCTION;
	instruction_list[I_POP_INSTRUCTION] = POP_INSTRUCTION;
	instruction_list[I_DB_INSTRUCTION] = DB_INSTRUCTION;
	instruction_list[I_DS_INSTRUCTION] = DS_INSTRUCTION;
}

void NOP_INSTRUCTION(struct token tokens[]){
	
}

void MOV_INSTRUCTION(struct token tokens[]){

}

void ADD_INSTRUCTION(struct token tokens[]){

}

void SUB_INSTRUCTION(struct token tokens[]){

}

void MUL_INSTRUCTION(struct token tokens[]){

}

void DIV_INSTRUCTION(struct token tokens[]){

}

void OR_INSTRUCTION(struct token tokens[]){

}

void XOR_INSTRUCTION(struct token tokens[]){

}

void AND_INSTRUCTION(struct token tokens[]){

}

void NOT_INSTRUCTION(struct token tokens[]){

}

void INT_INSTRUCTION(struct token tokens[]){

}

void SIVTE_INSTRUCTION(struct token tokens[]){

}

void RIVTE_INSTRUCTION(struct token tokens[]){

}

void IRET_INSTRUCTION(struct token tokens[]){

}

void CMP_INSTRUCTION(struct token tokens[]){

}

void JMP_INSTRUCTION(struct token tokens[]){

}

void CALL_INSTRUCTION(struct token tokens[]){

}

void RET_INSTRUCTION(struct token tokens[]){

}

void JE_INSTRUCTION(struct token tokens[]){

}

void JNE_INSTRUCTION(struct token tokens[]){

}

void JG_INSTRUCTION(struct token tokens[]){

}

void JGE_INSTRUCTION(struct token tokens[]){

}

void JL_INSTRUCTION(struct token tokens[]){

}

void JLE_INSTRUCTION(struct token tokens[]){

}

void JC_INSTRUCTION(struct token tokens[]){

}

void JNC_INSTRUCTION(struct token tokens[]){

}

void JZ_INSTRUCTION(struct token tokens[]){

}

void JNZ_INSTRUCTION(struct token tokens[]){

}

void PUSH_INSTRUCTION(struct token tokens[]){

}

void POP_INSTRUCTION(struct token tokens[]){

}

void DB_INSTRUCTION(struct token tokens[]){

}

void DS_INSTRUCTION(struct token tokens[]){

}
