#include <assembly.h>
#include <instructions.h>
#include <writer.h>

// Preform a directive operation
// !AT V1 - Place writer at V1
// !TEXT - Define text section
// !DATA - Define data section
void do_directive(struct token tokens[], int* i){
	if (tokens[*i + 1].type != T_STRING){
		error("Improper directive");
		return;
	}
		
	switch (hash_string(tokens[1].extra_bytes)){
	case AT_DIRECTIVE_HASH:
		debug("FOUND AT DIRECTIVE");
		set_write_position(tokens[*i + 2].value);

		*i += 3;

		break;

	case TEXT_DIRECTIVE_HASH:
		debug("FOUND TEXT DIRECTIVE");


		break;

	case DATA_DIRECTIVE_HASH:
		debug("FOUND DATA DIRECTIVE");

		
		break;
	}
}

void create_label_node(char* name, int address){
	// Check if label exists
	struct label* current = _labels;
	while (current != NULL){
		if (current->name != NULL && strcmp(current->name, name) == 0){
			error("Label %s is already declared on line %d", current->line);
			return;
		}
		
		current = current->next;
	}

	debug("Creating label with name %s and address %d", name, address);

	// Label has not been declared so declare it
	_current_label->address = address;
	_current_label->name = strdup(name);
	_current_label->line = _line;
	
	struct label* next = malloc(sizeof(struct label));
	next->name = NULL;

	_current_label->next = next;

	_current_label = next;
}

// Take given tokens and assemble them into values
void assemble(struct token* list, int count){
	// Convert linked list into array
	struct token* current = list;
	
	struct token tokens[count];
	
	debug("-------");

	debug("ASSEMBLING:");

	struct token* next;
	int i = 0;

	while (current != NULL){
		next = current->next;
		
		tokens[i].type = current->type;
		tokens[i].value = current->value;
		tokens[i].line = current->line;

		if (current->extra_bytes != NULL){
			tokens[i].extra_bytes = malloc(sizeof(current->extra_bytes));
			memcpy(tokens[i].extra_bytes, current->extra_bytes, sizeof(current->extra_bytes));
		}

		printf("%s (%d) %d ", TOKEN_NAMES[tokens[i].type], tokens[i].type, tokens[i].value);

		i++;
		
		free(current);

		current = next;
	}
	
	printf("\n");

	debug("-------");

	// Format instructions	   : instrucion <...>
	// Format directives       : !DIRECTIVE <...>
	// Format local labels     : .LABEL:
	// Format labels           : LABEL:
	i = 0;

	while (i < count - 1){
		_line = tokens[i].line;

		switch (tokens[i].type){
		case T_INSTRUCTION:
			// Generate instruction
			(*instruction_list[ISA[tokens[i].value].argc])(tokens, &i);

			break;
		
		case T_DIRECTIVE:
			// Do directive
			if (count > 1)
				do_directive(tokens, &i);
			else
				error("Improper directive");

			break;

		case T_DOT:
			// Local label
			if (tokens[i + 1].type == T_STRING && tokens[i + 2].type == T_COLON){
				

				i += 3;
			} else {
				error("Could not define local label %s", tokens[i].extra_bytes);	
			}

			break;	
		case T_STRING:
			// Possible label
			if (tokens[i + 1].type == T_COLON){
				// Label
				create_label_node(tokens[i].extra_bytes, get_write_position());

				i += 2;
			} else {
				error("Could not define label %s", tokens[i].extra_bytes);
				i += 2;
			}

			break;
		}
	}
}