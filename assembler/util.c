#include <util.h>

void uppercaseString(char* string){
	for (int i = 0; i < strlen(string); i++)
		*(string + i) = toupper(*(string + i));
}

void lowercaseString(char* string){
	for (int i = 0; i <= strlen(string); i++)
		*(string + i) = tolower(*(string + i));
}

bool IS_VISUAL(char c){
	return (c >= 0x21 && c <= 0x7E);
}

bool IS_ALPHA(char c){
	return ((c >= 0x41 && c <= 0x5A) || (c >= 0x61 && c <= 0x7A));
}

bool IS_DIGIT(char c){
	return (c >= 0x30 && c <= 0x39);
}

bool IS_ALPHANUMERIC(char c){
	return (IS_ALPHA(c) || IS_DIGIT(c));
}

char* filter_characters(char* string, bool(*function)(char c)){
	char buffer[strlen(string) + 1];
	memset(buffer, 0, strlen(string) + 1);
	
	int buffer_i = 0;
	for (int i = 0; i < strlen(string); i++)
		if ((*function)(*(string + i)))
			buffer[buffer_i++] = *(string + i);
	
	char* str = malloc(strlen(buffer) + 1);
	memset(str, 0, strlen(buffer) + 1);
	strcpy(str, buffer);

	return str;
}

int size_in_bytes(int argument){
	int i = 0;

	while (argument != 0){
		argument >>= 8;
		if (argument != 0) i++;
	}

	return i;
}

// DJB2
uint64_t hash_string(char* string){
	uint64_t hash = 5381;
	int c;

	while (c = *string++)
		hash = ((hash << 5) + hash) + c;

	return hash;
}

// series: a series of tokens that are valid to a label
// i: should be 0, used for recursion
// result: a pointer to where the result should be placed
int find_label_from_series(struct token series[], int i, struct label* result){
	switch (series[i].type){
	case T_DOT:
		if (result != NULL){
			// Result is not NULL, thus this is a local label of a global label that has been specified
		} else {
			// Result is NULL, thus this is a local label of the current label
			

		}

		break;

	case T_STRING:
		// LABEL
		// could also be LABEL.etc.etc.etc
		
		struct label* found = NULL;

		// Label was not found, return NULL
		if ((found = find_label_from_name(series[i].extra_bytes, _labels, false, 0)) == NULL)
			return 0;

		result = found;

		// Recurse to find local label
		if (series[i + 1].type == T_DOT)
			find_label_from_series(series, i + 1, result);

		// Label successfully found, is in given variable result
		return 1;
	}

	// Did not manage to enter series
	return -1;
}

struct label* find_label_from_name(char* name, struct label* head, bool is_linked, int length){
	if (is_linked){
		struct label* current = head;

		while (current != NULL){
			if (strcmp(current->name, name) == 0)
				return current;

			current = current->next;
		}
	} else {
		for (int i = 0; i < length; i++){
			struct label current = *(head + i);

			if (strcmp(current.name, name) == 0)
				return (head + i);
		}
	}

	return NULL;
}
