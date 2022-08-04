#include <assembly.h>
#include <instructions.h>
#include <writer.h>

// Preform a directive operation
// !AT V1 - Place writer at V1
// !TEXT - Define text section
// !DATA - Define data section
void do_directive(struct token tokens[]){
	if (tokens[1].type != T_STRING){
		error("Improper directive");
		return;
	}
		
	switch (hash_string(tokens[1].extra_bytes)){
	case AT_DIRECTIVE_HASH:
		debug("FOUND AT DIRECTIVE");
		set_write_position(tokens[2].value);

		break;

	case TEXT_DIRECTIVE_HASH:
		debug("FOUND TEXT DIRECTIVE");
		break;

	case DATA_DIRECTIVE_HASH:
		debug("FOUND DATA DIRECTIVE");
		break;
	}
}

// Take given tokens and assemble them into values
void assemble(struct token* list, int count){
	// Convert linked list into array
	struct token* current = list;
	
	struct token tokens[count];
	
	debug("-------");

	debug("ASSEMBLING:");
	int i = 0;
	while (current != NULL && current->type != T_COMMENT){
		tokens[i++] = *current;
		printf("%s (%d) ", TOKEN_NAMES[current->type], current->type);

		current = current->next;
	}
	
	printf("\n");

	debug("-------");

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
		if (count > 1)
			do_directive(tokens);
		else
			error("Improper directive");

		break;

	case T_DOT:
		// Local label
		if (count > 2 && tokens[1].type == T_STRING && tokens[count - 1].type == T_COLON){
			// Local Label
		} else {
			error("Could not define local label %s", tokens[0].extra_bytes);	
		}

		break;	
	case T_STRING:
		// Possible label
		if (count > 1 && tokens[count - 1].type == T_COLON){
			// Label
		} else {
			// error("Could not define label %s", tokens[0].extra_bytes);
		}

		break;
	}

}