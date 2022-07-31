#include <assembly.h>

void assemble(struct token* list){
	struct token* current = list;
	
	int result = 0;

	while (current != NULL){
		if (current->type == T_INT){
			result = current->value;
		} else if (current->type == T_ADD){
			result += current->next->value;
			current = current->next;
		}
			
		current = current->next;
	}

	printf("Result on line %d: %d\n", _line - 1, result);
}