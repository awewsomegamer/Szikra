#ifndef UTIL_H
#define UTIL_H

#define PRINT_BINARY(byte) \
	for (int i = (size_in_bytes(byte)+1)*8 - 1; i >= 0; i--) \
		printf("%d", (byte >> i) & 1); \
	printf("\n");

#include <global.h>

void uppercaseString(char* string);
void lowercaseString(char* string);

// (c >= 0x21 && c <= 0x7E)
// ((c >= 0x41 && c <= 0x5A) || (c >= 0x61 && c <= 0x7A))
// (c >= 0x30 && c <= 0x39)

bool IS_VISUAL(char c);
bool IS_ALPHA(char c);
bool IS_DIGIT(char c);
bool IS_ALPHANUMERIC(char c);

char* filter_characters(char* string, bool(*function)(char c));

int size_in_bytes(int argument);

uint64_t hash_string(char* string);



#endif