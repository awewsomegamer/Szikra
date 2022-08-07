#ifndef UTIL_H
#define UTIL_H

#define PRINT_BINARY(byte) \
	for (int i = (size_in_bytes(byte)+1)*8 - 1; i >= 0; i--) \
		printf("%d", (byte >> i) & 1); \
	printf("\n");

#include <global.h>

void uppercaseString(char* string);
void lowercaseString(char* string);

bool IS_VISUAL(char c);
bool IS_ALPHA(char c);
bool IS_DIGIT(char c);
bool IS_ALPHANUMERIC(char c);

char* filter_characters(char* string, bool(*function)(char c));

int size_in_bytes(int argument);

uint64_t hash_string(char* string);

// int find_label_from_series(struct token* series, int i, struct label* result);
struct label* find_label_from_name(char* name, struct label* head, bool is_linked);
void insert_reference(struct token* series, int position);

#endif