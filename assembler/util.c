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
