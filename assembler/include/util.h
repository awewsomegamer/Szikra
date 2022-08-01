#ifndef UTIL_H
#define UTIL_H

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

#endif