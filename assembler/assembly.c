#include <assembly.h>
#include <instructions.h>

// Take given tokens and assemble them into values
void assemble(struct token* list, int count){
	// Convert linked list into array
	struct token* current = list;
	
	struct token tokens[count];
	
	int i = 0;
	while (current != NULL){
		tokens[i++] = *current;
		current = current->next;
	}

	// Format instructions	   : instrucion <...>
	// Format directives       : !DIRECTIVE <...>
	// Format local labels     : .LABEL:
	// Format labels           : LABEL:
	switch (tokens[0].type){
	case T_INSTRUCTION:
		// Generate instruction
		(*instruction_list[ISA[tokens[0].value].argc])(tokens);

		break;
	
	case T_DIRECTIVE:
		// Do directive
		

		break;

	case T_DOT:
		// Local label
		if (count > 2 && tokens[1].type == T_STRING && tokens[count - 1].type == T_COLON){
			// Local Label
		} else {
			error("Could not define local label on line %d", _line);	
		}

		break;	
	case T_STRING:
		// Possible label
		if (count > 1 && tokens[count - 1].type == T_COLON){
			// Label
		} else {
			error("Could not define label on line %d", _line);
		}

		break;
	}

}